#include "pico/multicore.h"
#include <pico/stdlib.h>
#include <string.h>

#include <oledm/oledm.h>
#include "shared_state.h"

#include "buttons.h"
#include "end_point_select.h"
#include "gimbal_update.h"
#include "fatal.h"
#include "main_led.h"
#include "shutter_delay.h"

// A4988 Stepper Motor Controller (Reference only)
//
//         +------------+
//  N/C 1 -| EN    VMOT |- 16 +12V (with 100uf cap)
//  GND 2 -| MS1    GND |- 15 GND
//  GND 3 -| MS2     2B |- 14 MOTOR
//  GND 4 -| MS3     2A |- 13 MOTOR
// GP10 5 -| RST     1A |- 12 MOTOR
// GP11 6 -| SLP     1B |- 11 MOTOR
// GP12 7 -| STEP   VDD |- 10 +5V
// GP13 8 -| DIR    GND |- 9  GND
//         +------------+
//
// Pi Pico (A4988 pins not yet settled or coded)
//
//            +-------------------------------------+
//         1 -| TX0*/SDA0/MISO0/G0             VBUS |- 40 
//         2 -| RX0*/SCL0/CS0/G1               VSYS |- 39
//         3 -| GND                             GND |- 38
//         4 -| G2/SDA1/SCK0                  3V3EN |- 37
//         5 -| B3/SCL1/MOSI0                   3V3 |- 36
//         6 -| SDA0*/TX1/MISO0/G4             AREF |- 35 GIMBAL_VDD
//         7 -| SCL0*/RX1/CS0/G5           G28/ADC2 |- 34
//         8 -| GND                        AGND/GND |- 33 GIMBAL_GND
//         9 -| G6/SDA1/SCK0          G27/ADC1/SCL1 |- 32 GIMBAL_Y
// B_SHUT 10 -| G7/SCL1/MOSI0         G26/ADC0/SDA1 |- 31 GIMBAL_X
// B_PREV 11 -| G8/TX1/SDA0/MISO1               RUN |- 30 RESET
// B_NEXT 12 -| G9/RX1/SCL0/CS1                 G22 |- 29
//        13 -| GND                             GND |- 28
// M_RST  14 -| G10/SDA1/SCK1              G21/SCL0 |- 27 OLED_RST
// M_SLP  15 -| G11/SCL1/MOSI1             G20/SDA0 |- 26 OLED_DC
// M_STEP 16 -| G12/TX0/SDA0/MISO1  MOSI0*/G19/SCL1 |- 25 OLED_SDA
// M_DIR  17 -| G13/RX0/SCL0/CS1     SCK0*/G18/SDA1 |- 24 OLED_SCL
//        18 -| GND                             GND |- 23
//        19 -| G14/SDA1/SCK1     CS0*/G17/SCL0/RX0 |- 22 OLED_CS
//        20 -| G15/SCL1/MOSI1  MISO0*/G16/SDA0/TX0 |- 21
//            +-------------------------------------+


struct OLEDM display;
struct SharedState state;
uint8_t bitmap_data[DISPLAY_WIDTH * DISPLAY_ROWS];

static void update(void) {
  state.main_led_claimed = 0;
  buttons_update(&state);
  gimbal_update(&state);
  bitmap_fill(&state.bitmap, 0);

  switch (state.state) {
    case STATE_FATAL:
      fatal_update(&state);
      break;
    case STATE_END_POINT_SELECT:
      end_point_select_update(&state, false);
      break;
    case STATE_START_POINT_SELECT:
      end_point_select_update(&state, true);
      break;
    case STATE_SHUTTER_DELAY:
      shutter_delay_update(&state);
      break;
    default:
      fatal(&state, "UNKNOWN_STATE: %d", state.state);
  }

  bitmap_render_fast(&display, &state.bitmap, 0, 0); 
  if (!state.main_led_claimed) {
    main_led_put(state.frame_idx & 0x20);
  }
  ++state.frame_idx;
}

static void start_motor_control(void) {
  motor_control_init(
      &state.motor, MOTOR_BACKLASH, MAX_MOTOR_VELOCITY, MAX_MOTOR_ACCELERATION);
  motor_control_start(&state.motor);
}


static void init() {
  sleep_ms(50);
  main_led_init();

  memset(&state, 0, sizeof(struct SharedState));
  state.state = STATE_END_POINT_SELECT;
  state.bitmap.rows = DISPLAY_ROWS;
  state.bitmap.columns = DISPLAY_WIDTH;
  state.bitmap.data = bitmap_data;

  oledm_basic_init(&display);
  oledm_start(&display);
  oledm_clear(&display, 0x00);

  buttons_init();
  gimbal_update_init(&state);
  multicore_launch_core1(start_motor_control);
}

int main(void) {
  init();

  while (1) {
    update();
    sleep_ms(15);
  }
}

