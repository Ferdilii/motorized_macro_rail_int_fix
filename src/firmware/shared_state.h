#ifndef SHARED_STATE_H
#define SHARED_STATE_H
// State that is shared between different states of the program

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128
#define DISPLAY_ROWS 16

#include <inttypes.h>
#include <oledm/bitmap.h>
#include "motor_control.h"

// Motor parameters
#define MOTOR_BACKLASH 50
#define MAX_MOTOR_VELOCITY 150
#define MAX_MOTOR_ACCELERATION 150

// System encountered an error and can not continue
#define STATE_FATAL 0
#define STATE_END_POINT_SELECT 1
#define STATE_START_POINT_SELECT 2
#define STATE_DELAY_BETWEEN_SHOTS 3
#define STATE_SHOT_COUNT 4
#define STATE_CONFIRM_START 5
#define STATE_RUNNING 6

struct SharedState {
  struct Bitmap bitmap;

  // analog position (0x000-0x3FF)
  uint16_t gimbal_x_pos;
  uint16_t gimbal_y_pos;
  // digital direction (-1, 0, or 1)
  int8_t gimbal_x_dir;
  int8_t gimbal_y_dir;

  // button states.  These stay set until the receiver clears them.
  uint8_t next_pressed;

  struct MotorControl motor;

  // incremented by one each frame
  uint32_t frame_idx;

  // error message displayed with STATE_FATAL
  char fatal_err[32];

  int32_t start_pos;
  int32_t end_pos;

  // state modules can set this to non-zero to indicate
  // they are controlling the main led
  uint8_t main_led_claimed;
  uint8_t state;
};

#endif
