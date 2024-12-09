#include "gimbal_jog.h"
#include "motor_control.h"
#include "misc/gimbal.h"

#define DEADZONE 0x10

void gimbal_jog(
    struct SharedState* ss,
    const struct MotorControl* motor_snap,
    int32_t max_velocity) {
  uint16_t y = ss->gimbal_y_pos;
  int32_t new_jv = 0;
  if (y > (GIMBAL_CALIBRATE_CENTER + DEADZONE)) {
    y = y - DEADZONE - GIMBAL_CALIBRATE_CENTER;
    new_jv = -y * max_velocity / (GIMBAL_CALIBRATE_CENTER - DEADZONE);
  } else if (y < (GIMBAL_CALIBRATE_CENTER - DEADZONE)) {
    y = GIMBAL_CALIBRATE_CENTER - DEADZONE - y;
    new_jv = y * max_velocity / (GIMBAL_CALIBRATE_CENTER - DEADZONE);
  } 

  // A spin lock is needed to change motor settings so only
  // change them if needed.
  const float delta_jv = motor_snap->jog_velocity - (float)new_jv;
  if ((delta_jv >= 1.0) || (delta_jv <= -1.0)) {
    motor_control_set_jog_velocity(&(ss->motor), (float)new_jv);
  }
}

