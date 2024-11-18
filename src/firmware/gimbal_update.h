#ifndef GIMBAL_UPDATE_H
#define GIMBAL_UPDATE_H

#include "shared_state.h"

void gimbal_update_init(struct SharedState* state);
void gimbal_update(struct SharedState* state);

#endif

