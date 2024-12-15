#include "steps_per_mm.h"

#include "parameter_change.h"
#include "saved_settings.h"
#include "motor_control.h"

static struct ParameterChange pc;

void steps_per_mm_init(const char* title) {
  struct SavedSettings* settings = parameter_change_settings();
  parameter_change_init(
      &pc,
      title,
      &(settings->steps_per_mm),
      DEFAULT_STEPS_PER_MM,
      100,
      99999,
      0);
}

static void _test_mode(struct SharedState* ss) {
  struct MotorControl motor_snap;
  motor_control_snapshot(&motor_snap, &(ss->motor));
  if (ss->gimbal_y_dir < 0) {
    motor_control_try_target_position(
        &(ss->motor), motor_snap.current_pos + pc.evw.value); 
  } else if (ss->gimbal_y_dir > 0) {
    motor_control_try_target_position(
        &(ss->motor), motor_snap.current_pos - pc.evw.value); 
  }
}

void steps_per_mm_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
  if (pc.test_mode) {
    _test_mode(ss);
  }
}

