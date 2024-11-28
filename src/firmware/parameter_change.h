#ifndef PARAMETER_CHANGE_H
#define PARAMETER_CHANGE_H

#include "shared_state.h"
#include "enter_value_widget.h"
#include "saved_settings.h"

struct ParameterChange {
  const char* title;
  int32_t default_val;
  int32_t* permanent_val;
  uint8_t decimal_places;
  uint8_t test_mode;
  struct EnterValueWidget evw;
};

void parameter_change_init(
    struct ParameterChange* pc,
    const char* title,
    int32_t* permanent_val,
    int32_t default_val,
    int32_t min_val,
    int32_t max_val,
    uint8_t decimal_places);

struct SavedSettings* parameter_change_settings(void);

void parameter_change_update(
    struct ParameterChange* pc,
    struct SharedState* ss);

#endif

