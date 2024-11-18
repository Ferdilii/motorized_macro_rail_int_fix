#include "motor_driver.h"
#include "pico/stdlib.h"

#define RESET_PIN 10 // Active low
#define SLEEP_PIN 11 // Active low
#define STEP_PIN 12 
#define DIRECTION_PIN 13

// Clarify active low pins
#define SLEEP_ENABLED 0
#define SLEEP_DISABLED 1
#define RESET_ENABLED 0
#define RESET_DISABLED 1

// reverse these if needed
#define MOTOR_FORWARD 1
#define MOTOR_REVERSE 0

// pulse timing
#define SETTLE_US 1
#define PULSE_US 2

void motor_driver_init(void) {
  gpio_init(RESET_PIN);
  gpio_put(RESET_PIN, RESET_DISABLED);
  gpio_set_dir(RESET_PIN, GPIO_OUT);

  gpio_init(SLEEP_PIN);
  gpio_put(SLEEP_PIN, SLEEP_ENABLED);
  gpio_set_dir(SLEEP_PIN, GPIO_OUT);

  gpio_init(STEP_PIN);
  gpio_put(STEP_PIN, 0);
  gpio_set_dir(STEP_PIN, GPIO_OUT);

  gpio_init(DIRECTION_PIN);
  gpio_put(DIRECTION_PIN, MOTOR_FORWARD);
  gpio_set_dir(DIRECTION_PIN, GPIO_OUT);

  // give everything a reset
  gpio_put(RESET_PIN, RESET_ENABLED);
  sleep_ms(1);
  gpio_put(RESET_PIN, RESET_DISABLED);
  sleep_ms(1);
}

void motor_driver_step(int8_t direction) {
  if (direction == 0) {
    return;
  }
  motor_driver_wake();
  gpio_put(DIRECTION_PIN, direction > 0 ? MOTOR_FORWARD : MOTOR_REVERSE);
  busy_wait_us_32(SETTLE_US);
  gpio_put(STEP_PIN, 1);
  busy_wait_us_32(PULSE_US);
  gpio_put(STEP_PIN, 0);
  busy_wait_us_32(SETTLE_US);
}

void motor_driver_sleep(void) {
  gpio_put(SLEEP_PIN, SLEEP_ENABLED);
}

void motor_driver_wake(void) {
  // check first to avoid an unneeded sleep
  if (!gpio_get(SLEEP_PIN)) {
    gpio_put(SLEEP_PIN, SLEEP_DISABLED);
    sleep_ms(1);
  }
}

