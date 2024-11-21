#include "shutter.h"
#include "pico/stdlib.h"

#define SHUTTER_GPIO 15

void shutter_init(void) {
  gpio_init(SHUTTER_GPIO);
  gpio_set_dir(SHUTTER_GPIO, GPIO_OUT);
}

void shutter_high(void) {
  gpio_put(SHUTTER_GPIO, 1);
}

void shutter_low(void) {
  gpio_put(SHUTTER_GPIO, 0);
}

void shutter_release(uint32_t high_ms) {
  shutter_high();
  sleep_ms(high_ms);
  shutter_low();
}

