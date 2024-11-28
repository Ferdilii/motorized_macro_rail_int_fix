#include "end_point_select.h"

#include <stdio.h>

#include "gimbal_jog.h"
#include "oledm/font/terminus8x16.h"
#include "motor_control.h"
#include "render_common.h"
#include "saved_settings.h"

static inline uint8_t next_ok(const struct MotorControl* motor_snap) {
  // need at least 1mm forward or backward (200 steps)
  return (motor_snap->current_pos >= 200) || (motor_snap->current_pos <= -200);
}

static void update(struct SharedState* ss, const struct MotorControl* motor_snap, uint8_t is_start) {
  if ((ss->button & PREVIOUS_PRESSED) && (ss->motor.jog_mode == 0)) {
    if (is_start) {
      ss->state = STATE_END_POINT_SELECT;
    } else {
      ss->state = STATE_MENU;
    }
  } else if ((ss->button & NEXT_PRESSED) && (ss->motor.jog_mode == 0)) {
    if (is_start) {
      if (next_ok(motor_snap)) {
        ss->start_pos = (int32_t)(motor_snap->current_pos);
        ss->state = STATE_SHUTTER_DELAY;
      }
    } else {
      // end position is always zero
      if (!motor_control_try_set_current_pos(&(ss->motor), 0)) {
        // didn't work
        return;
      }
      ss->state = STATE_START_POINT_SELECT;
    }
  } else {
    const struct SavedSettings* settings = saved_settings_get();
    gimbal_jog(ss, motor_snap, settings->max_velocity);
  }
}

static void render_position(struct Bitmap* bm, const struct MotorControl* motor_snap) {
  // 200 steps per mm
  char str[32];
  const float pos = (motor_snap->current_pos) / 200.0;
  snprintf(str, sizeof(str), "Plate:   %5.2fmm", pos);
  bitmap_str(bm, terminus8x16, str, 0, 32, bitmap_SET);

  snprintf(str, sizeof(str), "%8d", (int32_t)(motor_snap->current_pos));
  bitmap_str(bm, terminus8x16, str, 64, 48, bitmap_SET);

  snprintf(str, sizeof(str), "Motor:  %8d", (int32_t)(motor_snap->motor_pos / 2));
  bitmap_str(bm, terminus8x16, str, 0, 64, bitmap_SET);
}

static void render_backlash(struct Bitmap* bm, const struct MotorControl* motor_snap) {
  char str[32];
  const int32_t backlash = (int16_t)((motor_snap->motor_pos / 2) - motor_snap->current_pos);
  snprintf(str, sizeof(str), "Backlash:  %5d", backlash);
  bitmap_str(bm, terminus8x16, str, 0, 80, bitmap_SET);

  // convert backlash from a full scale to a -63 to 63 scale
  const int32_t scaled_bl = backlash * 63 / (int32_t)motor_snap->backlash;
  const uint16_t y = 112;
  bitmap_vline(bm, 0, y-7, 7, bitmap_SET);
  bitmap_vline(bm, 32, y-5, 5, bitmap_SET);
  bitmap_vline(bm, 64, y-7, 7, bitmap_SET);
  bitmap_vline(bm, 96, y-5, 5, bitmap_SET);
  bitmap_vline(bm, 127, y-5, 7, bitmap_SET);
  bitmap_vline(bm, 64 + scaled_bl, y-10, 10, bitmap_OR);
}

static void render(struct SharedState* ss, const struct MotorControl* motor_snap, uint8_t is_start) {
  struct Bitmap* bm = &(ss->bitmap);
  if (is_start) {
    const char* next_str = next_ok(motor_snap) ? "Delay" : "";
    render_common(bm, "Start Position", "EndPos", next_str);
  } else {
    render_common(bm, "End Position", "Menu", "StartP");
  }
  render_position(bm, motor_snap);
  render_backlash(bm, motor_snap);
}

void end_point_select_update(struct SharedState* ss, uint8_t is_start) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));

  update(ss, &motor_snap, is_start);
  render(ss, &motor_snap, is_start);
}

