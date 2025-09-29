/*
********************************************************************************************************
文 件 名：Tracks.c
函数功能：红外寻迹传感器实现
入口参数：无
返 回 值：无
日    期：
接    线：
        1---PA8
        2---PA9
        3---PA10
        4---PA11
        5---PB0
        6---PB1
        7---PB10
        8---PB11
*********************************************************************************************************
*/
/*
********************************************************************************************************
更新日志：

*********************************************************************************************************
*/
#include "Tracks.h"



// 声明外部变量
extern uint16_t CrossAndBlackAreaCount;  // 从SYS.c引用的全局计数变量
extern uint16_t weighted_sum;
// 传感器阈值（根据实际调试调整）
#define BLACK_THRESHOLD 0  // 黑色线的阈值（0表示检测到黑色）

/**
  * 函    数：取反
  * 参    数：data
  * 返 回 值：atad[dog]
  * 功    能：16进制反向用于判断
  */
uint8_t byte_reverse(uint8_t data) {
	data = ((data & 0xAA) >> 1) | ((data & 0x55) << 1);
	data = ((data & 0xCC) >> 2) | ((data & 0x33) << 2);
	data = (data >> 4) | (data << 4);
	return data;
}

/**
  * 函    数：读取所有循迹传感器状态
  * 参    数：无
  * 返 回 值：16位整数，每一位代表一个传感器的状态（0=检测到黑色，1=检测到白色）
  * 功    能：读取8路循迹传感器的当前状态
  */
uint16_t Tracks_Read(void)
{
    uint16_t value = 0;
    
    // 读取8路传感器状态，PA8~PA15，PB3~PB4
		//value--->0b[1]~[8]
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT_B, TRACKS_PIN_8) << 0);  // 传感器1
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT_B, TRACKS_PIN_7) << 1);  // 传感器2
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT_B, TRACKS_PIN_6) << 2);  // 传感器3
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT_B, TRACKS_PIN_5) << 3);  // 传感器4
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT, TRACKS_PIN_4) << 4);  // 传感器5
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT, TRACKS_PIN_3) << 5);  // 传感器6
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT, TRACKS_PIN_2) << 6);  // 传感器7
    value |= (GPIO_ReadInputDataBit(TRACKS_PORT, TRACKS_PIN_1) << 7);  // 传感器8
    
    return value;
}

/**
  * 函    数：获取循迹状态
  * 参    数：tracks_value - 传感器读数
  * 返 回 值：循迹状态（左转、右转、直行、全黑全白等）
  * 功    能：根据传感器读数判断当前的路径状态
  */
uint8_t Tracks_GetStatus(void)
{
		//tracks_value--->0b[1]~[8]
		uint16_t tracks_value = Tracks_Read();
    // 计算检测到黑色的传感器数量
    uint8_t black_count = 0;
		int16_t deviation = Tracks_GetDeviation();
		
    for (uint8_t i = 0; i < TRACKS_NUM; i++)
    {
        if ((tracks_value & (1 << i)) == BLACK_THRESHOLD)
        {
            black_count++;
        }
    }
		// 如果所有传感器都检测到白色，说明丢失了轨迹
    if (black_count == 0)
    {
        return TRACKS_LOST;
    }
		// 如果所有传感器都检测到黑色，说明全黑
    else if (black_count == 8)
    {
        return TRACKS_LOST;
    }
		
    // 检测左直角弯
		else if(GPIO_ReadInputDataBit(TRACKS_PORT, TRACKS_PIN_1) == BLACK_THRESHOLD)
    {
        return TRACKS_LEFT_ANGLE;
    }
    
    // 检测右直角弯
		else if(GPIO_ReadInputDataBit(TRACKS_PORT_B, TRACKS_PIN_8) == BLACK_THRESHOLD)
			{
					return TRACKS_RIGHT_ANGLE;
			}
		// 偏左	
    else if (deviation < -1)  
			{
					return TRACKS_RIGHT_TURN;
			}
		// 偏右	
		else if (deviation > 1)  
			{
					return TRACKS_LEFT_TURN;
			}
		//走就完了	
		else return TRACKS_STRAIGHT;
				
//    return ELSE;
}


