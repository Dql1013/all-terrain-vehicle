#include "PID.h"

float Err_L=0,LastErr_L=0,LastLastErr_L=0;
float Err_R=0,LastErr_R=0,LastLastErr_R=0;
float pwm_CCR_L=0,Add_CCR_L=0;                                //pwm�µ�PWM�Ƚ�ֵ��add���µ�PWMռ�ձȸ���ֵ
float pwm_CCR_R=0,Add_CCR_R=0;                                //pwm�µ�PWM�Ƚ�ֵ��add���µ�PWMռ�ձȸ���ֵ

float p=70,i=1,d=1;                         //����ϵ��������ϵ����΢��ϵ��
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
float Incremental_Kp=0.7,Incremental_Ki=0.1;
int16_t Incremental_PI (int16_t Encoder, int16_t Target)
{
  //   
  static int16_t Bias, Pwm, Last_bias;
  Bias = Encoder - Target;            //����ƫ��
  Pwm += Incremental_Kp * (Bias - Last_bias) + Incremental_Ki * Bias; //����ʽPI������
  Last_bias = Bias;	                 //������һ��ƫ��
  return Pwm;                         //�������
}

int16_t PID_L(int16_t speed,int16_t setspeed)
{
    Err_L=setspeed-speed;                            										 //�趨�ٶ�-ʵʱ�ٶ�
    Add_CCR_L = p*(Err_L-LastErr_L)+i*(Err_L)+d*(Err_L+LastLastErr_L-2*LastErr_L);   //PWMռ�ձ����ӵĲ���=��������+���ֻ���+΢�ֻ���
//	if(Add_CCR_L<-1||Add_CCR_L>1)                                                //�޷���΢С�仯�����ģ����ٶ���
//	{
//	                                                    
//	}
	    pwm_CCR_L+=Add_CCR_L;
	  if(pwm_CCR_L > 9999)
        pwm_CCR_L = 9999;
    if(pwm_CCR_L < 300)
        pwm_CCR_L = 300;
    LastLastErr_L=LastErr_L;                       											//����һ����ֵ�����ϴ����
    LastErr_L=Err_L;																										//��������ֵ���ϴ����
    return pwm_CCR_L;																					//����PWM�µ�ռ�ձ�ֵ
		
}
int16_t PID_R(int16_t speed,int16_t setspeed)
{
    Err_R=setspeed-speed;                            										 //�趨�ٶ�-ʵʱ�ٶ�
    Add_CCR_R = p*(Err_R-LastErr_R)+i*(Err_R)+d*(Err_R+LastLastErr_R-2*LastErr_R);   //PWMռ�ձ����ӵĲ���=��������+���ֻ���+΢�ֻ���
//	if(Add_CCR_R<-1||Add_CCR_R>1)                                                //�޷���΢С�仯�����ģ����ٶ���
//	{
//	                                                  
//	}
	    pwm_CCR_R+=Add_CCR_R;  
	  if(pwm_CCR_R > 9999)
        pwm_CCR_R = 9999;
		if(pwm_CCR_R < 300)
        pwm_CCR_R = 300;
//    if(pwm_CCR<0)
//        pwm_CCR=0;
    LastLastErr_R=LastErr_R;                       											//����һ����ֵ�����ϴ����
    LastErr_R=Err_R;																										//��������ֵ���ϴ����
    return pwm_CCR_R;																					//����PWM�µ�ռ�ձ�ֵ
		
}
