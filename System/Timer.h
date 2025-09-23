#ifndef __TIMER_H
#define __TIMER_H

#include "stm32f10x.h"

// 全局变量声明
extern float RotateSpeed1;       //第一路编码器转速变量（单位：圈/min）
extern float RotateSpeed2;       //第二路编码器转速变量（单位：圈/min）

// 定时器相关定义
#define PWM_FREQ_LEFT      1000  // 左电机PWM频率
#define PWM_FREQ_RIGHT     1000  // 右电机PWM频率
#define ENCODER_TIM_LEFT   TIM3  // 左编码器定时器
#define ENCODER_TIM_RIGHT  TIM4  // 右编码器定时器
#define PWM_TIM            TIM2  // PWM输出定时器
#define INTERRUPT_TIM      TIM1  // 中断定时器

// 函数声明
void Timer_All_Init(void);
void Timer1_Init(uint16_t arr, uint16_t psc);
void Timer2_PWM_Init(uint16_t arr, uint16_t psc);
void Timer3_Encoder_Init(void);
void Timer4_Encoder_Init(void);
void Set_PWM(uint16_t left_pwm, uint16_t right_pwm);
void Set_Left_Speed(uint16_t Speed_Left);
void Set_Right_Speed(uint16_t Speed_Right);
int16_t Get_Encoder_Count(uint8_t encoder_num);
void Clear_Encoder_Count(uint8_t encoder_num);

#endif /* __TIMER_H */
