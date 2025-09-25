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
#include "Delay.h"
#include "TB6612.h"

// 声明外部变量
extern uint16_t CrossAndBlackAreaCount;  // 从SYS.c引用的全局计数变量

// 传感器阈值（根据实际调试调整）
#define BLACK_THRESHOLD 0  // 黑色线的阈值（0表示检测到黑色）

/**
  * 函    数：循迹传感器初始化
  * 参    数：无
  * 返 回 值：无
  * 功    能：初始化所有循迹传感器的GPIO引脚
  */
void Tracks_Init(void)
{
    // 调用System/GPIO.c中的GPIO_Tracks_Init函数初始化引脚
    GPIO_Tracks_Init();
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
  * 返 回 值：循迹状态（左转、右转、直行、十字路口等）
  * 功    能：根据传感器读数判断当前的路径状态
  */
uint8_t Tracks_GetStatus(uint16_t tracks_value)
{
    // 计算检测到黑色的传感器数量
    uint8_t black_count = 0;
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
    
    // 检测十字路口
    if (Tracks_DetectCrossroad())
    {
        return TRACKS_CROSSROAD;
    }
    
    // 检测左直角弯
    uint16_t left_angle_pattern = 0x19;  // 0b00001111，左半部分全黑，右半部分全白
										 // 0b00011001
    if ((tracks_value & 0xFF) == left_angle_pattern)
    {
        return TRACKS_LEFT_ANGLE;
    }
    
    // 检测右直角弯
    uint16_t right_angle_pattern = 0x98;  // 0b11110000，右半部分全黑，左半部分全白
										  // 0b10011000
    if ((tracks_value & 0xFF) == right_angle_pattern)
    {
        return TRACKS_RIGHT_ANGLE;
    }
    

    // 计算偏差值判断方向
    int16_t deviation = Tracks_GetDeviation(tracks_value);
    
    if (deviation < -50)  // 大幅偏左
    {
        return TRACKS_LEFT_TURN;
    }
    else if (deviation > 50)  // 大幅偏右
    {
        return TRACKS_RIGHT_TURN;
    }
    else  // 基本直行
    {
        return TRACKS_STRAIGHT;
    }
}

/**
  * 函    数：检测十字路口
  * 参    数：无
  * 返 回 值：1=检测到十字路口，0=未检测到
  * 功    能：通过检测横向和纵向的黑线判断是否为十字路口
  */
uint8_t Tracks_DetectCrossroad(void)
{
    // 读取当前传感器状态
    uint16_t value = Tracks_Read();
    
    // 检测前后左右是否都有黑线
    // 这里使用一个简单的判断方法：中心区域有黑线且两边也有较多黑线
    // 将二进制字面量改为十六进制表示
    uint8_t center_black = ((value & 0x0F0) != 0);  // 中间4个传感器是否有黑色
    uint8_t side_black = ((value & 0x00F) != 0) && ((value & 0xF00) != 0);  // 两边是否都有黑色
    
    // 可以增加额外的确认步骤，例如短暂停止或减速后再次检测
    if (center_black && side_black)
    {
        // 为了提高准确性，可以进行二次确认
        Delay_ms(10);
        uint16_t value2 = Tracks_Read();
        uint8_t center_black2 = ((value2 & 0x0F0) != 0);
        uint8_t side_black2 = ((value2 & 0x00F) != 0) && ((value2 & 0xF00) != 0);
        
        return (center_black2 && side_black2);
    }
    
    return 0;
}

/**
  * 函    数：检测并计数黑区
  * 参    数：无
  * 返 回 值：当前黑区计数
  * 功    能：当传感器1和8同时检测到黑色时，增加CrossAndBlackAreaCount计数
  */
uint16_t Tracks_CheckAndCountBlackArea(void)
{
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
int16_t Tracks_GetDeviation(uint16_t tracks_value)
{
    int32_t weighted_sum = 0;
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
    if (total_weight == 0)
    {
        return 0;
    }
    
    // 计算偏差值（-100到100）
    int16_t deviation = (weighted_sum * 100) / (total_weight * 4);
    
    // 限制偏差值范围
    if (deviation < -100) deviation = -100;
    if (deviation > 100) deviation = 100;
    
    return deviation;
}

/**
  * 函    数：轨迹控制函数
  * 参    数：left_speed - 左电机速度
  *         right_speed - 右电机速度
  * 返 回 值：无
  * 功    能：根据传感器检测结果控制小车运动方向
  */
void Tracks_Control(uint16_t left_speed, uint16_t right_speed)
{
    // 读取传感器状态
    uint16_t tracks_value = Tracks_Read();
    
    // 获取循迹状态
    uint8_t status = Tracks_GetStatus(tracks_value);
    
    // 根据状态控制小车运动
//    switch (status)
//    {
//        case TRACKS_LEFT_TURN:
//            // 大幅偏左，需要左转调整
//            Motor_LeftTurn(9000);
//            break;
//        case TRACKS_RIGHT_TURN:
//            // 大幅偏右，需要右转调整
//            Motor_RightTurn(9000);
//            break;
//        case TRACKS_STRAIGHT:
//            // 基本直行
//            Motor_Forward(5000, 5000);
//            break;
//        case TRACKS_CROSSROAD:
//            // 检测到十字路口，继续直行
//            Motor_Forward(5000, 5000);
//            break;
//        case TRACKS_LEFT_ANGLE:
//            // 左直角弯
//            Motor_LeftTurn(9000);
//            break;
//        case TRACKS_RIGHT_ANGLE:
//            // 右直角弯
//            Motor_RightTurn(9000);
//            break;
//        case TRACKS_LOST:
//            // 丢失轨迹，停止电机
//            Motor_Stop();
//            break;
//        default:
//            // 默认情况下保持直行
//            Motor_Forward(5000, 5000);
//            break;
//    }
}

