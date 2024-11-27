#ifndef PARAMETER_CHANGE_H
#define PARAMETER_CHANGE_H

#include "shared_state.h"
#include "enter_value_widget.h"

struct ParameterChange {
  const char* title;
  int32_t default_val;
  int32_t* permanent_val;
  uint8_t decimal_places;
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

void parameter_change_update(
    struct ParameterChange* pc,
    struct SharedState* ss);

#endif

