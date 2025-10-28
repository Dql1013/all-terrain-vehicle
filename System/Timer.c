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
				Tout= ((arr+1)*(psc+1))/72M
						
						0.01s		1000*72 /72M
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
    Timer2_PWM_Init(3333, 71);//适用于MG540
//    Timer2_PWM_Init(999, 71);//适用于MG530
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

  TIM_TimeBaseStructure.TIM_Period = 10000 - 1; //设置自动重装载寄存器周期值
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
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);			//开启TIM3的时钟
	
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
	TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
											//配置编码器模式以及两个输入通道是否反相
											//注意此时参数的Rising和Falling已经不代表上升沿和下降沿了，而是代表是否反相
											//此函数必须在输入捕获初始化之后进行，否则输入捕获的配置会覆盖此函数的部分配置
	
	/*TIM使能*/
	TIM_Cmd(TIM3, ENABLE);		//使能TIM3，定时器开始运行
}

/**
  * 函    数：TIM4初始化(编码器模式)
  * 参    数：无
  * 返 回 值：无
  */
void Timer4_Encoder_Init(void)
{
	/*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);			//开启TIM4的时钟
	
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
	TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, TIM_ICPolarity_Falling, TIM_ICPolarity_Falling);
											//配置编码器模式以及两个输入通道是否反相
											//注意此时参数的Rising和Falling已经不代表上升沿和下降沿了，而是代表是否反相
											//此函数必须在输入捕获初始化之后进行，否则输入捕获的配置会覆盖此函数的部分配置
	
	/*TIM使能*/
	TIM_Cmd(TIM4, ENABLE);		//使能TIM4，定时器开始运行
}

/**
  * 函    数：设置PWM占空比
  * 参    数：left_pwm 左电机PWM占空比(0-999)
  *           right_pwm 右电机PWM占空比(0-999)
  * 返 回 值：无
  */
void Set_PWM(uint16_t left_speed , uint16_t right_speed)
{
	
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
