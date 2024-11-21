#include <string.h>
#include "motor_control.h"
#include "motor_driver.h"

#ifdef TEST
void sleep_us(uint32_t unused) { }
#else
#include "pico/stdlib.h"
#endif

// Run the loop around 10khz
#define LOOP_DELAY_US 50
// sleep after 500000 us
#define IDLE_CYCLES_BEFORE_SLEEP (500000 / LOOP_DELAY_US)

static inline uint8_t near_zero(float v, float threshold) {
  return (v < threshold) && (v > -threshold);
}

static inline void sleep(struct MotorControl* mc) {
  motor_driver_step(0); // always set low if preparing for sleep
  mc->sleep = 1;
}

static inline void prepare(struct MotorControl* mc, int8_t direction) {
  if (mc->sleep) {
    mc->sleep = 0;
    mc->idle_frames = 0;
    motor_driver_wake();
  }
  if (mc->direction != direction) {
    mc->direction = direction;
    motor_driver_dir(direction);
  }
}

void motor_control_init(
    struct MotorControl* mc,
    uint32_t backlash,
    uint32_t max_velocity,
    uint32_t acceleration) {
  memset(mc, 0, sizeof(*mc));
  mc->backlash = backlash;
  mc->max_velocity = max_velocity;
  mc->acceleration = acceleration;
  int lock_num = spin_lock_claim_unused(1);
  mc->lock = spin_lock_init(lock_num);
  motor_driver_init();
  motor_driver_sleep();
  mc->sleep = 1;
}

static float update_physics_normalized(
    float dist, float vel, float acc, float max_vel, float delta_seconds) {
  // first, apply the acceleration normally
  vel += acc * delta_seconds;
  if (vel > max_vel) {
    vel = max_vel;
  }

  // Now check, if we apply maximum deceleration, will we end up going further
  // or shorter than dist.  If it's further, then deccelleration (braking) needs to
  // be applied.
  const float braking_dist = vel * vel / (2.0 * acc);
  if (braking_dist > dist) {
    // undo the overshoot so a proper calculation is not biased.
    vel -= acc * delta_seconds;
    // now add a very small amount of overshoot
    dist += 0.02;
    // here we need to recalculate the needed decelleration so that we hit the target
    // right on.  In otherwords:
    //   dist = vel * vel / (2.0 * newacc)
    //   dist * 2.0 * newacc = vel * vel
    //   newacc = vel * vel / (2.0 * dist)
    // also make it negative
    acc = -(vel * vel / (2.0 * dist));
    vel += acc * delta_seconds;
  }

  return vel;
}

static void update_physics(struct MotorControl* mc, float delta_seconds) {
  // The first task is to remove the offsets and symmetry of the calculation,
  // Removing the offset means we don't have to carry it through calculations
  // and symmetry means that we don't have to do every calculation in positive
  // and negative directions.
  float dist = mc->target_pos - mc->current_pos;
  float velocity = mc->velocity;
  float acceleration = mc->acceleration;
  uint8_t invert = 0;

  if (dist < 0) {
    dist = -dist;
    velocity = -mc->velocity;
    invert = 1;
  }

  if ((dist < 0.1) && near_zero(velocity, 0.1)) {
    // close enough
    return;
  }
  velocity = update_physics_normalized(
    dist,
    velocity,
    acceleration,
    mc->max_velocity,
    delta_seconds);

  if (invert) {
    mc->velocity = -velocity;
  } else {
    mc->velocity = velocity;
  }
}

static void update_current_pos_positive(struct MotorControl* mc) {
  const float new_curr_pos = (mc->motor_pos / 2) - mc->backlash;
  if (new_curr_pos > mc->current_pos) {
    mc->current_pos = new_curr_pos;
  }
}

static void update_current_pos_negative(struct MotorControl* mc) {
  const float new_curr_pos = (mc->motor_pos / 2) + mc->backlash;
  if (new_curr_pos < mc->current_pos) {
    mc->current_pos = new_curr_pos;
  }
}

