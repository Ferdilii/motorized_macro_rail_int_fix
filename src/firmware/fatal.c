#include "fatal.h"
#include "oledm/font/terminus8x16.h"
#include "main_led.h"

void fatal_update(struct SharedState* state) {
  bitmap_str(
    &(state->bitmap),
    terminus8x16,
    state->fatal_err,
    0,
    4,
    bitmap_SET);

  // blink areound 5 times a second (12 frames would be close)
  state->main_led_claimed = 1;
  main_led_put(state->frame_idx & 0x08);   
}