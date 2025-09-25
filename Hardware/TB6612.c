/*
********************************************************************************************************
文 件 名：TB6612.c
函数功能：TB6612电机驱动实现
入口参数：无
返 回 值：无
日    期：
接    线：
        PWMA---PA2
        AIN2---PB15
        AIN1---PB14
        BIN1---PB13
        BIN2---PB12
        PWMB---PA3
*********************************************************************************************************
*/
/*
		Motor_Forward (left_speed , right_speed)
		Motor_Backward(left_speed , right_speed)
		Motor_LeftTurn (speed)
		Motor_RightTurn(speed)
		Motor_Stop()
*/
/*
********************************************************************************************************
更新日志：

*********************************************************************************************************
*/
#include "TB6612.h"

/**
  * 函    数：TB6612初始化
  * 参    数：无
  * 返 回 值：无
  * 功    能：初始化TB6612电机驱动所需的GPIO引脚
  */
void TB6612_Init(void)
{
    // 初始化GPIO（通过System/GPIO.c中的GPIO_TB6612_Init函数）
    GPIO_TB6612_Init();
    
    // 初始化时停止所有电机
    Motor_Stop();

}

/**
  * 函    数：设置电机方向
  * 参    数：motor - 电机编号（MOTOR_LEFT/MOTOR_RIGHT/MOTOR_ALL）
  *           direction - 方向（0-停止，1-前进，2-后退）
  * 返 回 值：无
  * 功    能：控制指定电机的旋转方向
  */
void Motor_SetDirection(uint8_t motor, uint8_t direction)
{
    if (motor == MOTOR_LEFT || motor == MOTOR_ALL)
    {
        switch (direction)
        {
            case 0:  // 停止
                MOTOR_LEFT_STOP;
                break;
            case 1:  // 前进
                MOTOR_LEFT_FORWARD;
                break;
            case 2:  // 后退
                MOTOR_LEFT_BACKWARD;
                break;
        }
    }
    
    if (motor == MOTOR_RIGHT || motor == MOTOR_ALL)
    {
        switch (direction)
        {
            case 0:  // 停止
                MOTOR_RIGHT_STOP;
                break;
            case 1:  // 前进
                MOTOR_RIGHT_FORWARD;
                break;
            case 2:  // 后退
                MOTOR_RIGHT_BACKWARD;
                break;
        }
    }
}

/**
  * 函    数：小车前进
  * 参    数：left_speed - 左电机速度
  *           right_speed - 右电机速度
  * 返 回 值：无
  * 功    能：控制小车前进并设置左右电机速度
  */
void Motor_Forward(int left_speed, int right_speed)
{
    Motor_SetDirection(MOTOR_ALL, 1);  // 设置前进方向
    Set_PWM(left_speed, right_speed);  // 设置PWM占空比
}

/**
  * 函    数：小车后退
  * 参    数：left_speed - 左电机速度
  *           right_speed - 右电机速度
  * 返 回 值：无
  * 功    能：控制小车后退并设置左右电机速度
  */
void Motor_Backward(int left_speed, int right_speed)
{
    Motor_SetDirection(MOTOR_ALL, 2);  // 设置后退方向
    Set_PWM(left_speed, right_speed);  // 设置PWM占空比
}

/**
  * 函    数：小车左转
  * 参    数：speed - 转弯速度
  * 返 回 值：无
  * 功    能：控制小车向左转弯
  */
void Motor_LeftTurn(uint16_t speed)
{
    Motor_SetDirection(MOTOR_LEFT, 0);   // 左电机停止
    Motor_SetDirection(MOTOR_RIGHT, 1);  // 右电机前进
    Set_PWM(0, speed);                    // 设置PWM占空比：左轮0，右轮9000
}

/**
  * 函    数：小车右转
  * 参    数：speed - 转弯速度
  * 返 回 值：无
  * 功    能：控制小车向右转弯
  */
void Motor_RightTurn(uint16_t speed)
{
    Motor_SetDirection(MOTOR_LEFT, 1);   // 左电机前进
    Motor_SetDirection(MOTOR_RIGHT, 0);  // 右电机停止
    Set_PWM(speed, 0);                    // 设置PWM占空比：左轮9000，右轮0
}

/**
  * 函    数：停止电机
  * 参    数：motor - 电机编号（MOTOR_LEFT/MOTOR_RIGHT/MOTOR_ALL）
  * 返 回 值：无
  * 功    能：停止指定的电机
  */
void Motor_Stop(void)
{
    Motor_SetDirection(MOTOR_ALL, 0);  // 设置停止方向
    
    // 关闭PWM输出
    Set_PWM(0, 0);
}
