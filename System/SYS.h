#ifndef __SYS_H
#define __SYS_H

// 基础库头文件
#include "stm32f10x.h"

// 系统核心模块头文件
#include "Delay.h"
#include "GPIO.h"
#include "Timer.h"

// 外设模块头文件
//#include "../Hardware/LED.h"
#include "../Hardware/Key.h"
#include "../Hardware/Encoder.h"
#include "../Hardware/TB6612.h"
#include "../Hardware/OLED.h"
#include "../Hardware/Tracks.h"
#include "../Hardware/UserTasks.h"
#include "../Hardware/PID.h"

 //编码器每转脉冲数（根据实际编码器修改此值）
#define ENCODER_PPR 14*30 //适用于MG540
//#define ENCODER_PPR 11*35 //适用于MG530
#define SAMPLE_PERIOD 0.1 //采样周期（秒）	9999/719



// 用户任务函数声明
void STM32_System_Init(void);
void oled_show(void);
void haixinbei(void);

#endif
