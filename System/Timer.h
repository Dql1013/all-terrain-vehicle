#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

//#include "stm32f10x_it.c"

#include "Encoder.h"
#include "../Hardware/OLED.h"
// 定时器相关定义
// No larger than 65535, because the timer of STM32F103 is 16 bit
//不可大于65535，因为STM32F103的定时器是16位的
#define ENCODER_TIM_PERIOD (u16)(65535)   

#define PWM_FREQ_LEFT      500  // 左电机PWM频率
#define PWM_FREQ_RIGHT     500  // 右电机PWM频率
#define ENCODER_TIM_LEFT   TIM3  // 左编码器定时器
#define ENCODER_TIM_RIGHT  TIM4  // 右编码器定时器
#define PWM_TIM            TIM2  // PWM输出定时器
#define INTERRUPT_TIM      TIM1  // 中断定时器

// 函数声明
void Timer_All_Init(void);
void NVIC_Timer_Init(void);
void Timer2_PWM_Init(uint16_t arr, uint16_t psc);
void Timer3_Encoder_Init(void);
void Timer4_Encoder_Init(void);
void Set_PWM(uint16_t left_pwm, uint16_t right_pwm);

#endif /* __TIMER_H */
