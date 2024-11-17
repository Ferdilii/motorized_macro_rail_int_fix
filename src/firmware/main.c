#include <pico/stdlib.h>
#include <string.h>

#include <oledm/oledm.h>
#include "shared_state.h"
#include "fatal.h"
#include "main_led.h"

// A4988 Stepper Motor Controller (Reference only)
//
//    +------------+
// 1 -| EN    VMOT |- 16
// 2 -| MS1    GND |- 15
// 3 -| MS2     2B |- 14
// 4 -| MS3     2A |- 13
// 5 -| RST     1A |- 12
// 6 -| SLP     1B |- 11
// 7 -| STEP   VDD |- 10
// 8 -| DIR    GND |- 9
//    +------------+
//
// Pi Pico (A4988 pins not yet settled or coded)
//
//          +-------------------------------------+
//       1 -| TX0*/SDA0/MISO0/G0             VBUS |- 40 
//       2 -| RX0*/SCL0/CS0/G1               VSYS |- 39
//       3 -| GND                             GND |- 38
//       4 -| G2/SDA1/SCK0                  3V3EN |- 37
//       5 -| B3/SCL1/MOSI0                   3V3 |- 36
//       6 -| SDA0*/TX1/MISO0/G4             AREF |- 35
//       7 -| SCL0*/RX1/CS0/G5           G28/ADC2 |- 34
//       8 -| GND                        AGND/GND |- 33
//       9 -| G6/SDA1/SCK0          G27/ADC1/SCL1 |- 32
//      10 -| G7/SCL1/MOSI0         G26/ADC0/SDA1 |- 31
//      11 -| G8/TX1/SDA0/MISO1               RUN |- 30
//      12 -| G9/RX1/SCL0/CS1                 G22 |- 29
//      13 -| GND                             GND |- 28
//      14 -| G10/SDA1/SCK1              G21/SCL0 |- 27 OLED_RST
//      15 -| G11/SCL1/MOSI1             G20/SDA0 |- 26 OLED_DC
//  RST 16 -| G12/TX0/SDA0/MISO1  MOSI0*/G19/SCL1 |- 25 OLED_SDA
//  SLP 17 -| G13/RX0/SCL0/CS1     SCK0*/G18/SDA1 |- 24 OLED_SCL
//      18 -| GND                             GND |- 23
// STEP 19 -| G14/SDA1/SCK1     CS0*/G17/SCL0/RX0 |- 22 OLED_CS
//  DIR 20 -| G15/SCL1/MOSI1  MISO0*/G16/SDA0/TX0 |- 21
//          +-------------------------------------+


struct OLEDM display;
struct SharedState state;
uint8_t bitmap_data[DISPLAY_WIDTH * DISPLAY_ROWS];

static void update(void) {
  state.main_led_claimed = 0;
  bitmap_fill(&state.bitmap, 0);

  switch (state.state) {
    case STATE_FATAL:
      fatal_update(&state);
      break;
    default:
      fatal(&state, "UNKNOWN_STATE: %d", state.state);
  }

  bitmap_render_fast(&display, &state.bitmap, 0, 0); 
  ++state.frame_idx;
}

static void init() {
  sleep_ms(50);

  oledm_basic_init(&display);
  oledm_start(&display);

  memset(&state, 0, sizeof(struct SharedState));
  state.state = STATE_FIRST_END_POINT_SELECT;
  state.bitmap.rows = DISPLAY_ROWS;
  state.bitmap.columns = DISPLAY_WIDTH;
  state.bitmap.data = bitmap_data;
}

int main(void) {
  init();

  while (1) {
    update();
    sleep_ms(15);
  }
}

