#include "PID.h"

float Err=0,LastErr=0,LastLastErr=0;
float pwm_CCR=0,Add_CCR=0;                                //pwm�µ�PWM�Ƚ�ֵ��add���µ�PWMռ�ձȸ���ֵ
float p=0.5,i=0.03,d=0.001;                         //����ϵ��������ϵ����΢��ϵ��
//float setspeed=800;                              //�趨Ŀ���ٶ�
float Position_Kp=0.02,Position_Ki=0.0002;
//λ��ʽPID������
int16_t Position_PI (int16_t Encoder, int16_t Target)
{
  //   
  static int16_t Bias, Pwm;
  static long Integral_bias;
  Bias = Encoder - Target;            //����ƫ��
  Integral_bias += Bias;	             //���ƫ��Ļ���
  if(Integral_bias > 1500000)  Integral_bias = 1500000; //�����޷�
  if(Integral_bias < -1500000)  Integral_bias = -1500000; //�����޷�
  Pwm = Position_Kp * Bias + Position_Ki * Integral_bias; //λ��ʽPI������
  return Pwm;                         //�������
}
//����PI������
float Incremental_Kp=1,Incremental_Ki=1;
int16_t Incremental_PI (int16_t Encoder, int16_t Target)
{
  //   
  static int16_t Bias, Pwm, Last_bias;
  Bias = Encoder - Target;            //����ƫ��
  Pwm += Incremental_Kp * (Bias - Last_bias) + Incremental_Ki * Bias; //����ʽPI������
  Last_bias = Bias;	                 //������һ��ƫ��
  return Pwm;                         //�������
}

int16_t PID(int16_t speed,int16_t setspeed)
{
    Err=setspeed-speed;                            										 //�趨�ٶ�-ʵʱ�ٶ�
    Add_CCR = p*(Err-LastErr)+i*(Err)+d*(Err+LastLastErr-2*LastErr);   //PWMռ�ձ����ӵĲ���=��������+���ֻ���+΢�ֻ���
	if(Add_CCR<-1||Add_CCR>1)                                                //�޷���΢С�仯�����ģ����ٶ���
	{
	 pwm_CCR+=Add_CCR;                                                      
	}
	 
	  if(pwm_CCR>9999)
        pwm_CCR=9999;
    if(pwm_CCR<0)
        pwm_CCR=0;
    LastLastErr=LastErr;                       										 //����һ����ֵ�����ϴ����
    LastErr=Err;																									 //��������ֵ���ϴ����
    return pwm_CCR;																										 //����PWM�µ�ռ�ձ�ֵ
}

