#ifndef SHARED_STATE_H
#define SHARED_STATE_H
// State that is shared between different states of the program

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 128
#define DISPLAY_ROWS 16

#include <inttypes.h>
#include <oledm/bitmap.h>

// System encountered an error and can not continue
#define STATE_FATAL 0
#define STATE_FIRST_END_POINT_SELECT 1

struct SharedState {
  struct Bitmap bitmap;

  // incremented by one each frame
  uint32_t frame_idx;

  // error message displayed with STATE_FATAL
  char fatal_err[32];

  // state modules can set this to non-zero to indicate
  // they are controlling the main led
  uint8_t main_led_claimed;

  uint8_t state;
};

#endif
