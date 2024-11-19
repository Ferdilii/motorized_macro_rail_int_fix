#ifndef SHUTTER_DELAY_H
#define SHUTTER_DELAY_H

#include "shared_state.h"
#include <inttypes.h>

void shutter_delay_init(struct SharedState* ss);
void shutter_delay_update(struct SharedState* ss);

#endif

