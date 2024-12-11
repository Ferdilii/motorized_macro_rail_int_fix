#include "logging.h"
#include "main_led.h"
#include <stdio.h>
#include "pico/stdlib.h"

#ifdef DEBUG
void logging_init(void) {
  stdio_init_all();
  sleep_ms(1000);
  printf("logging initialized.");
}

void logging_update(struct SharedState* ss) {
  ss->main_led_claimed = 1;
  main_led_put(ss->frame_idx & 0x08);
}
#else
void logging_init(void) {
}

void logging_update(struct SharedState* ss) {
}
#endif
