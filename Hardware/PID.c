#include "PID.h"

float Err=0,LastErr=0,LastLastErr=0;
float pwm_CCR=0,Add_CCR=0;                                //pwm新的PWM比较值，add，新的PWM占空比更改值
float p=0.5,i=0.03,d=0.001;                         //比例系数、积分系数、微分系数
//float setspeed=800;                              //设定目标速度
float Position_Kp=0.02,Position_Ki=0.0002;
//位置式PID控制器
int16_t Position_PI (int16_t Encoder, int16_t Target)
{
  //   
  static int16_t Bias, Pwm;
  static long Integral_bias;
  Bias = Encoder - Target;            //计算偏差
  Integral_bias += Bias;	             //求出偏差的积分
  if(Integral_bias > 1500000)  Integral_bias = 1500000; //积分限幅
  if(Integral_bias < -1500000)  Integral_bias = -1500000; //积分限幅
  Pwm = Position_Kp * Bias + Position_Ki * Integral_bias; //位置式PI控制器
  return Pwm;                         //增量输出
}
//增量PI控制器
float Incremental_Kp=1,Incremental_Ki=1;
int16_t Incremental_PI (int16_t Encoder, int16_t Target)
{
  //   
  static int16_t Bias, Pwm, Last_bias;
  Bias = Encoder - Target;            //计算偏差
  Pwm += Incremental_Kp * (Bias - Last_bias) + Incremental_Ki * Bias; //增量式PI控制器
  Last_bias = Bias;	                 //保存上一次偏差
  return Pwm;                         //增量输出
}

int16_t PID(int16_t speed,int16_t setspeed)
{
    Err=setspeed-speed;                            										 //设定速度-实时速度
    Add_CCR = p*(Err-LastErr)+i*(Err)+d*(Err+LastLastErr-2*LastErr);   //PWM占空比增加的部分=比例环节+积分环节+微分环节
	if(Add_CCR<-1||Add_CCR>1)                                                //限幅，微小变化不更改，减少抖动
	{
	 pwm_CCR+=Add_CCR;                                                      
	}
	 
	  if(pwm_CCR>9999)
        pwm_CCR=9999;
    if(pwm_CCR<0)
        pwm_CCR=0;
    LastLastErr=LastErr;                       										 //把上一次误差赋值给上上次误差
    LastErr=Err;																									 //把最新误差赋值给上次误差
    return pwm_CCR;																										 //返回PWM新的占空比值
}

