#ifndef __ENCODER_H
#define __ENCODER_H

#include "stm32f10x.h"
#include "Timer.h"

// 编码器相关宏定义
#define ENCODER_LEFT       0       // 左编码器编号
#define ENCODER_RIGHT      1       // 右编码器编号
#define ENCODER_COUNT_MAX  65535   // 编码器最大计数值

// 函数声明
void Encoder_Init(void);
int16_t Encoder_GetCount(uint8_t encoder_num);
float Encoder_GetSpeed(uint8_t encoder_num);
void Encoder_ClearCount(uint8_t encoder_num);
void Encoder_TimerIRQHandler(void);

#endif
