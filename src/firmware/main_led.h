#ifndef MAIN_LED_H
#define MAIN_LED_H

#include <inttypes.h>

void main_led_init(void);
// uint32_t to make it easy to pass values
void main_led_put(uint32_t on);

#endif
