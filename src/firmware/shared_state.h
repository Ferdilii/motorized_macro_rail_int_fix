#ifndef SHARED_STATE_H
#define SHARED_STATE_H
// State that is shared between different states of the program

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128
#define DISPLAY_ROWS 16

#include <inttypes.h>
#include <oledm/bitmap.h>
#include "motor_control.h"

// System encountered an error and can not continue
#define STATE_FATAL 0
#define STATE_END_POINT_SELECT 1
#define STATE_START_POINT_SELECT 2
#define STATE_SHUTTER_DELAY 3
#define STATE_SHOT_COUNT 4
#define STATE_RUNNING 6
#define STATE_MENU 7
#define STATE_MAX_VELOCITY 8
#define STATE_ACCELERATION 9
#define STATE_BACKLASH 10
#define STATE_SETTLE_SECONDS 11

// bits for buttons bit field
#define NEXT_PRESSED     0x01
#define PREVIOUS_PRESSED 0x02
//#define SHUTTER_PRESSED  0x04  currently handled in button.c exclusively

struct SharedState {
  struct Bitmap bitmap;

  // analog position (0x000-0x3FF)
  uint16_t gimbal_x_pos;
  uint16_t gimbal_y_pos;
  // digital direction (-1, 0, or 1)
  int8_t gimbal_x_dir;
  int8_t gimbal_y_dir;

  // button states.
  uint8_t button;

  struct MotorControl motor;

  // incremented by one each frame
  uint32_t frame_idx;

  // error message displayed with STATE_FATAL
  char fatal_err[32];

  int32_t start_pos;
  uint32_t shutter_delay_ms;
  uint32_t shot_count;

  // state modules can set this to non-zero to indicate
  // they are controlling the main led
  uint8_t main_led_claimed;
  uint8_t state;
};

#endif
