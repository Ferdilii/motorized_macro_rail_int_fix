#include "backlash.h"

#include "parameter_change.h"
#include "saved_settings.h"
#include "motor_control.h"

static struct ParameterChange pc;

void backlash_init(const char* title) {
  struct SavedSettings* settings = parameter_change_settings();
  parameter_change_init(
      &pc,
      title,
      &(settings->backlash),
      DEFAULT_BACKLASH,
      0,
      999,
      0);
}

static void _test_mode(struct SharedState* ss) {
  struct MotorControl motor_snap;
  struct SavedSettings* settings = parameter_change_settings();
  motor_control_snapshot(&motor_snap, &(ss->motor));
  if (motor_snap.backlash != settings->backlash) {
    motor_control_try_backlash(&(ss->motor), settings->backlash);
  }
  if (ss->gimbal_y_dir > 0) {
    motor_control_try_set_current_pos(
        &(ss->motor), motor_snap.current_pos + settings->backlash); 
  } else if (ss->gimbal_y_dir < 0) {
    motor_control_try_set_current_pos(
        &(ss->motor), motor_snap.current_pos - settings->backlash); 
  }
}

void backlash_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
  if (pc.test_mode) {
    _test_mode(ss);
  } else if (ss->state != STATE_BACKLASH) {
    const struct SavedSettings* settings = saved_settings_get();
    motor_control_try_backlash(&(ss->motor), settings->backlash);
  }
}

