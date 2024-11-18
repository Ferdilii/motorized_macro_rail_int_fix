#include "gimbal_update.h"
#include <misc/gimbal.h> 
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include <inttypes.h>

#define GIMBAL_X_GPIO 26
#define GIMBAL_X_ADC 0
#define GIMBAL_Y_GPIO 27
#define GIMBAL_Y_ADC 1

struct GimbalCalibrate gcal_x;
struct GimbalCalibrate gcal_y;
struct GimbalDirection gdir_x;
struct GimbalDirection gdir_y;

void gimbal_update_init(struct SharedState* state) {
  adc_init();
  // Make sure GPIO is high-impedance, no pullups etc
  adc_gpio_init(GIMBAL_X_GPIO);
  adc_gpio_init(GIMBAL_Y_GPIO);

  const uint16_t adc_min = 600;
  const uint16_t adc_max = 3495;
  gimbal_cal_init(&gcal_x, adc_min, adc_max);
  gimbal_cal_init(&gcal_y, adc_min, adc_max);

  const uint16_t deadzone = GIMBAL_CALIBRATE_MAX / 10;
  const uint16_t pullback = GIMBAL_CALIBRATE_MAX / 10;
  gimbal_dir_init(&gdir_x, deadzone, pullback);
  gimbal_dir_init(&gdir_y, deadzone, pullback);
}

void gimbal_update(struct SharedState* state) {
  adc_select_input(GIMBAL_X_ADC);
  const uint16_t gimbal_x_adc = adc_read();
  state->gimbal_x_pos = gimbal_calibrate(&gcal_x, gimbal_x_adc);
  state->gimbal_x_dir = gimbal_direction(&gdir_x, state->gimbal_x_pos);

  adc_select_input(GIMBAL_Y_ADC);
  const uint16_t gimbal_y_adc = adc_read();
  state->gimbal_y_pos = gimbal_calibrate(&gcal_y, gimbal_y_adc);
  state->gimbal_y_dir = gimbal_direction(&gdir_y, state->gimbal_y_pos);
}
