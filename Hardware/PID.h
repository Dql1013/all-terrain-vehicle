#ifndef __PID_H
#define __PID_H

#include "stm32f10x.h"                  // Device header

extern float setspeed; 

int16_t PID(int16_t speed,int16_t setspeed);
int16_t Incremental_PI (int16_t Encoder, int16_t Target);
int16_t Position_PI (int16_t Encoder, int16_t Target);


#endif
