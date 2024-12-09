#include "max_velocity.h"

#include "gimbal_jog.h"
#include "parameter_change.h"
#include "saved_settings.h"
#include "motor_control.h"

static struct ParameterChange pc;

void max_velocity_init(const char* title) {
  struct SavedSettings* settings = parameter_change_settings();
  parameter_change_init(
      &pc,
      title,
      &(settings->max_velocity),
      DEFAULT_MAX_VELOCITY,
      10,
      9999,
      0);
}

static void _test_mode(struct SharedState* ss) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));
  gimbal_jog(ss, &motor_snap, pc.evw.value);
}

void max_velocity_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
  if (pc.test_mode) {
    _test_mode(ss);
  } else if (ss->state != STATE_MAX_VELOCITY) {
    const struct SavedSettings* settings = saved_settings_get();
    motor_control_set_max_velocity(&(ss->motor), settings->max_velocity);
  }
}

