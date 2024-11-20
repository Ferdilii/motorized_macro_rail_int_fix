#include "enter_value_widget.h"
#include <string.h>

#include "oledm/font/terminus8x16.h"

void enter_value_widget_init(
    struct EnterValueWidget* evw,
    int32_t value,
    int32_t min_value,
    int32_t max_value) {
  memset(evw, 0, sizeof(*evw));
  evw->value = value;
  evw->min_value = min_value;
  evw->max_value = max_value;
}

static void _widget_update_value(struct EnterValueWidget* evw, int8_t ydir) {
  int32_t change = ydir;
  for (int i=0; i<evw->current_digit; ++i) {
    change *= 10;
  }
  const int32_t new_value = evw->value + change;
  if (new_value < evw->min_value) {
    evw->value = evw->min_value;
  } else if (new_value > evw->max_value) {
    evw->value = evw->max_value;
  } else {
    evw->value = new_value;
  }
}

static uint8_t _max_current_digit(const struct EnterValueWidget* evw) {
  uint8_t max_curr_digit = 0;
  int32_t v = evw->max_value;
  while (v >= 10) {
    ++max_curr_digit;
    v /= 10;
  }
  return max_curr_digit;
}

static void _widget_update_current_digit(struct EnterValueWidget* evw, int8_t xdir) {
  if (xdir > 0) {
    if (evw->current_digit < _max_current_digit(evw)) {
      ++evw->current_digit;
    }
  } else if (xdir < 0) {
    if (evw->current_digit > 0) {
      --evw->current_digit;
    }
  }
}

void enter_value_widget_update(
    struct EnterValueWidget* evw, int8_t xdir, int8_t ydir) {
  if (ydir) {
    _widget_update_value(evw, ydir);
  } else if (xdir) {
    _widget_update_current_digit(evw, xdir);
  }
}

static uint8_t _get_digit(uint32_t v, uint8_t digit) {
  while (1) {
    if (digit == 0) {
      break;
    }
    v /= 10;
    --digit;
  }
  return v % 10;
}

void enter_value_widget_render(
    const struct EnterValueWidget* evw,
    struct Bitmap* bm,
    uint16_t endx,
    uint16_t y,
    uint8_t decimal_places) {
  const int8_t digits = (int8_t)_max_current_digit(evw);
  uint16_t xpos = endx - (8 * digits) - 8;
  if (decimal_places > 0) {
    xpos -= 8;
  }
  uint32_t value = evw->value;
  char c = '-';
  if (evw->value < 0) {
    value = -evw->value;
    bitmap_strLen(bm, terminus8x16, &c, 1, xpos, y, bitmap_SET);
    xpos += 8;
  }
  for (int8_t digit = digits; digit >= 0; --digit) {
    uint8_t v = _get_digit(value, digit);
    c = '0' + v;
    bitmap_strLen(
        bm,
        terminus8x16,
        &c,
        1,
        xpos,
        y,
        digit == evw->current_digit ? bitmap_NSET : bitmap_SET);
    xpos += 8;
    if ((decimal_places > 0) && (digit == decimal_places)) {
      c = '.';
      bitmap_strLen(bm, terminus8x16, &c, 1, xpos, y, bitmap_SET);
      xpos += 8;
    }
  }
}

