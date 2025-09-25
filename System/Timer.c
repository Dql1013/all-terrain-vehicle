/*
********************************************************************************************************
文 件 名：Timer.c
功    能：定时器配置和PWM控制
日    期: 2024-10-1
备    注：
        Timer1 中断
        Timer2 电机控制pwm,用到了timer2的ch3和ch4，即 PA2,PA3连接电机驱动的使能端PWMA（左电机）和 PWMB（右电机）
        Timer3 用于左电机编码器计数
        Timer4 用于右电机编码器计数

*********************************************************************************************************
*/
/*
********************************************************************************************************
更新日志：
10.1：完善定时器初始化和电机控制功能
添加Set_Left_Speed和Set_Right_Speed函数
*********************************************************************************************************
*/

#include "Timer.h"


/**
  * 函    数：初始化所有定时器
  * 参    数：无
  * 返 回 值：无
  */
void Timer_All_Init(void)
{
    // 初始化中断定时器(默认1ms中断)
    Timer1_Init(999, 71);
    
    // 初始化PWM定时器(1kHz频率，更适合电机控制)
    Timer2_PWM_Init(999, 71);
    
    // 初始化编码器定时器，用于闭环控制提高扭矩稳定性
    Timer3_Encoder_Init();
    Timer4_Encoder_Init();
}

/**
  * 函    数：TIM1初始化(通用中断定时器)
  * 参    数：arr 自动重装载值
  *           psc 预分频器值
  * 返 回 值：无
  */
void Timer1_Init(uint16_t arr, uint16_t psc)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
    
    TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    TIM_Cmd(TIM1, ENABLE);
}

/**
  * 函    数：TIM2初始化(PWM输出)
  * 参    数：arr 自动重装载值
  *           psc 预分频器值
  * 返 回 值：无
  */
void Timer2_PWM_Init(uint16_t arr, uint16_t psc)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    // 设置定时器基本参数
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // 配置PWM模式
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);

    TIM_Cmd(TIM2, ENABLE);
}

/**
  * 函    数：TIM3初始化(编码器模式)
  * 参    数：无
  * 返 回 值：无
  */
void Timer3_Encoder_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    // 配置编码器模式
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    
    // 配置编码器接口
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM3, &TIM_ICInitStructure);
    
    // 设置编码器模式
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 清除计数器并启动定时器
    TIM_SetCounter(TIM3, 0);
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * 函    数：TIM4初始化(编码器模式)
  * 参    数：无
  * 返 回 值：无
  */
void Timer4_Encoder_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    // 配置编码器模式
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    // 配置编码器接口
    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_ICStructInit(&TIM_ICInitStructure);
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICFilter = 0;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit(TIM4, &TIM_ICInitStructure);
    
    // 设置编码器模式
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 清除计数器并启动定时器
    TIM_SetCounter(TIM4, 0);
    TIM_Cmd(TIM4, ENABLE);
}

/**
  * 函    数：设置PWM占空比
  * 参    数：left_pwm 左电机PWM占空比(0-999)
  *           right_pwm 右电机PWM占空比(0-999)
  * 返 回 值：无
  */
void Set_PWM(int left_speed , int right_speed)
{
	if (left_speed  > 1000) left_speed  = 1000;
	if (right_speed > 1000) right_speed = 1000;
	TIM_SetCompare3(TIM2, left_speed);			
	TIM_SetCompare4(TIM2, right_speed);
}


/**
  * 函    数：获取编码器计数值
  * 参    数：encoder_num 编码器编号(0:左,1:右)
  * 返 回 值：编码器计数值
  */
int16_t Get_Encoder_Count(uint8_t encoder_num)
{
    int16_t count = 0;
    
    if (encoder_num == 0)  // 左编码器
    {
        count = (int16_t)TIM_GetCounter(TIM3);
        TIM_SetCounter(TIM3, 0);  // 读取后清零
    }
    else if (encoder_num == 1)  // 右编码器
    {
        count = (int16_t)TIM_GetCounter(TIM4);
        TIM_SetCounter(TIM4, 0);  // 读取后清零
    }
    
    return count;
}

/**
  * 函    数：清除编码器计数
  * 参    数：encoder_num 编码器编号(0:左,1:右)
  * 返 回 值：无
  */
void Clear_Encoder_Count(uint8_t encoder_num)
{
    if (encoder_num == 0)
    {
        TIM_SetCounter(TIM3, 0);
    }
    else if (encoder_num == 1)
    {
        TIM_SetCounter(TIM4, 0);
    }
}

/**
  * 函    数：TIM1中断服务函数
  * 参    数：无
  * 返 回 值：无
  */
// 注意：此函数需要在stm32f10x_it.c中实现

void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
    {
        // 在这里添加定时中断处理代码
        
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