static void update_current_pos(struct MotorControl* mc) {
  if (mc->velocity > 0) {
    update_current_pos_positive(mc);
  } else if (mc->velocity < 0) {
    update_current_pos_negative(mc);
  }
}

static void update_jog_velocity(struct MotorControl* mc, float delta_seconds) {
  const float max_delta_v = mc->acceleration * delta_seconds;

  if (mc->jog_velocity > mc->velocity) {
    const float delta_v = mc->jog_velocity - mc->velocity;
    if (delta_v < max_delta_v) {
      mc->velocity = mc->jog_velocity;
    } else {
      mc->velocity += max_delta_v;
    }
  } else {
    const float delta_v = mc->velocity - mc->jog_velocity;
    if (delta_v < max_delta_v) {
      mc->velocity = mc->jog_velocity;
    } else {
      mc->velocity -= max_delta_v;
    }
  }
}

void update_position(struct MotorControl* mcp, float delta_seconds) {
  const int32_t orig_motor_pos = (int32_t)(mcp->motor_pos);
  const float new_motor_pos = mcp->motor_pos + (mcp->velocity * delta_seconds);
  const int32_t delta = (int32_t)(new_motor_pos) - orig_motor_pos;

  if (delta > 0) {
    prepare(mcp, 1);
    motor_driver_step((orig_motor_pos + delta) & 1);
  } else if (delta < 0) {
    prepare(mcp, -1);
    motor_driver_step((orig_motor_pos + delta) & 1);
  }

  if ((delta <= 1) && (delta >= -1)) {
    // the usual case
    mcp->motor_pos = new_motor_pos; 
  } else if (delta < 0) {
    // control loop is too slow
    mcp->motor_pos -= 1;
  } else {
    // control loop is too slow
    mcp->motor_pos += 1;
  }
  update_current_pos(mcp);
}

void check_for_jog_end(struct MotorControl* mcp) {
  if (near_zero(mcp->jog_velocity, 0.5) &&
      near_zero(mcp->velocity, 0.5)) {
    mcp->velocity = 0;
    mcp->jog_velocity = 0;
    mcp->jog_mode = 0;
    mcp->current_pos = (int32_t)(mcp->current_pos);
    mcp->target_pos = mcp->current_pos;
    sleep(mcp);
  }
}

void check_for_target_reached(struct MotorControl* mcp) {
  float target_dist = mcp->target_pos - mcp->current_pos;

  if (((target_dist < 0) && (mcp->velocity >= 0)) ||
      ((target_dist > 0) && (mcp->velocity <= 0))) {
    // overshot the target per-design.  Time to stop.
    mcp->velocity = 0;
    mcp->current_pos = mcp->target_pos;
    sleep(mcp);
  }
}

void motor_control_loop(struct MotorControl* mcp) {
  uint32_t prev_us = mcp->loop_us;
  mcp->loop_us = time_us_32(); // new time

  if (prev_us == 0 || (mcp->loop_us <= prev_us)) {
    return;
  }
  const float delta_seconds = (float)(mcp->loop_us - prev_us) / 1000000.0;

  if (mcp->jog_mode) {
    update_jog_velocity(mcp, delta_seconds);
    // *2 allows for appropriate velocity through the unit conversion
    update_position(mcp, delta_seconds * 2);
    check_for_jog_end(mcp);
  } else {
    update_physics(mcp, delta_seconds);
    update_position(mcp, delta_seconds * 2);
    check_for_target_reached(mcp);
  }
}

void motor_control_start(struct MotorControl* mc) {
  mc->running = 1;
  uint8_t running = 1;

  while (running) {
    spin_lock_unsafe_blocking(mc->lock);
    motor_control_loop(mc);
    running = mc->running;
    spin_unlock_unsafe(mc->lock);
    if (mc->sleep) {
      ++mc->idle_frames;
      if (mc->idle_frames >= IDLE_CYCLES_BEFORE_SLEEP) {
        motor_driver_sleep();
        mc->idle_frames = 0;
      }
    }
    sleep_us(LOOP_DELAY_US);
  }
}

