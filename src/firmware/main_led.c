#include "main_led.h"
#include <pico/stdlib.h>

void main_led_init(void) {
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void main_led_put(uint32_t on) {
  gpio_put(PICO_DEFAULT_LED_PIN, on ? 1 : 0);
}
