#include "shutter_delay.h"

#include <stdio.h>

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"
#include "render_common.h"

static void update(struct SharedState* ss) {
}

static void render(struct SharedState* ss) {
  struct Bitmap* bm = &(ss->bitmap);
  render_title(bm, "Shutter Delay");
}

void shutter_delay_update(struct SharedState* ss) {
  update(ss);
  render(ss);
}

