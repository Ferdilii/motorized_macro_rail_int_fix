#include "a4988_calibrate.h"
#include "pico/stdlib.h"

#include "motor_control.h"
#include "motor_driver.h"
#include "oledm/font/terminus8x16.h"
#include "render_common.h"

static const char* title;

void a4988_calibrate_init(struct SharedState* ss, const char* _title) {
  motor_control_stop_loop(&(ss->motor));
  title = _title;
}

static void enable_current_flow(void) {
  motor_driver_wake();
  sleep_ms(10);
  motor_driver_step(1);
}

static void disable_current_flow(void) {
  motor_driver_step(0);
  sleep_ms(10);
  motor_driver_sleep();
}

static void render_msg(struct Bitmap* bm, const char* msg[]) {
  int16_t y = TEXT_HEIGHT * 2;
  for (uint8_t i=0; msg[i]; ++i, y += TEXT_HEIGHT) {
    bitmap_str(
        bm,
        TEXT_FONT,
        msg[i],
        0,
        y,
        bitmap_SET);
  }
}
static void update(struct SharedState* ss) {
  if (ss->button & PREVIOUS_PRESSED) {
    disable_current_flow();
    motor_control_start_loop(&(ss->motor));
    ss->state = STATE_MENU;
  } else if (ss->button & NEXT_PRESSED) {
    if (motor_driver_is_step_high()) {
      disable_current_flow();
    } else {
      enable_current_flow();
    }
  }
}

static void render(struct SharedState* ss) {
  const uint8_t step_high = motor_driver_is_step_high();
  struct Bitmap* bm = &(ss->bitmap);
  render_common(
      bm,
      "A4988 Calibrate",
      "Back",
      step_high ? "Stop" : "Start");

  if (step_high) {
    const char* msg[] = {
      "Press NEXT to",
      "stop current",
      "flow",
      0,
    };
    render_msg(bm, msg);
  } else {
    const char* msg[] = {
      "Press NEXT to",
      "start current",
      "flow to motor",
      "for measurement",
      0,
    };
    render_msg(bm, msg);
  }
}

void a4988_calibrate_update(struct SharedState* ss) {
  update(ss);
  render(ss);
}

