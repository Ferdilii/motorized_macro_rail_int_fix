#ifndef LOGGING_H
#define LOGGING_H

//#define DEBUG

#ifdef DEBUG
  #define DEBUG_LOG(FMT, ...) printf(FMT "\n", __VA_ARGS__)
#else
  #define DEBUG_LOG(FMT, ...)
#endif

#include "shared_state.h"
void logging_init(void);
void logging_update(struct SharedState* ss); 

#endif
