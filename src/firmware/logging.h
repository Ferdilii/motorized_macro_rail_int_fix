#ifndef LOGGING_H
#define LOGGING_H

//#define DEBUG

#ifdef DEBUG
  #include <stdio.h>
  #define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
  #define DEBUG_LOG(...)
#endif

#include "shared_state.h"
void logging_init(void);
void logging_update(struct SharedState* ss); 

#endif
