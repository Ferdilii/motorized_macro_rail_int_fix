#include "end_point_select.h"

#include <stdio.h>

#include "misc/gimbal.h"
#include "oledm/font/terminus8x16.h"
#include "motor_control.h"

#define DEADZONE 0x10

static void update(struct SharedState* ss, const struct MotorControl* motor_snap) {
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
  if ((delta_jv >= 1.0) || (delta_jv <= -1.0)) {
    motor_control_set_jog_velocity(&(ss->motor), (float)new_jv);
  } else if (!motor_snap->jog_mode) {
    if (ss->gimbal_x_dir > 0) {
      motor_control_try_target_position(&(ss->motor), motor_snap->current_pos + 1);
    } else if (ss->gimbal_x_dir < 0) {
      motor_control_try_target_position(&(ss->motor), motor_snap->current_pos - 1);
    }
  }
}

static void render_title(struct Bitmap* bm) {
  bitmap_str(bm, terminus8x16, "End Point Select", 0, 0, bitmap_SET);
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

  snprintf(str, sizeof(str), "Motor:  %8d", (int32_t)(motor_snap->motor_pos));
  bitmap_str(bm, terminus8x16, str, 0, 64, bitmap_SET);
}

static void render_backlash(struct Bitmap* bm, const struct MotorControl* motor_snap) {
  char str[32];
  const int16_t backlash = (int16_t)(motor_snap->motor_pos - motor_snap->current_pos);
  snprintf(str, sizeof(str), "Backlash:  %5d", backlash);

  // convert backlash from a full scale to a 0-64 scale
  const int16_t scaled_bl = backlash * 64 / motor_snap->backlash;
  bitmap_vline(bm, 0, 117, 10, bitmap_OR);
  bitmap_vline(bm, 127, 117, 10, bitmap_OR);
  bitmap_vline(bm, 64 + scaled_bl, 120, 7, bitmap_OR);
}

static void render(struct SharedState* ss, const struct MotorControl* motor_snap) {
  struct Bitmap* bm = &(ss->bitmap);
  render_title(bm);
  render_position(bm, motor_snap);
  render_backlash(bm, motor_snap);
}

void end_point_select_update(struct SharedState* ss) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));

  update(ss, &motor_snap);
  render(ss, &motor_snap);
}

