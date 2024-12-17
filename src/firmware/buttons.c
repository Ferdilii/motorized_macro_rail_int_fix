#include "buttons.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <misc/debounce.h>
#include "shutter.h"

#define SHUTTER_BUTTON_GPIO 9
#define PREVIOUS_BUTTON_GPIO 8
#define NEXT_BUTTON_GPIO 7

#define DEBOUNCE_MS 10

struct Debounce next_db;
struct Debounce previous_db;
struct Debounce shutter_db;
volatile uint8_t button_bit_array;

#define SHUTTER_LOW 0
#define SHUTTER_TRIGGER 1
#define SHUTTER_HIGH 2

volatile uint8_t shutter_state;

static inline uint32_t uptime_ms() {
  return to_ms_since_boot(get_absolute_time());
}

// Callback for gpio_set_irq_callback.  Since it's an ISR,
// try to avoid expensive operations.
static void button_pressed_callback(uint gpio, uint32_t events) {
  switch (gpio) {
    case NEXT_BUTTON_GPIO:
      // The callback helper helps filter away debounce glitches
      // The debounce_gpio_irq_callback_helper has a side effect (updates state in
      // the debounce structure so it should always be called.
      if (debounce_gpio_irq_callback_helper(&next_db, uptime_ms(), events) &&
          next_db.val) {
        button_bit_array |= NEXT_PRESSED;
      }
      break;
    case PREVIOUS_BUTTON_GPIO:
      if (debounce_gpio_irq_callback_helper(&previous_db, uptime_ms(), events) &&
          previous_db.val) {
        button_bit_array |= PREVIOUS_PRESSED;
      }
      break;
    case SHUTTER_BUTTON_GPIO:
      if (debounce_gpio_irq_callback_helper(&shutter_db, uptime_ms(), events) &&
          shutter_db.val) {
        shutter_state = SHUTTER_TRIGGER;
      }
      break;
  }
}

// Sets up a pin to sense a button press.  Hardware-wise the button is
// connected to the pin and to ground.  The code below sets the internal
// pullup for the button so the pin will sit at 3.3V when the button
// is not pressed.  When the button is pressed, the 3.3V will be pulled
// to ground which is an event that the pico is configured to recognize
// and raise an interrupt for.
static void setup_gpio(uint gpio) {
  gpio_init(gpio);
  gpio_set_dir(gpio, GPIO_IN);
  gpio_pull_up(gpio);
  sleep_ms(1);  // give the pullup some time to do it's thing. Maybe not needed.
  gpio_set_irq_enabled(gpio, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void buttons_init(void) {
  setup_gpio(NEXT_BUTTON_GPIO);
  setup_gpio(PREVIOUS_BUTTON_GPIO);
  setup_gpio(SHUTTER_BUTTON_GPIO);
  gpio_set_irq_callback(button_pressed_callback);
  irq_set_enabled(IO_IRQ_BANK0, true);
  debounce_init(&next_db, DEBOUNCE_MS);
  debounce_init(&previous_db, DEBOUNCE_MS);
  debounce_init(&shutter_db, DEBOUNCE_MS);
  button_bit_array = 0x00;
}

void buttons_update(struct SharedState* ss) {
  if (shutter_state == SHUTTER_TRIGGER) {
    shutter_high();
    shutter_state = SHUTTER_HIGH;
  } else if (shutter_state == SHUTTER_HIGH) {
    shutter_low();
    shutter_state = SHUTTER_LOW;
  }
  ss->button = button_bit_array;
  button_bit_array = 0x00;  // reset
}


