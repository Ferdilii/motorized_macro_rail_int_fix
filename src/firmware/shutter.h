#ifndef SHUTTER_H
#define SHUTTER_H

#include <inttypes.h>

void shutter_init(void);
void shutter_release(uint32_t high_ms);
void shutter_high(void);
void shutter_low(void);

#endif

