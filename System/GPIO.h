#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f10x.h"

// 引脚定义宏 - 根据硬件连接整理

//#define OLED_PORT GPIOA
//#define OLED_D0  GPIO_Pin_1
//#define OLED_D1  GPIO_Pin_0

//#define OLED_PORT_C GPIOC
//#define OLED_RES  GPIO_Pin_15
//#define OLED_DC   GPIO_Pin_14
//#define OLED_CS   GPIO_Pin_13

#define OLED_PORT GPIOA
#define OLED_CS GPIO_Pin_1
#define OLED_DC GPIO_Pin_0

#define OLED_PORT_C GPIOC
#define OLED_RES  GPIO_Pin_15
#define OLED_D1		GPIO_Pin_14
#define OLED_D0		GPIO_Pin_13

#define OLED_W_D0(x)		GPIO_WriteBit(OLED_PORT_C, OLED_D0 , (BitAction)(x))
#define OLED_W_D1(x)		GPIO_WriteBit(OLED_PORT_C, OLED_D1 , (BitAction)(x))
#define OLED_W_RES(x)		GPIO_WriteBit(OLED_PORT_C, OLED_RES, (BitAction)(x))
#define OLED_W_DC(x)		GPIO_WriteBit(OLED_PORT  , OLED_DC , (BitAction)(x))
#define OLED_W_CS(x)		GPIO_WriteBit(OLED_PORT  , OLED_CS , (BitAction)(x))

#define KEY_PORT         GPIOB
#define KEY_PIN          GPIO_Pin_4

#define TB6612_IN_PORT GPIOB
#define TB6612_AIN1_PIN  GPIO_Pin_15
#define TB6612_AIN2_PIN  GPIO_Pin_14
#define TB6612_BIN1_PIN  GPIO_Pin_12
#define TB6612_BIN2_PIN  GPIO_Pin_13

#define TB6612_PWM GPIOA
#define TB6612_PWMA  GPIO_Pin_2
#define TB6612_PWMB  GPIO_Pin_3

#define ENCODER_PORT_LEFT  GPIOA
#define ENCODER_A_LEFT     GPIO_Pin_6
#define ENCODER_B_LEFT     GPIO_Pin_7
#define ENCODER_PORT_RIGHT GPIOB
#define ENCODER_A_RIGHT    GPIO_Pin_7
#define ENCODER_B_RIGHT    GPIO_Pin_6

#define TRACKS_PORT       GPIOA
#define TRACKS_PIN_1      GPIO_Pin_8
#define TRACKS_PIN_2      GPIO_Pin_9
#define TRACKS_PIN_3      GPIO_Pin_10
#define TRACKS_PIN_4      GPIO_Pin_11

#define TRACKS_PORT_B     GPIOB
#define TRACKS_PIN_5      GPIO_Pin_0
#define TRACKS_PIN_6      GPIO_Pin_1
#define TRACKS_PIN_7      GPIO_Pin_10
#define TRACKS_PIN_8      GPIO_Pin_11

// 伸缩电机引脚定义
#define MOTOR3_PORT       GPIOC
#define MOTOR3_PIN_PLUS   GPIO_Pin_14

// 函数声明
void GPIO_All_Init(void);
void GPIO_OLED_Init(void);
void GPIO_LED_Init(void);
void GPIO_Key_Init(void);
void GPIO_TB6612_Init(void);
void GPIO_Encoder_Init(void);
void GPIO_Tracks_Init(void);
void GPIO_Motor3_Init(void);

// GPIO操作宏定义
#define GPIO_WriteBit(Port, Pin, BitVal)   ((BitVal) ? \
                                           GPIO_SetBits(Port, Pin) : \
                                           GPIO_ResetBits(Port, Pin))
#define GPIO_ReadInputBit(Port, Pin)       GPIO_ReadInputDataBit(Port, Pin)
#define GPIO_ReadOutputBit(Port, Pin)      GPIO_ReadOutputDataBit(Port, Pin)

#endif /* __GPIO_H */
