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
    new_jv = y * MAX_MOTOR_VELOCITY / (GIMBAL_CALIBRATE_CENTER - DEADZONE);
  } else if (y < (GIMBAL_CALIBRATE_CENTER - DEADZONE)) {
    y = GIMBAL_CALIBRATE_CENTER - DEADZONE - y;
    new_jv = -y * MAX_MOTOR_VELOCITY / (GIMBAL_CALIBRATE_CENTER - DEADZONE);
  } 

  // A spin lock is needed to change motor settings so only
  // change them if needed.
  const float delta_jv = motor_snap->jog_velocity - (float)new_jv;
  if ((delta_jv >= 1.0) && (delta_jv <= -1.0)) {
    motor_control_set_jog_velocity(&(ss->motor), (float)new_jv);
  } else if (!motor_snap->jog_mode) {
    if (ss->gimbal_x_dir > 0) {
      motor_control_try_target_position(&(ss->motor), motor_snap->current_pos + 1);
    } else if (ss->gimbal_x_dir < 1) {
      motor_control_try_target_position(&(ss->motor), motor_snap->current_pos - 1);
    }
  }
}

static void render(struct SharedState* ss, const struct MotorControl* motor_snap) {
  char str[32];
  struct Bitmap* bm = &(ss->bitmap);
  bitmap_str(bm, terminus8x16, "End Point Select", 0, 0, bitmap_SET);

  bitmap_hline(bm, 0, 17, 128, bitmap_SET);

  // 200 steps per mm
  int32_t pos = (int32_t)(motor_snap->current_pos) / 2;
  const int32_t pos_mm = pos / 100;
  // C % has inconvenient behavor for negatives.
  if (pos < 0) {
    pos = -pos;
  }
  const uint32_t pos_mm_frac = pos % 100;
  snprintf(str, sizeof(str), "Plate:%4d.%02dmm", pos_mm, pos_mm_frac);
  bitmap_str(bm, terminus8x16, str, 0, 32, bitmap_SET);

  snprintf(str, sizeof(str), "%-8d", (int32_t)(motor_snap->current_pos));
  bitmap_str(bm, terminus8x16, str, 64, 48, bitmap_SET);

  snprintf(str, sizeof(str), "Motor:  %-8d", (int32_t)(motor_snap->motor_pos));
  bitmap_str(bm, terminus8x16, str, 0, 64, bitmap_SET);
}

void end_point_select_update(struct SharedState* ss) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));

  update(ss, &motor_snap);
  render(ss, &motor_snap);
}

