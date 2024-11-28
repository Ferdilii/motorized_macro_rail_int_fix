#include "acceleration.h"

#include "gimbal_jog.h"
#include "parameter_change.h"
#include "saved_settings.h"
#include "motor_control.h"

static struct ParameterChange pc;

void acceleration_init(const char* title) {
  struct SavedSettings* settings = parameter_change_settings();
  parameter_change_init(
      &pc,
      title,
      &(settings->acceleration),
      DEFAULT_ACCELERATION,
      10,
      9999,
      0);
}

static void _test_mode(struct SharedState* ss) {
  struct MotorControl motor_snap;
  struct SavedSettings* settings = parameter_change_settings();
  motor_control_snapshot(&motor_snap, &(ss->motor));
  if (motor_snap.acceleration != settings->acceleration) {
    motor_control_set_acceleration(&(ss->motor), settings->acceleration);
  }
  gimbal_jog(ss, &motor_snap, settings->max_velocity);
}

void acceleration_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
  if (pc.test_mode) {
    _test_mode(ss);
  } else if (ss->state != STATE_ACCELERATION) {
    const struct SavedSettings* settings = saved_settings_get();
    motor_control_set_acceleration(&(ss->motor), settings->acceleration);
  }
}