void motor_control_stop(struct MotorControl* mc) {
  spin_lock_unsafe_blocking(mc->lock);
  mc->running = 0;
  spin_unlock_unsafe(mc->lock);
}

void motor_control_set_max_velocity(struct MotorControl* mc, uint32_t steps_per_sec) {
  spin_lock_unsafe_blocking(mc->lock);
  mc->max_velocity = steps_per_sec;
  spin_unlock_unsafe(mc->lock);
}

void motor_control_set_acceleration(struct MotorControl* mc, uint32_t acc) {
  spin_lock_unsafe_blocking(mc->lock);
  mc->acceleration = acc;
  spin_unlock_unsafe(mc->lock);
}

uint8_t motor_control_try_target_position(struct MotorControl* mc, int32_t p) {
  spin_lock_unsafe_blocking(mc->lock);
  uint8_t changed = 0;
  if ((mc->velocity > -0.1) && (mc->velocity < 0.1)) {
    mc->jog_mode = 0;
    mc->velocity = 0;
    mc->target_pos = p;
    changed = 1;
  }
  spin_unlock_unsafe(mc->lock);
  return changed;
}

uint8_t motor_control_check_stopped(struct MotorControl* mc) {
  spin_lock_unsafe_blocking(mc->lock);
  uint8_t stopped = 0;
  float delta = 0;
  if (!mc->jog_mode) {
    delta = mc->target_pos - mc->current_pos;
  }
  if ((delta > -1) && (delta < 1) && (mc->velocity > -0.1) && (mc->velocity < 0.1)) {
    mc->jog_mode = 0;
    mc->velocity = 0;
    stopped = 1;
  }
  spin_unlock_unsafe(mc->lock);
  return stopped;
}

uint8_t motor_control_try_backlash(struct MotorControl* mc, int32_t p) {
  spin_lock_unsafe_blocking(mc->lock);
  uint8_t changed = 0;
  if ((mc->velocity > -0.1) && (mc->velocity < 0.1)) {
    mc->jog_velocity = 0;
    mc->jog_mode = 0;
    mc->velocity = 0;
    mc->backlash = p;
    mc->motor_pos = mc->current_pos * 2;
    mc->target_pos = mc->current_pos;
    changed = 1;
  }
  spin_unlock_unsafe(mc->lock);
  return changed;
}

void motor_control_set_jog_velocity(struct MotorControl* mc, float steps_per_sec) {
  spin_lock_unsafe_blocking(mc->lock);
  mc->jog_mode = 1;
  if (steps_per_sec > mc->max_velocity) {
    steps_per_sec = mc->max_velocity;
  } else if (steps_per_sec < -((float)mc->max_velocity)) {
    steps_per_sec = -((float)mc->max_velocity);
  }
  mc->jog_velocity = steps_per_sec;
  spin_unlock_unsafe(mc->lock);
}

uint8_t motor_control_try_set_current_pos(struct MotorControl* mc, int32_t p) {
  spin_lock_unsafe_blocking(mc->lock);
  uint8_t changed = 0;
  if ((mc->velocity > -0.1) && (mc->velocity < 0.1)) {
    const float difference = mc->current_pos - p;
    mc->jog_velocity = 0;
    mc->jog_mode = 0;
    mc->velocity = 0;
    mc->current_pos = p;
    mc->target_pos = p;
    mc->motor_pos -= difference * 2;
    changed = 1;
  }
  spin_unlock_unsafe(mc->lock);
  return changed;
}

void motor_control_snapshot(struct MotorControl* dest, const struct MotorControl* src) {
  spin_lock_unsafe_blocking(src->lock);
  memcpy(dest, src, sizeof(*src));
  spin_unlock_unsafe(src->lock);
}

