#include "parameter_change.h"
#include <stdio.h>
#include <string.h>

#include "render_common.h"
#include "oledm/bitmap.h"
#include "oledm/font/terminus8x16.h"

static struct SavedSettings settings;

struct SavedSettings* parameter_change_settings(void) {
  return &settings;
}

void parameter_change_init(
    struct ParameterChange* pc,
    const char* title,
    int32_t* permanent_val,
    int32_t default_val,
    int32_t min_val,
    int32_t max_val,
    uint8_t decimal_places) {
  memcpy(&settings, saved_settings_get(), sizeof(struct SavedSettings));
  pc->title = title;
  pc->default_val = default_val;
  pc->permanent_val = permanent_val;
  pc->decimal_places = decimal_places;
  enter_value_widget_init(&(pc->evw), *permanent_val, min_val, max_val);
}

static void _update(
    struct ParameterChange* pc,
    struct SharedState* ss) {
  if (ss->button & PREVIOUS_PRESSED) {
    if (*(pc->permanent_val) != pc->evw.value) {
      *(pc->permanent_val) = pc->evw.value;
      saved_settings_write(&settings);
    }
    ss->state = STATE_MENU;
  } else if (ss->button & NEXT_PRESSED) {
    // TODO: implement a way for different settings to offer testing mechanisms.
  } else {
    enter_value_widget_update(&(pc->evw), -ss->gimbal_x_dir, -ss->gimbal_y_dir);
  }
}

static void _render(
    struct ParameterChange* pc,
    struct SharedState* ss) {
  struct Bitmap* bm = &(ss->bitmap);
  render_common(bm, pc->title, "Cancel", "Set");
  int16_t y = 64;
  bitmap_str(bm, terminus8x16, "Value", 0, y, bitmap_SET);
  enter_value_widget_render(&(pc->evw), bm, 128, y, pc->decimal_places); 
  y += 16;

  char str[32];
  sprintf(str, "Previous %7d", *(pc->permanent_val));
  bitmap_str(bm, terminus8x16, str, 0, y, bitmap_SET);
  y += 16;

  sprintf(str, "Default %8d", pc->default_val);
  bitmap_str(bm, terminus8x16, str, 0, y, bitmap_SET);
}

void parameter_change_update(
    struct ParameterChange* pc,
    struct SharedState* ss) {
  _update(pc, ss);
  _render(pc, ss);
}

