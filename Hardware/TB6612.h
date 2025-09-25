#ifndef __TB6612_H
#define __TB6612_H

#include "stm32f10x.h"
#include "../System/GPIO.h"  // 包含GPIO引脚定义
#include "../System/Timer.h" // 包含PWM控制函数

// 电机方向控制宏定义
#define MOTOR_LEFT_FORWARD   do {GPIO_WriteBit(TB6612_AIN1_PORT, TB6612_AIN1_PIN, 1); GPIO_WriteBit(TB6612_AIN2_PORT, TB6612_AIN2_PIN, 0);} while(0)
#define MOTOR_LEFT_BACKWARD  do {GPIO_WriteBit(TB6612_AIN1_PORT, TB6612_AIN1_PIN, 0); GPIO_WriteBit(TB6612_AIN2_PORT, TB6612_AIN2_PIN, 1);} while(0)
#define MOTOR_LEFT_STOP      do {GPIO_WriteBit(TB6612_AIN1_PORT, TB6612_AIN1_PIN, 0); GPIO_WriteBit(TB6612_AIN2_PORT, TB6612_AIN2_PIN, 0);} while(0)
#define MOTOR_RIGHT_FORWARD  do {GPIO_WriteBit(TB6612_BIN1_PORT, TB6612_BIN1_PIN, 1); GPIO_WriteBit(TB6612_BIN2_PORT, TB6612_BIN2_PIN, 0);} while(0)
#define MOTOR_RIGHT_BACKWARD do {GPIO_WriteBit(TB6612_BIN1_PORT, TB6612_BIN1_PIN, 0); GPIO_WriteBit(TB6612_BIN2_PORT, TB6612_BIN2_PIN, 1);} while(0)
#define MOTOR_RIGHT_STOP     do {GPIO_WriteBit(TB6612_BIN1_PORT, TB6612_BIN1_PIN, 0); GPIO_WriteBit(TB6612_BIN2_PORT, TB6612_BIN2_PIN, 0);} while(0)

// 电机编号定义
#define MOTOR_LEFT   0
#define MOTOR_RIGHT  1
#define MOTOR_ALL    2

// 函数声明
void TB6612_Init(void);                    // TB6612初始化
void Motor_SetDirection(uint8_t motor, uint8_t direction);  // 设置电机方向
void Motor_Forward(int left_speed, int right_speed);  // 前进
void Motor_Backward(int left_speed, int right_speed); // 后退
void Motor_LeftTurn(uint16_t speed);       // 左转
void Motor_RightTurn(uint16_t speed);      // 右转
void Motor_Stop(void);            // 停止电机

#endif /* __TB6612_H */
