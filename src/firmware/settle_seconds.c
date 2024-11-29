#include "settle_seconds.h"
#include "pico/stdlib.h"

#include "fatal.h"
#include "parameter_change.h"
#include "saved_settings.h"
#include "motor_control.h"
#include "shutter.h"

#define STATE_IDLE 0
#define STATE_MOVING 1
#define STATE_SETTLE 2

static struct ParameterChange pc;
static uint8_t state;
static uint32_t wait_ms;

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

void settle_seconds_init(const char* title) {
  struct SavedSettings* settings = parameter_change_settings();
  state = STATE_IDLE;
  parameter_change_init(
      &pc,
      title,
      &(settings->settle_dsecs),
      DEFAULT_SETTLE_DSECS,
      0,
      600,
      1);
}

static int32_t _get_distance(void) {
  // we want to ramp to full velocity, then back down, with a little
  // pad.
  //
  // maxv = a * t
  // t = maxv / a
  // d = 0.5 * a * t * t (initial v is 0)
  // d = 0.5 * a * (maxv / a) * (maxv / a)
  // d = 0.5 * maxv * maxv / a
  // then times 2 for the ramp down
  // d = maxv * maxv / a
  struct SavedSettings* ss = parameter_change_settings();
  int32_t d = ss->max_velocity * ss->max_velocity / ss->acceleration;
  d += 25;  // add 1/8th turn
  // safety check in case maxv or a are at unexpected values
  if (d > (ss->steps_per_mm * 3)) {
    d = ss->steps_per_mm * 3;
  }
  return d;
}

static void _test_mode(struct SharedState* ss) {
  struct MotorControl motor_snap;
  struct SavedSettings* settings = parameter_change_settings();
  motor_control_snapshot(&motor_snap, &(ss->motor));

  switch (state) {
    case STATE_IDLE:
      if (ss->gimbal_y_dir > 0) {
        motor_control_try_set_current_pos(
            &(ss->motor), motor_snap.current_pos + _get_distance()); 
      } else if (ss->gimbal_y_dir < 0) {
        motor_control_try_set_current_pos(
            &(ss->motor), motor_snap.current_pos - _get_distance()); 
      }
      state = STATE_MOVING;
      break;
    case STATE_MOVING:
      if (motor_control_check_stopped(&(ss->motor))) {
        wait_ms = uptime_ms() + (settings->settle_dsecs * 100);
        state = STATE_SETTLE;
      }
      break;
    case STATE_SETTLE:
      shutter_release(20);
      state = STATE_IDLE;
      break;
    default:
      fatal(ss, "Unknown settle state: %d", state);
  }
}

void settle_seconds_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
  if (pc.test_mode) {
    _test_mode(ss);
  }
}

