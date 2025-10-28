#ifndef __TRACKS_H
#define __TRACKS_H

#include "stm32f10x.h"
#include "../System/GPIO.h"  // 使用GPIO宏定义
#include "Delay.h"
#include "TB6612.h"
#include <stdio.h>

// 循迹传感器数量
#define TRACKS_NUM 8

// 循迹状态定义
#define TRACKS_LEFT_TURN    1   // 左转标志
#define TRACKS_RIGHT_TURN   2   // 右转标志
#define TRACKS_STRAIGHT     3   // 直行标志
#define TRACKS_CROSSROAD    4   // 十字路口标志
#define TRACKS_LEFT_ANGLE   5   // 左直角弯标志
#define TRACKS_RIGHT_ANGLE  6   // 右直角弯标志
#define TRACKS_LOST         7   // 丢失轨迹标志 || 全白
#define TRACKS_ELSE         0   // 未定义标志
// 函数声明
uint16_t Tracks_Read(void);             // 读取所有传感器状态
uint8_t Tracks_GetStatus(void);  // 获取循迹状态
int Tracks_GetDeviation(void);  // 获取偏差值用于PID控制
uint16_t Tracks_CheckAndCountBlackArea(void);  // 检测并计数黑区函数
void Tracks_Control(int left_speed,int right_speed);  // 轨迹控制函数

#endif /* __TRACKS_H */
