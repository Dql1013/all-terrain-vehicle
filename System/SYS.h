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

// 用户任务函数声明
void STM32_System_Init(void);
void oled_show(void);
void haixinbei(void);
void Update_Counter(void);  // 新增
void Find(void);            // 新增

#endif
