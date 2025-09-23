/*
********************************************************************************************************
文件名：SYS.c
功  能：初始化所有外设，统一系统入口
备  注：集中管理所有外设的初始化和控制
日  期: 2024-10-5
*********************************************************************************************************
*/

/*
********************************************************************************************************
更新日志：
10.5;新增SYS.c
10.5:新增haixinbei()
10.15:优化系统初始化，整合所有外设初始化
10.16:完善SYS模块，集成所有外设功能
*********************************************************************************************************
*/

#include "SYS.h"


// 全局变量定义
uint16_t CrossAndBlackAreaCount = 0;  // 黑区计数变量
uint8_t MAP_count = 0;                // 地图计数
uint8_t FIND_Flag = 0;                // 查找标志

// 路径控制相关变量
uint8_t case_0 = 0;
uint8_t case_1 = 0;
uint8_t case_2 = 0;
uint8_t case_3 = 0;
uint8_t case_4 = 0;
uint8_t case_5 = 0;
uint8_t case_6 = 0;
uint8_t case_7 = 0;
uint8_t case_8 = 0;
uint8_t case_9 = 0;
uint8_t case_10 = 0;
uint8_t case_11 = 0;
uint8_t case_12 = 0;
uint8_t case_13 = 0;
uint8_t case_14 = 0;
uint8_t case_15 = 0;
uint8_t case_16 = 0;
uint8_t case_17 = 0;
uint8_t case_18 = 0;

// 电机速度参数
uint8_t left_speed = 85;
uint8_t right_speed = 85;

/**
  * 函    数：更新计数器函数
  * 参    数：无
  * 返 回 值：无
  * 功    能：更新计数状态
  */
void Update_Counter(void)
{
    // 此处可以添加计数器更新的具体逻辑
}

/**
  * 函    数：查找路径函数
  * 参    数：无
  * 返 回 值：无
  * 功    能：控制小车寻找路径
  */
void Find(void)
{
    Tracks_Control(left_speed, right_speed);
}

/**
  * 函    数：系统全局初始化
  * 参    数：无
  * 返 回 值：无
  * 功    能：初始化所有系统组件和外设
  */
void STM32_System_Init(void)
{
    // 初始化GPIO引脚
    GPIO_All_Init();
    
    // 初始化定时器
    Timer_All_Init();
    
    // 初始化Hardware外设
//    LED_Init();         // LED初始化
//    Key_Init();         // 按键初始化
//    Encoder_Init();     // 编码器初始化
    TB6612_Init();      // 电机驱动初始化
    OLED_Init();        // OLED显示初始化
    // 在STM32_System_Init函数中取消注释
    Tracks_Init();      // 循迹传感器初始化
//    UserTasks_Init();   // 用户任务初始化
    
    // 系统初始化完成，点亮LED1作为指示
//    LED1_ON();
}

/**
  * 函    数：路径规划函数
  * 参    数：无
  * 返 回 值：无
  * 功    能：实现海心贝路径规划算法
  */
