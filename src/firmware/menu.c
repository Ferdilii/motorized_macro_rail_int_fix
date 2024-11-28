#include "menu.h"

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"

#include "acceleration.h"
#include "backlash.h"
#include "max_velocity.h"
#include "render_common.h"

const static char* items[] = {
  "End Position",  // idx 0
  "Max Vel (step/s)",  // idx 0
  "Accel (step/s*s)",  // idx 0
  "Backlash (steps)", // idx 1
  "Settle Seconds",   // idx 1
  "Steps / mm",    // idx 1
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

  uint8_t idx = menu_idx <= 2 ? 0 : 1;
  const uint8_t end_idx = idx + 5;
  int16_t ypos = 32;

  for (; idx < end_idx; ++idx) {
    bitmap_str(
        bm,
        terminus8x16,
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
