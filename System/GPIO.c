/*
********************************************************************************************************
文 件 名：GPIO.c
功    能：GPIO引脚初始化和管理
日    期: 2024-10-1
备    注：集中管理所有外设的GPIO初始化

*********************************************************************************************************
*/
/*
********************************************************************************************************
更新日志：
10.1：这种地方建议少来，你改乱了我都不一定能改回来
10.1：完善GPIO初始化功能，集中管理所有外设引脚
*********************************************************************************************************
*/

#include "GPIO.h"

/**
  * 函    数：OLED引脚初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_OLED_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	
	GPIO_InitStructure.GPIO_Pin = OLED_D0 | OLED_D1 ;
  GPIO_Init(OLED_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = OLED_RES | OLED_DC | OLED_CS ;
  GPIO_Init(OLED_PORT_C, &GPIO_InitStructure);
	
//	GPIO_InitStructure.GPIO_Pin = OLED_D0;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = OLED_D1;
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = OLED_RES;
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = OLED_DC;
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);
//	GPIO_InitStructure.GPIO_Pin = OLED_CS;
// 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	OLED_W_D0(0);
	OLED_W_D1(1);
	OLED_W_RES(1);
	OLED_W_DC(1);
	OLED_W_CS(1);
}

/**
  * 函    数：按键引脚初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_Key_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = KEY_PIN ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(KEY_PORT, &GPIO_InitStructure);
}

/**
  * 函    数：TB6612电机驱动引脚初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_TB6612_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = TB6612_AIN1_PIN | TB6612_AIN2_PIN | 
                                  TB6612_BIN1_PIN | TB6612_BIN2_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	    // 设置PWM引脚 PA2, PA3
    GPIO_InitStructure.GPIO_Pin = TB6612_PWMA | TB6612_PWMB;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
}

/**
  * 函    数：编码器引脚初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_Encoder_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 左编码器
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = ENCODER_A_LEFT | ENCODER_B_LEFT;
    GPIO_Init(ENCODER_PORT_LEFT, &GPIO_InitStructure);
    
    // 右编码器
    GPIO_InitStructure.GPIO_Pin = ENCODER_A_RIGHT | ENCODER_B_RIGHT;
    GPIO_Init(ENCODER_PORT_RIGHT, &GPIO_InitStructure);
}

/**
  * 函    数：红外寻迹模块引脚初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_Tracks_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    
    // 禁用JTAG功能，将PB3设置为普通GPIO
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //下拉PA12，
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    
    // 寻迹传感器PA端口
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_InitStructure.GPIO_Pin = TRACKS_PIN_1 | TRACKS_PIN_2 | TRACKS_PIN_3 | TRACKS_PIN_4 ;
    GPIO_Init(TRACKS_PORT, &GPIO_InitStructure);
    
    // 寻迹传感器PB端口
    GPIO_InitStructure.GPIO_Pin = TRACKS_PIN_5 | TRACKS_PIN_6 | TRACKS_PIN_7 | TRACKS_PIN_8;
    GPIO_Init(TRACKS_PORT_B, &GPIO_InitStructure);
}


/**
  * 函    数：伸缩电机引脚初始化
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_Motor3_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = MOTOR3_PIN_PLUS ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR3_PORT, &GPIO_InitStructure);
}

/**
  * 函    数：初始化所有GPIO
  * 参    数：无
  * 返 回 值：无
  */
void GPIO_All_Init(void)
{
		GPIO_OLED_Init();
    GPIO_Key_Init();
    GPIO_TB6612_Init();
    GPIO_Encoder_Init();
    GPIO_Tracks_Init();
    GPIO_Motor3_Init(); // 添加伸缩电机初始化
}