void haixinbei(void)
{
	oled_show();
  //伸腿

//	Motor3_Control(MOTOR3_EXTEND);
//	Motor_Forward(400,400);
////	Delay_ms(1000);
//	Car_Stop();
//  static uint8_t haixinbei_flag = 0;
//  if(haixinbei_flag == 0)
//  {
//    haixinbei_flag = 1;
//    Motor3_TimeControl();
//  }
	
  // 直接在haixinbei函数中实现循迹功能
  // 读取传感器状态
  uint16_t tracks_value = Tracks_Read();
  
  // 判断循迹状态
  uint8_t status = Tracks_GetStatus(tracks_value);
  
  // 根据状态控制电机
  switch(status)
  {
      case TRACKS_STRAIGHT:
          // 直线行驶
          Motor_Forward(500, 500);
          break;
//      case TRACKS_LEFT_TURN:
//          // 左转 - 左轮停，右轮转
//		  Motor_Forward(0, 0);
//          break;
//      case TRACKS_RIGHT_TURN:
//          // 右转 - 右轮停，左轮转
//          Motor_Forward(0, 0);
//          break;
      case TRACKS_CROSSROAD:
          // 十字路口，可以根据需要处理
          Motor_Forward(500, 500);
          break;
      case TRACKS_LEFT_ANGLE:
          // 左直角弯
		  Motor_Forward(500 , 0);
          break;
      case TRACKS_RIGHT_ANGLE:
          // 右直角弯
          Motor_Forward(0 ,500);
          break;
      case TRACKS_LOST:
          // 丢失轨迹，停止或执行找回操作
          Motor_Stop();
          break;
      default:
          // 默认直行
          Motor_Forward(500, 500);
          break;
  }
	
	/*
	Update_Counter();
	if		 (CrossAndBlackAreaCount == 0  ) { MAP_count = 0 ; FIND_Flag = 0;Update_Counter();
				if (case_0  == 0){ Car_Stop();Delay_ms(50 );Car_Go(left_speed , right_speed);Delay_ms(150);case_0 = 1;}}        
	
	else if(CrossAndBlackAreaCount == 1  ) { MAP_count = 1 ; FIND_Flag = 1;Update_Counter();
				if (case_1  == 0){ Car_Stop();Delay_ms(100);case_1  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 2  ) { MAP_count = 2 ; FIND_Flag = 1;Update_Counter();
				if (case_2  == 0){ Car_Stop();Delay_ms(100);case_2  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}        //小阶梯
	
	else if(CrossAndBlackAreaCount == 3  ) { MAP_count = 3 ; FIND_Flag = 1;Update_Counter();
				if (case_3  == 0){ Car_Stop();Delay_ms(100);case_3  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 4  ) { MAP_count = 4 ; FIND_Flag = 1;Update_Counter();
				if (case_4  == 0){ Car_Stop();Delay_ms(100);case_4  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}        //防滑带    
	
	else if(CrossAndBlackAreaCount == 5  ) { Find();Delay_ms(70);Car_Stop();Delay_ms(50);}        //防滑带    
	
	else if(CrossAndBlackAreaCount == 5  ) { MAP_count = 5 ; FIND_Flag = 1;Update_Counter();
				if (case_5  == 0){ Car_Stop();Delay_ms(500);Car_Turn_Right(left_speed,right_speed);Delay_ms(100);
          case_5  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 6  ) { MAP_count = 6 ; FIND_Flag = 1;Update_Counter();
				if (case_6  == 0){ Car_Stop();Delay_ms(100);case_6  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}        //窄桥
	
	else if(CrossAndBlackAreaCount == 7  ) { MAP_count = 7 ; FIND_Flag = 1;Update_Counter();
				if (case_7  == 0){ Car_Stop();Delay_ms(500);Car_Turn_Right(left_speed,right_speed);Delay_ms(135);
          Car_Stop();Delay_ms(500);case_7  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 8  ) { MAP_count = 8 ; FIND_Flag = 1;Update_Counter();
				if (case_8  == 0){ Car_Stop();Delay_ms(100);case_8  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}        //大阶梯
	
	else if(CrossAndBlackAreaCount == 9  ) { MAP_count = 9 ; FIND_Flag = 1;Update_Counter();
				if (case_9  == 0){ Car_Stop();Delay_ms(500);Car_Turn_Right(left_speed,right_speed);Delay_ms(135);
          case_9  = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 10 ) { MAP_count = 10; FIND_Flag = 1;Update_Counter();
				if (case_10 == 0){ Car_Stop();Delay_ms(100);case_10 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}    //柔软草地
	
	else if(CrossAndBlackAreaCount == 11 ) { MAP_count = 11; FIND_Flag = 1;Update_Counter();
				if (case_11 == 0){ Car_Stop();Delay_ms(100);case_11 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 12 ) { MAP_count = 12; FIND_Flag = 1;Update_Counter();
				if (case_12 == 0){ Car_Stop();Delay_ms(100);case_12 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}        //减速带
	
	else if(CrossAndBlackAreaCount == 13 ) { MAP_count = 13; FIND_Flag = 1;Update_Counter();
				if (case_13 == 0){ Car_Stop();Delay_ms(500);Car_Turn_Right(left_speed,right_speed);Delay_ms(135);
          case_13 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 14 ) { MAP_count = 14; FIND_Flag = 1;Update_Counter();
				if (case_14 == 0){ Car_Stop();Delay_ms(500);Car_Turn_Right(left_speed,right_speed);Delay_ms(135);
          case_14 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 15 ) { MAP_count = 15; FIND_Flag = 1;Update_Counter();
				if (case_15 == 0){ Car_Stop();Delay_ms(100);case_15 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}        //栅格
	
	else if(CrossAndBlackAreaCount == 16 ) { MAP_count = 16; FIND_Flag = 1;Update_Counter();
				if (case_16 == 0){ Car_Stop();Delay_ms(100);case_16 = 1;}Find();Delay_ms(50);Car_Stop();Delay_ms(50);}
	
	else if(CrossAndBlackAreaCount == 17 ) { MAP_count = 17; FIND_Flag = 0;Update_Counter();
				if (case_17 == 0){ Car_Stop();Delay_ms(100);case_17 = 1;}if (case_17 == 1){ Car_Stop();Delay_ms(100);
          Car_Go(left_speed , right_speed);Delay_ms(200);case_17 = 2;}}    //窄桥前1/2
	
	else if(CrossAndBlackAreaCount == 18 ) { MAP_count = 18; FIND_Flag = 0;Update_Counter();
				if (case_18 == 0){ Car_Stop();Delay_ms(100);case_18 = 1;}if (case_18 == 1){ Car_Stop();Delay_ms(100);
          Car_Go(left_speed , right_speed);Delay_ms(200);case_18 = 2;}}        //上高台
	
	else {MAP_count = 99 ;Car_Stop();}
	*/
}

