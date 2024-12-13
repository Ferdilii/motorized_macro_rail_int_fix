#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include <stdint.h>

void motor_driver_init(void);
void motor_driver_dir(int8_t direction); 
void motor_driver_step(uint8_t high); 
uint8_t motor_driver_is_step_high(void); 
void motor_driver_sleep(void); 
void motor_driver_wake(void); 

#endif

