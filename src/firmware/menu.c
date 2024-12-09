#include "menu.h"

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"

#include "acceleration.h"
#include "backlash.h"
#include "max_velocity.h"
#include "render_common.h"
#include "settle_seconds.h"
#include "steps_per_mm.h"

const static char* items[] = {
  "End Position",  // idx 0-3
  "Max Vel (step/s)",  // idx 0-3
  "Accel (step/s*s)",  // idx 1-4
  "Backlash (steps)", // idx 2-5
  "Settle Seconds",   // idx 2-5
  "Steps / mm",    // idx 2-5
};

#define NUM_ITEMS (sizeof(items) / sizeof(items[0]))

static int8_t menu_idx;

static void _update(struct SharedState* ss) {
  if (ss->button & NEXT_PRESSED) {
    const int8_t index = menu_idx;
    menu_idx = 0;
    switch (index) {
      case 0:
        ss->state = STATE_END_POINT_SELECT;
        break;
      case 1:
        max_velocity_init(items[index]);
        ss->state = STATE_MAX_VELOCITY;
        break;
      case 2:
        acceleration_init(items[index]);
        ss->state = STATE_ACCELERATION;
        break;
      case 3:
        backlash_init(items[index]);
        ss->state = STATE_BACKLASH;
        break;
      case 4:
        settle_seconds_init(items[index]);
        ss->state = STATE_SETTLE_SECONDS;
        break;
      case 5:
        steps_per_mm_init(items[index]);
        ss->state = STATE_STEPS_PER_MM;
        break;
      default:
        // not yet implemented
        menu_idx = index;
    }
  } else if (ss->button & PREVIOUS_PRESSED) {
    menu_idx = 0;
    ss->state = STATE_END_POINT_SELECT;
  } else if (ss->gimbal_y_dir < 0) {
    --menu_idx;
    if (menu_idx < 0) {
      menu_idx = NUM_ITEMS - 1;
    }
  } else if (ss->gimbal_y_dir > 0) {
    ++menu_idx;
    if (menu_idx >= NUM_ITEMS) {
      menu_idx = 0;
    }
  }
}

static void _render(struct SharedState* ss) {
  struct Bitmap* bm = &(ss->bitmap);
  render_common(bm, "Settings", "End Pos", "Select");

  const int8_t num_items = sizeof(items) / sizeof(items[0]);
  int8_t idx = menu_idx - 1;
  if (idx < 0) {
    idx = 0;
  } else if ((idx + 4) > num_items) {
    idx = num_items - 4;
  }
  const uint8_t end_idx = idx + 4;
  int16_t ypos = TEXT_HEIGHT * 2;

  for (; idx < end_idx; ++idx, ypos+=TEXT_HEIGHT) {
    bitmap_str(
        bm,
        TEXT_FONT,
        items[idx],
        0,
        ypos,
        idx == menu_idx ? bitmap_NSET : bitmap_SET);
  }
}

void menu_update(struct SharedState* ss) {
  _update(ss);
  _render(ss);
}
