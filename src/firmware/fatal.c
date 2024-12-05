#include "fatal.h"
#include "oledm/font/terminus8x16.h"
#include "main_led.h"
#include <stdarg.h>
#include <stdio.h>

void fatal(struct SharedState* state, const char* format, ...) {
  va_list args;
  va_start(args, format);

  state->state = STATE_FATAL;
  vsnprintf(state->fatal_err, sizeof(state->fatal_err), format, args);
}

void fatal_update(struct SharedState* state) {
  bitmap_str(
    &(state->bitmap),
    TEXT_FONT,
    state->fatal_err,
    0,
    TEXT_HEIGHT * 4,
    bitmap_SET);

  // blink areound 5 times a second (12 frames would be close)
  state->main_led_claimed = 1;
  main_led_put(state->frame_idx & 0x08);   
}
