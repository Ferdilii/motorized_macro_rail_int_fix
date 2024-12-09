#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include <stdint.h>

#ifdef TEST
typedef int spin_lock_t;
#define spin_lock_claim_unused(X) 0
#define spin_lock_init(X) 0
#define spin_lock_unsafe_blocking(X)
#define spin_unlock_unsafe(X)
uint32_t time_us_32(void);
#else
#include "hardware/sync.h"
#endif

// Current state of the motor controller.
struct MotorControl {
  // motor position and current position are separated by the
  // concept of backlash - where reversing direction eats up
  // some "play" before the mechanics re-engage.
  //
  // Motor pos is 2x the scale of current pos (400 vs 200 units per 360 deg typically)
  float motor_pos;  // user r/w, loop r/w
  float current_pos;  // user r/w, loop r/w

  // target position determines where the motor should go
  float target_pos;  // user r/w, loop r

  // backlash, in steps, determines the amount of movement
  // that corresponds to consuming backlash
  uint32_t backlash;  // user r/w, loop r

  // acceleration, in steps/second*2, determines how quickly
  // the motor should get up to max speed and how early it
  // should start to speed down.
  uint32_t acceleration;  // user r/w, loop r
  // target velocity, in steps/second, determines the maximum
  // velocity of the motor
  uint32_t max_velocity;  // user r/w, loop r

  // velocity, in steps/second is the current stepper motor
  // velocity
  float velocity;  // user r, loop r/w

  // used to send updates to the control logic
  spin_lock_t* lock;  // user r/w, loop r/w
  // used to time acceleration and velocity of the loop
  uint32_t loop_us; // loop r/w

  // used to help stop the loop
  uint8_t running;  // user r/w, loop r/w

  // If true, the unit is in jog mode and simply adds velocity to the current
  // position on each step
  uint8_t jog_mode;
  float jog_velocity;

  // idle frames are used to avoid going to sleep immediately
  uint32_t idle_frames;
  uint8_t sleep;
  int8_t direction;  // current motor direction
};

// initialize the passed structure
void motor_control_init(
    struct MotorControl* mc,
    uint32_t backlash,
    uint32_t max_velocity,
    uint32_t acceleration);

// Starts the motor control loop on CPU1
void motor_control_start_loop(struct MotorControl* mc);
// stops the motor control loop.  The control loop will not
// stop if motor_control_check_stopped is false.
void motor_control_stop_loop(struct MotorControl* mc);

// Stops the motor control loop
void motor_control_stop(struct MotorControl* mc);

#ifdef TEST
// runs one iteration of the motor control loop.
void motor_control_loop(struct MotorControl* mc);
#endif

// This is used to change the maximum velocity in steps/sec.
void motor_control_set_max_velocity(struct MotorControl* mc, uint32_t steps_per_sec);
void motor_control_set_acceleration(struct MotorControl* mc, uint32_t acc);

// This is used to change the position of the target.  It only functions
// if the current velocity is near zero.  This is a simple way to avoid
// putting the control algorithm into impossible-to-solve scenerios (example
// the velocity is +200 and you instruct it to stop ten steps back)
//
// Returns a 1 if the target position was changed
uint8_t motor_control_try_target_position(struct MotorControl* mc, int32_t p);

// Changes the backlash value if the motor is stopped.
uint8_t motor_control_try_backlash(struct MotorControl* mc, int32_t p);

// check to see if the motor is stopped
uint8_t motor_control_check_stopped(struct MotorControl* mc);

// Puts the algorithm into jog mode and pushes the velocity toward
// the requested steps per second.  If doing so would exceed the maximum
// set acceleration, then the maximum acceleration is used instead.
void motor_control_set_jog_velocity(struct MotorControl* mc, float steps_per_sec);

// Changes current_pos without moving the motor.  motor_pos is moved by an equal delta.
uint8_t motor_control_try_set_current_pos(struct MotorControl* mc, int32_t p);

// creates a snapshot copy of the given structure for reading
// information without needed a lock
void motor_control_snapshot(struct MotorControl* dest, const struct MotorControl* src);

// estimates the time needed to travel n steps under the current velocity
// and acceleration.
uint32_t estimate_seek_time_ms(const struct MotorControl* mv, int32_t steps);

#endif

