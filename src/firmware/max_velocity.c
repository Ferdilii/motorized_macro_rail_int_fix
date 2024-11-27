#include "max_velocity.h"

#include "parameter_change.h"

static struct ParameterChange pc;
int32_t permanent_val;  // TODO: get from permanent storage

void max_velocity_init(const char* title) {
  parameter_change_init(
      &pc,
      title,
      &permanent_val,
      200,  // TODO: get from permanent storage
      10,
      9999,
      0);
}

void max_velocity_update(struct SharedState* ss) {
  parameter_change_update(&pc, ss);
}

