#include "max_velocity.h"

#include "parameter_change.h"
#include "saved_settings.h"

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

void max_velocity_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
}