/**
  * 函    数：检测并计数黑区
  * 参    数：无
  * 返 回 值：当前黑区计数
  * 功    能：当传感器1和8同时检测到黑色时，增加CrossAndBlackAreaCount计数
  */
uint16_t Tracks_CheckAndCountBlackArea(void)
{
		//tracks_value--->0b[1]~[8]
    uint16_t current_value = Tracks_Read();
    static uint8_t prev_state = 0;  // 记录上一次的状态，防止重复计数
    
    // 检测传感器1和传感器8是否同时检测到黑色
    // 传感器1: 第0位，传感器8: 第7位
    uint8_t sensor1_black = ((current_value & (1 << 0)) == BLACK_THRESHOLD);
    uint8_t sensor8_black = ((current_value & (1 << 7)) == BLACK_THRESHOLD);
    
    // 当两个传感器都检测到黑色，且上一次状态不是全黑时，计数加1
    if (sensor1_black && sensor8_black && (prev_state == 0))
    {
        CrossAndBlackAreaCount++;
        prev_state = 1;  // 更新状态为全黑
    }
    // 当两个传感器不同时检测到黑色时，重置状态
    else if (!(sensor1_black && sensor8_black))
    {
        prev_state = 0;  // 更新状态为非全黑
    }
    
    return CrossAndBlackAreaCount;
}

/**
  * 函    数：获取轨迹偏差值
  * 参    数：tracks_value - 传感器读数
  * 返 回 值：偏差值（-100到100）
  * 功    能：计算当前轨迹相对于中心的偏差，用于PID控制
  */
int16_t Tracks_GetDeviation(void)
{
	//tracks_value--->0b[1]~[8]
	uint16_t tracks_value = Tracks_Read();
	
    weighted_sum = 0;
    int32_t total_weight = 0;
    
    // 为每个传感器分配权重，越靠近中间权重越大
    int8_t weights[TRACKS_NUM] = {-4, -3, -2, -1, 1, 2, 3, 4};  // 8路传感器的权重
    // 计算加权和
    for (uint8_t i = 0; i < TRACKS_NUM; i++)
    {
        // 如果检测到黑色线（值为0）
        if ((tracks_value & (1 << i)) == BLACK_THRESHOLD)
        {
            weighted_sum += weights[i];
            total_weight += 1;
        }
    }
    
    // 防止除零错误
    if (total_weight == TRACKS_NUM)
    {
        return weighted_sum;
    }
		return 0;
}


/**
  * 函    数：轨迹控制函数
  * 参    数：left_speed - 左电机速度
  *           right_speed - 右电机速度
  * 返 回 值：无
  * 功    能：根据传感器检测结果控制小车运动方向
  */
void Tracks_Control(int left_speed,int right_speed)
{
    // 获取循迹状态
    uint8_t status = Tracks_GetStatus();
	
    left_speed  = left_speed  + Tracks_GetDeviation();
	right_speed = right_speed - Tracks_GetDeviation();
	
    // 根据状态控制小车运动
    switch (status)
    {
        case TRACKS_LEFT_TURN:
            // 大幅偏左，需要左转调整
            Motor_LeftTurn(250);
            break;
        case TRACKS_RIGHT_TURN:
            // 大幅偏右，需要右转调整
            Motor_RightTurn(250);
            break;
        case TRACKS_STRAIGHT:
            // 基本直行
            Motor_Forward(left_speed, right_speed);
            break;
        case TRACKS_CROSSROAD:
            // 检测到十字路口，继续直行
            Motor_Forward(left_speed, right_speed);
            break;
        case TRACKS_LEFT_ANGLE:
            // 左直角弯
            Motor_LeftTurn_90();
            break;
        case TRACKS_RIGHT_ANGLE:
            // 右直角弯
            Motor_RightTurn_90();
            break;
        case TRACKS_LOST:
            // 丢失轨迹，停止电机
            Motor_Stop();
            break;
        default:
            // 默认情况下保持直行
            Motor_Forward(left_speed, right_speed);
            break;
    }
}

