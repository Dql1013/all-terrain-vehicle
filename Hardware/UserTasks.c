/*
********************************************************************************************************
文 件 名：UserTasks.c
函数功能：基础之外的功能实现
入口参数：
返 回 值：
日    期：
接    线：
伸缩电机（输出5V）
        继电器 --- PA4
辅助轮（借用M1+,M1-）
        M4+ --- M1+
        M4- --- M1-
*********************************************************************************************************
*/
/*
********************************************************************************************************
更新日志：
添加伸缩电机控制功能，支持高低电平控制、定时控制和到位停止
统一头文件引用，使用SYS.h作为唯一入口
*********************************************************************************************************
*/
#include "UserTasks.h"
#include "Delay.h"

/**
  * 函    数：用户任务初始化
  * 参    数：无
  * 返 回 值：无
  * 功    能：初始化所有用户任务相关的硬件
  */
void UserTasks_Init(void)
{
    // 伸缩电机引脚已经在GPIO_All_Init中初始化，这里可以添加其他用户任务的初始化
}

/**
  * 函    数：伸缩电机方向控制
  * 参    数：direction - 控制方向（MOTOR3_EXTEND/MOTOR3_RETRACT/MOTOR3_STOP）
  * 返 回 值：无
  * 功    能：控制伸缩电机的伸出、缩回或停止
  */
void Motor3_Control(uint8_t direction)
{
    switch(direction)
    {
        case MOTOR3_EXTEND:
            // 设置电机正转（伸出）
            GPIO_SetBits(MOTOR3_PORT, MOTOR3_PIN_PLUS);  // PC14输出3.3V
            break;
        case MOTOR3_STOP:
        default:
            // 停止电机
            GPIO_ResetBits(MOTOR3_PORT,  MOTOR3_PIN_PLUS);  // 都输出0V
            break;
    }
}

/**
  * 函    数：伸缩电机定时控制
  * 参    数：direction - 控制方向（MOTOR3_EXTEND/MOTOR3_RETRACT）
  *         time_ms - 运行时间（毫秒）
  * 返 回 值：无
  * 功    能：控制伸缩电机按照指定方向运行指定时间后自动停止
  */
void Motor3_TimeControl(void)
{
    
    
    // 启动电机
    Motor3_Control(MOTOR3_EXTEND);
    
    // 延时指定时间
    Delay_ms(900);
    
    // 停止电机
    Motor3_Control(MOTOR3_STOP);
}
