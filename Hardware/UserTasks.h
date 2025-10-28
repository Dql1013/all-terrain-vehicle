#ifndef __USERTASKS_H
#define __USERTASKS_H

#include "stm32f10x.h"
#include "../System/GPIO.h"
#include "../System/Timer.h"
#include "Delay.h"
// 伸缩电机控制宏定义
#define MOTOR3_EXTEND    0  // 伸出
#define MOTOR3_STOP   	 1  // 停止

// 函数声明
void UserTasks_Init(void);
void Motor3_Control(uint8_t direction);
void Motor3_TimeControl(void);

#endif