/**
  * 函    数：OLED显示函数
  * 参    数：无
  * 返 回 值：无
  * 功    能：在OLED上显示系统状态信息
  */
void oled_show(void)
{
    // 显示系统状态信息
    uint16_t tracks_value = Tracks_Read();
    int16_t encoder_left = Encoder_GetCount(ENCODER_LEFT);
    int16_t encoder_right = Encoder_GetCount(ENCODER_RIGHT);
    
    OLED_Clear();
    OLED_ShowString(1, 1, "Tracks:");
    OLED_ShowBinNum(1, 8, tracks_value, 8);
    
    OLED_ShowString(2, 1, "Left En:");
    OLED_ShowSignedNum(2, 9, encoder_left, 5);
    
    OLED_ShowString(3, 1, "Right En:");
    OLED_ShowSignedNum(3, 9, encoder_right, 5);
    
    OLED_ShowString(4, 1, "Status:");
    uint8_t status = Tracks_GetStatus(tracks_value);
    switch(status)
    {
        case TRACKS_STRAIGHT: OLED_ShowString(4, 8, "STRAIGHT"); break;
        case TRACKS_LEFT_TURN: OLED_ShowString(4, 8, "LEFT"); break;
        case TRACKS_RIGHT_TURN: OLED_ShowString(4, 8, "RIGHT"); break;
        case TRACKS_CROSSROAD: OLED_ShowString(4, 8, "CROSS"); break;
        case TRACKS_LEFT_ANGLE: OLED_ShowString(4, 8, "L-ANGLE"); break;
        case TRACKS_RIGHT_ANGLE: OLED_ShowString(4, 8, "R-ANGLE"); break;
        case TRACKS_LOST: OLED_ShowString(4, 8, "LOST"); break;
    }
}





