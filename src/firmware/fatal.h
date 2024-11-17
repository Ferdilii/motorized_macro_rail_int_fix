#ifndef FATAL_H
#define FATAL_H

#include "shared_state.h"

// Reports a fatal error
void fatal(struct SharedState* state, const char* format, ...);

// shows a fatal message on the screen and blinks the main led
void fatal_update(struct SharedState* state);

#endif
