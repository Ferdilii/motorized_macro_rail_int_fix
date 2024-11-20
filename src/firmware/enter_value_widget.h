#ifndef ENTER_VALUE_WIDGET_H
#define ENTER_VALUE_WIDGET_H

#include "oledm/bitmap.h"
#include <inttypes.h>

struct EnterValueWidget {
  // setup
  int32_t value;
  int32_t min_value;
  int32_t max_value;

  // state
  uint8_t current_digit;
};

void enter_value_widget_init(
    struct EnterValueWidget* evw,
    int32_t value,
    int32_t min_value,
    int32_t max_value);
void enter_value_widget_update(
    struct EnterValueWidget* evw, int8_t xdir, int8_t ydir);
void enter_value_widget_render(
    const struct EnterValueWidget* evw,
    struct Bitmap* bm,
    uint16_t endx,
    uint16_t y,
    uint8_t decimal_places);

#endif

