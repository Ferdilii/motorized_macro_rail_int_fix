#ifndef GIMBAL_JOG_H
#define GIMBAL_JOG_H

#include "shared_state.h"

void gimbal_jog(
    struct SharedState* ss,
    const struct MotorControl* motor_snap,
    int32_t max_velocity);

#endif

