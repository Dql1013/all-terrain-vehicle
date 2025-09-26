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
// 声明外部变量

	
/**
  * 函    数：初始化所有定时器
  * 参    数：无
  * 返 回 值：无
  */
void Timer_All_Init(void)
{
  
    // 初始化PWM定时器(1kHz频率，更适合电机控制)
    Timer2_PWM_Init(999, 71);
    
    // 初始化编码器定时器，用于闭环控制提高扭矩稳定性
    Timer3_Encoder_Init();
    Timer4_Encoder_Init();
	
		NVIC_Timer_Init();
}

/**
  * 函    数：TIM1初始化(通用中断定时器)
  * 参    数：arr 自动重装载值
  *           psc 预分频器值
  * 返 回 值：无
  */
void NVIC_Timer_Init(void)
{    
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //时钟使能

  TIM_TimeBaseStructure.TIM_Period = 5000 - 1; //设置自动重装载寄存器周期值
  TIM_TimeBaseStructure.TIM_Prescaler = 720 - 1;//设置预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//重复计数设置
  TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //参数初始化
  TIM_ClearFlag(TIM1, TIM_FLAG_Update);//清中断标志位

  TIM_ITConfig(      //使能或者失能指定的TIM中断
    TIM1,            //TIM1
    TIM_IT_Update  | //TIM 更新中断源
    TIM_IT_Trigger,  //TIM 触发中断源 
    ENABLE  	     //使能
    );
	
  //设置优先级
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  	   //从优先级0级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 

  TIM_Cmd(TIM1, ENABLE);  //使能TIMx外设

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
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
  TIM_ICInitTypeDef TIM_ICInitStructure;  
//  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);   //使能定时器
	
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0; 							// No prescaling     //不分频
  TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD;  //设定计数器自动重装值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //选择时钟分频：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数    
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);  //初始化定时器
  
  TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);//使用编码器模式3
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM3, &TIM_ICInitStructure);  
	
  TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIM的更新标志位
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM3,0);
  TIM_Cmd(TIM3, ENABLE); 
}

/**
  * 函    数：TIM4初始化(编码器模式)
  * 参    数：无
  * 返 回 值：无
  */
void Timer4_Encoder_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  
  TIM_ICInitTypeDef TIM_ICInitStructure;  
//  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//使能定时器4的时钟
    
  TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
  
  TIM_TimeBaseStructure.TIM_Prescaler = 0x0; // No prescaling 
  TIM_TimeBaseStructure.TIM_Period = ENCODER_TIM_PERIOD; //设定计数器自动重装值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//选择时钟分频：不分频
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   //TIM向上计数  
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);//使用编码器模式3
  TIM_ICStructInit(&TIM_ICInitStructure);
  TIM_ICInitStructure.TIM_ICFilter = 0;
  TIM_ICInit(TIM4, &TIM_ICInitStructure);
  
  TIM_ClearFlag(TIM4, TIM_FLAG_Update);
  TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
	TIM_SetCounter(TIM4,0);
  TIM_Cmd(TIM4, ENABLE); 
}

/**
  * 函    数：设置PWM占空比
  * 参    数：left_pwm 左电机PWM占空比(0-999)
  *           right_pwm 右电机PWM占空比(0-999)
  * 返 回 值：无
  */
void Set_PWM(uint16_t left_speed , uint16_t right_speed)
{
//	if (left_speed  < 400) left_speed  = 400;
//	if (right_speed < 400) right_speed = 400;
//	if (left_speed  > 1000) left_speed  = 1000;
//	if (right_speed > 1000) right_speed = 1000;
	TIM_SetCompare3(TIM2, left_speed);			
	TIM_SetCompare4(TIM2, right_speed);
}

/**
  * 函    数：TIM2中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
/* 定时器中断函数，可以复制到使用它的地方
void TIM1_UP_IRQHandler(void) 
{ 	    	  	     
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//检查指定的TIM中断发生与否:TIM 中断源 
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);//清除TIMx的中断待处理位:TIM 中断源 
//在此处添加中断执行内容
	}	     
} 
*/
