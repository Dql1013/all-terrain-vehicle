/*
********************************************************************************************************
文 件 名：Encoder.c
函数功能：编码器数据读取和轮速闭环
入口参数：
返 回 值：
日    期：
接    线：
				M1A---PA6
				M1B---PA7
				M2A---PB8
				M2B---PB9
*********************************************************************************************************
*/
/*
********************************************************************************************************
更新日志：
10.1：完善编码器功能，实现计数读取和速度计算
*********************************************************************************************************
*/

#include "Encoder.h"

/**
  * 函    数：编码器初始化
  * 参    数：无
  * 返 回 值：无
  */
void Encoder_Init(void)
{
    // 初始化定时器为编码器模式
    Timer3_Encoder_Init();  // 左编码器 TIM3
    Timer4_Encoder_Init();  // 右编码器 TIM4
    
    // 清除编码器计数
    TIM_SetCounter(ENCODER_TIM_LEFT, 0);
    TIM_SetCounter(ENCODER_TIM_RIGHT, 0);
}

/**
  * 函    数：获取编码器计数值
  * 参    数：encoder_num 编码器编号(ENCODER_LEFT/ENCODER_RIGHT)
  * 返 回 值：编码器计数值
  */
int16_t Encoder_GetCount_Left(void)
{
	/*使用Temp变量作为中继，目的是返回CNT后将其清零*/
	int16_t Temp;
	Temp = TIM_GetCounter(TIM3);
	TIM_SetCounter(TIM3, 0);
	return Temp;
}
int16_t Encoder_GetCount_Right(void)
{
	/*使用Temp变量作为中继，目的是返回CNT后将其清零*/
	int16_t Temp;
	Temp = TIM_GetCounter(TIM4);
	TIM_SetCounter(TIM4, 0);
	return Temp;
}

