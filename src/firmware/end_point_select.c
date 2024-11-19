#include "end_point_select.h"

#include <stdio.h>

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"
#include "motor_control.h"

#define DEADZONE 0x10

static void update(struct SharedState* ss, const struct MotorControl* motor_snap, uint8_t is_start) {
  uint16_t y = ss->gimbal_y_pos;
  int32_t new_jv = 0;
  if (y > (GIMBAL_CALIBRATE_CENTER + DEADZONE)) {
    y = y - DEADZONE - GIMBAL_CALIBRATE_CENTER;
    new_jv = -y * MAX_MOTOR_VELOCITY / (GIMBAL_CALIBRATE_CENTER - DEADZONE);
  } else if (y < (GIMBAL_CALIBRATE_CENTER - DEADZONE)) {
    y = GIMBAL_CALIBRATE_CENTER - DEADZONE - y;
    new_jv = y * MAX_MOTOR_VELOCITY / (GIMBAL_CALIBRATE_CENTER - DEADZONE);
  } 

  // A spin lock is needed to change motor settings so only
  // change them if needed.
  const float delta_jv = motor_snap->jog_velocity - (float)new_jv;
  if ((ss->button & NEXT_PRESSED) && (ss->motor.jog_mode == 0)) {
    if (is_start) {
      ss->start_pos = (int32_t)(motor_snap->current_pos);
      ss->state = STATE_DELAY_BETWEEN_SHOTS;
    } else {
      if (!motor_control_try_zero(&(ss->motor))) {
        // didn't work
        return;
      }
      ss->end_pos = (int32_t)(motor_snap->current_pos);
      ss->state = STATE_START_POINT_SELECT;
    }
  } else if ((delta_jv >= 1.0) || (delta_jv <= -1.0)) {
    motor_control_set_jog_velocity(&(ss->motor), (float)new_jv);
  }
}

static void render_title(struct Bitmap* bm, uint8_t is_start) {
  const char* title = is_start ? "Start Position" : "End Position";
  bitmap_str(bm, terminus8x16, title, 0, 0, bitmap_SET);
  bitmap_hline(bm, 0, 17, 128, bitmap_SET);
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
  bitmap_str(bm, terminus8x16, str, 0, 96, bitmap_SET);

  // convert backlash from a full scale to a -63 to 63 scale
  const int32_t scaled_bl = backlash * 63 / (int32_t)motor_snap->backlash;
  bitmap_vline(bm, 0, 120, 7, bitmap_SET);
  bitmap_vline(bm, 32, 122, 5, bitmap_SET);
  bitmap_vline(bm, 64, 120, 7, bitmap_SET);
  bitmap_vline(bm, 96, 122, 5, bitmap_SET);
  bitmap_vline(bm, 127, 120, 7, bitmap_SET);
  bitmap_vline(bm, 64 + scaled_bl, 117, 10, bitmap_OR);
}

static void render(struct SharedState* ss, const struct MotorControl* motor_snap, uint8_t is_start) {
  struct Bitmap* bm = &(ss->bitmap);
  render_title(bm, is_start);
  render_position(bm, motor_snap);
  render_backlash(bm, motor_snap);
}

void end_point_select_update(struct SharedState* ss, uint8_t is_start) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));

  update(ss, &motor_snap, is_start);
  render(ss, &motor_snap, is_start);
}

