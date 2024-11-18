#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <stdint.h>

void motor_driver_init(void);
void motor_driver_step(int8_t direction); 
void motor_driver_sleep(void); 
void motor_driver_wake(void); 

#endif

