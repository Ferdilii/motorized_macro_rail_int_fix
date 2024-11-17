#ifndef FATAL_H
#define FATAL_H

#include "shared_state.h"

// shows a fatal message on the screen and blinks the main led
void fatal_update(struct SharedState* state);

#endif
