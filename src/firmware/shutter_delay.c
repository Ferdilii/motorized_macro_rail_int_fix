#include "shutter_delay.h"

#include <stdio.h>

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"
#include "enter_value_widget.h"
#include "render_common.h"

struct EnterValueWidget shutter_evw;

static void update(struct SharedState* ss) {
  if (ss->button & NEXT_PRESSED) {
    ss->state = STATE_SHOT_COUNT;
  } else if (ss->button & PREVIOUS_PRESSED) {
    ss->state = STATE_START_POINT_SELECT;
  } else {
    shutter_evw.value = ss->shutter_delay_ms / 100;
    enter_value_widget_update(&shutter_evw, -ss->gimbal_x_dir, -ss->gimbal_y_dir);
    ss->shutter_delay_ms = shutter_evw.value * 100;
  }
}

static void render(struct SharedState* ss) {
  struct Bitmap* bm = &(ss->bitmap);
  render_common(bm, "Shutter Delay", "StartP", "Count");
  int16_t xpos = 16;
  const int16_t ypos = 64;
  enter_value_widget_render(&shutter_evw, bm, xpos, ypos, 1); 
  const char* s = "seconds";
  // account for '00.0'
  xpos += 5 * 8;
  bitmap_str(bm, terminus8x16, s, xpos, ypos, bitmap_SET);
}

void shutter_delay_init(struct SharedState* ss) {
  ss->shutter_delay_ms = 1000;
  enter_value_widget_init(&shutter_evw, 1, 1, 999);
}

void shutter_delay_update(struct SharedState* ss) {
  update(ss);
  render(ss);
}

