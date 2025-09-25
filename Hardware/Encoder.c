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


// 编码器计数值变量
static int32_t encoder_left_count = 0;
static int32_t encoder_right_count = 0;

// 编码器速度计算变量
static int32_t encoder_left_last_count = 0;
static int32_t encoder_right_last_count = 0;
static float encoder_left_speed = 0.0f;
static float encoder_right_speed = 0.0f;

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
    Encoder_ClearCount(ENCODER_LEFT);
    Encoder_ClearCount(ENCODER_RIGHT);
}

/**
  * 函    数：获取编码器计数值
  * 参    数：encoder_num 编码器编号(ENCODER_LEFT/ENCODER_RIGHT)
  * 返 回 值：编码器计数值
  */
int16_t Encoder_GetCount(uint8_t encoder_num)
{
    if (encoder_num == ENCODER_LEFT)
    {
        return (int16_t)TIM_GetCounter(ENCODER_TIM_LEFT);
    }
    else if (encoder_num == ENCODER_RIGHT)
    {
        return (int16_t)TIM_GetCounter(ENCODER_TIM_RIGHT);
    }
    return 0;
}

/**
  * 函    数：获取编码器速度
  * 参    数：encoder_num 编码器编号(ENCODER_LEFT/ENCODER_RIGHT)
  * 返 回 值：编码器速度值
  * 说    明：速度单位为：编码器脉冲数/10ms
  */
float Encoder_GetSpeed(uint8_t encoder_num)
{
    if (encoder_num == ENCODER_LEFT)
    {
        return encoder_left_speed;
    }
    else if (encoder_num == ENCODER_RIGHT)
    {
        return encoder_right_speed;
    }
    return 0.0f;
}

/**
  * 函    数：清除编码器计数
  * 参    数：encoder_num 编码器编号(ENCODER_LEFT/ENCODER_RIGHT)
  * 返 回 值：无
  */
void Encoder_ClearCount(uint8_t encoder_num)
{
    if (encoder_num == ENCODER_LEFT)
    {
        TIM_SetCounter(ENCODER_TIM_LEFT, 0);
        encoder_left_count = 0;
        encoder_left_last_count = 0;
        encoder_left_speed = 0.0f;
    }
    else if (encoder_num == ENCODER_RIGHT)
    {
        TIM_SetCounter(ENCODER_TIM_RIGHT, 0);
        encoder_right_count = 0;
        encoder_right_last_count = 0;
        encoder_right_speed = 0.0f;
    }
}

/**
  * 函    数：编码器定时器中断处理函数
  * 参    数：无
  * 返 回 值：无
  * 说    明：应在10ms定时器中断中调用此函数，用于计算编码器速度
  */
void Encoder_TimerIRQHandler(void)
{
    // 读取当前编码器计数值
    int16_t left_current = Encoder_GetCount(ENCODER_LEFT);
    int16_t right_current = Encoder_GetCount(ENCODER_RIGHT);
    
    // 更新总计数
    encoder_left_count += left_current;
    encoder_right_count += right_current;
    
    // 计算速度（当前计数减去上次计数）
    encoder_left_speed = (float)(left_current - encoder_left_last_count);
    encoder_right_speed = (float)(right_current - encoder_right_last_count);
    
    // 保存当前计数值，用于下次计算
    encoder_left_last_count = left_current;
    encoder_right_last_count = right_current;
    
    // 清除编码器计数，准备下一次采样
    Clear_Encoder_Count(ENCODER_LEFT);
    Clear_Encoder_Count(ENCODER_RIGHT);
}

