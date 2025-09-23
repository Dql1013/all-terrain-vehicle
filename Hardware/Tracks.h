#ifndef __TRACKS_H
#define __TRACKS_H

#include "stm32f10x.h"
#include "../System/GPIO.h"  // 使用GPIO宏定义

// 循迹传感器数量
#define TRACKS_NUM 8

// 循迹状态定义
#define TRACKS_LEFT_TURN    1   // 左转标志
#define TRACKS_RIGHT_TURN   2   // 右转标志
#define TRACKS_STRAIGHT     3   // 直行标志
#define TRACKS_CROSSROAD    4   // 十字路口标志
#define TRACKS_LEFT_ANGLE   5   // 左直角弯标志
#define TRACKS_RIGHT_ANGLE  6   // 右直角弯标志
#define TRACKS_LOST         7   // 丢失轨迹标志

// 函数声明
void Tracks_Init(void);                 // 初始化循迹传感器
uint16_t Tracks_Read(void);             // 读取所有传感器状态
uint8_t Tracks_GetStatus(uint16_t tracks_value);  // 获取循迹状态
uint8_t Tracks_DetectCrossroad(void);   // 检测十字路口
int16_t Tracks_GetDeviation(uint16_t tracks_value);  // 获取偏差值用于PID控制

#endif /* __TRACKS_H */
