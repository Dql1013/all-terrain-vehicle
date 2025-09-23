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
#include "stm32f10x.h"

int16_t Speed1;			//定义第一路编码器速度变量
int16_t Speed2;			//定义第二路编码器速度变量
//全局变量RotateSpeed1，RotateSpeed2，用于存储电机的转速

float RotateSpeed1;       //定义第一路编码器转速变量（单位：圈/min）
float RotateSpeed2;       //定义第二路编码器转速变量（单位：圈/min）

#define ENCODER_PPR 100  //编码器每转脉冲数（根据实际编码器修改此值）
#define SAMPLE_PERIOD 0.05 //采样周期（秒）

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
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // PA2(TIM2_CH3)和PA3(TIM2_CH4)配置为复用推挽输出
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置TIM2基本定时器
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = arr;
    TIM_TimeBaseStructure.TIM_Prescaler = psc;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
    // 配置TIM2 CH3和CH4为PWM模式
    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;  // 初始占空比为0
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);
    
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM2, ENABLE);
    
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
    
		/*时基单元初始化*/
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
		TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;               //计数周期，即ARR的值
		TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;                //预分频器，即PSC的值
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
		TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM3的时基单元
		
		/*输入捕获初始化*/
		TIM_ICInitTypeDef TIM_ICInitStructure;						//定义结构体变量
		TIM_ICStructInit(&TIM_ICInitStructure);						//结构体初始化，若结构体没有完整赋值
															//则最好执行此函数，给结构体所有成员都赋一个默认值
															//避免结构体初值不确定的问题
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;			//选择配置定时器通道1
		TIM_ICInitStructure.TIM_ICFilter = 0xF;					//输入滤波器参数，可以过滤信号抖动
		TIM_ICInit(TIM3, &TIM_ICInitStructure);					//将结构体变量交给TIM_ICInit，配置TIM3的输入捕获通道
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;			//选择配置定时器通道2
		TIM_ICInitStructure.TIM_ICFilter = 0xF;					//输入滤波器参数，可以过滤信号抖动
		TIM_ICInit(TIM3, &TIM_ICInitStructure);					//将结构体变量交给TIM_ICInit，配置TIM3的输入捕获通道
		
		/*编码器接口配置*/
		TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
												//配置编码器模式以及两个输入通道是否反相
												//注意此时参数的Rising和Falling已经不代表上升沿和下降沿了，而是代表是否反相
												//此函数必须在输入捕获初始化之后进行，否则输入捕获的配置会覆盖此函数的部分配置
		
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
			
			/*时基单元初始化*/
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;				//定义结构体变量
		TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;     //时钟分频，选择不分频，此参数用于配置滤波器时钟，不影响时基单元功能
		TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; //计数器模式，选择向上计数
		TIM_TimeBaseInitStructure.TIM_Period = 65536 - 1;               //计数周期，即ARR的值
		TIM_TimeBaseInitStructure.TIM_Prescaler = 1 - 1;                //预分频器，即PSC的值
		TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;            //重复计数器，高级定时器才会用到
		TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);             //将结构体变量交给TIM_TimeBaseInit，配置TIM4的时基单元
		
		/*输入捕获初始化*/
		TIM_ICInitTypeDef TIM_ICInitStructure;						//定义结构体变量
		TIM_ICStructInit(&TIM_ICInitStructure);						//结构体初始化，若结构体没有完整赋值
															//则最好执行此函数，给结构体所有成员都赋一个默认值
															//避免结构体初值不确定的问题
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;			//选择配置定时器通道1
		TIM_ICInitStructure.TIM_ICFilter = 0xF;					//输入滤波器参数，可以过滤信号抖动
		TIM_ICInit(TIM4, &TIM_ICInitStructure);					//将结构体变量交给TIM_ICInit，配置TIM4的输入捕获通道
		TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;			//选择配置定时器通道2
		TIM_ICInitStructure.TIM_ICFilter = 0xF;					//输入滤波器参数，可以过滤信号抖动
		TIM_ICInit(TIM4, &TIM_ICInitStructure);					//将结构体变量交给TIM_ICInit，配置TIM4的输入捕获通道
		
		/*编码器接口配置*/
		TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
												//配置编码器模式以及两个输入通道是否反相
												//注意此时参数的Rising和Falling已经不代表上升沿和下降沿了，而是代表是否反相
												//此函数必须在输入捕获初始化之后进行，否则输入捕获的配置会覆盖此函数的部分配置
    
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
void Set_PWM(uint16_t left_pwm, uint16_t right_pwm)
{
    // 限制PWM值在有效范围内
    if (left_pwm > 9999) left_pwm = 1500;
    if (right_pwm > 999) right_pwm = 1500;
    
    Set_Left_Speed(left_pwm);
    Set_Right_Speed(right_pwm);
}

/**
  * 函    数：设置左电机速度
  * 参    数：Speed_Left 左电机速度值
  * 返 回 值：无
  * 功    能：配置TIM2通道3的PWM输出，控制左电机速度
  */
void Set_Left_Speed(uint16_t Speed_Left)
{
    // 限制PWM值在有效范围内
    if (Speed_Left > 999) Speed_Left = 999;
    
    // 直接更新PWM值，无需每次都重新初始化整个通道
    TIM_SetCompare3(TIM2, Speed_Left);
}

/**
  * 函    数：设置右电机速度
  * 参    数：Speed_Right 右电机速度值
  * 返 回 值：无
  * 功    能：配置TIM2通道4的PWM输出，控制右电机速度
  */
void Set_Right_Speed(uint16_t Speed_Right)
{
    // 限制PWM值在有效范围内
    if (Speed_Right > 999) Speed_Right = 999;
    
    // 直接更新PWM值，无需每次都重新初始化整个通道
    TIM_SetCompare4(TIM2, Speed_Right);
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
				Speed1 = Get_Encoder_Count(0);								//每隔固定时间段读取一次第一路编码器计数增量值
				Speed2 = Get_Encoder_Count(1);							//每隔固定时间段读取一次第二路编码器计数增量值
				// 计算转速（圈/分钟） = (脉冲数/采样周期) / (编码器每转脉冲数*4) * 60
				// 乘以4是因为使用了TIM_EncoderMode_TI12模式
				RotateSpeed1 = (Speed1 / SAMPLE_PERIOD) / (ENCODER_PPR * 4.0f) * 60.0f;
				RotateSpeed2 = (Speed2 / SAMPLE_PERIOD) / (ENCODER_PPR * 4.0f) * 60.0f;
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

