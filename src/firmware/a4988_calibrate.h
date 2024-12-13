#ifndef A4988_CALIBRATE_H
#define A4988_CALIBRATE_H

#include "shared_state.h"

void a4988_calibrate_init(
    struct SharedState* ss,
    const char* title);
void a4988_calibrate_update(struct SharedState* ss);

#endif
