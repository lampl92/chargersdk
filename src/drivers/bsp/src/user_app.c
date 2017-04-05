#include "stm32f4xx_hal.h"
#include "bsp.h"
#include <stdlib.h>
#include <math.h>
#include "bsp_timer.h"
extern  void IIC_Init(void);
extern IO_chip1 Chip1;
extern IO_chip2 Chip2;
extern void write_pca9554_1(void);
extern void Delay_ms(unsigned long long);
extern void PCA9554_init(void);
extern unsigned int pwm_r_1,pwm_g_1,pwm_b_1,pwm_r_2,pwm_g_2,pwm_b_2;
extern uint16_t pwm_ms;
AD_samp AD_samp_dma;
samp Sys_samp;
flag sys_flag;
unsigned short flag_gun_Close,flag_gun_Open;
float cp1,cp2,va;
unsigned short channel;
unsigned longCD4067_sum,leakage_current_sum,va_samp_sum,ia_samp_sum,CP2_sum,CP1_sum;
unsigned test_timer;
unsigned long CP1_sum_sys,CP2_sum_sys;
unsigned long  test1,test2,test3;
unsigned char num_cp1,num_cp2;
unsigned short vref=2014;
unsigned long CD4067_sum;
uint8_t DC_channel;
uint8_t flag_breath_r_1,flag_breath_r_2,flag_breath_g_1,flag_breath_g_2,flag_breath_b_1,flag_breath_b_2;
uint8_t pwm_r_flag_1,pwm_r_flag_2,pwm_g_flag_1,pwm_g_flag_2,pwm_b_flag_1,pwm_b_flag_2;
uint8_t duty_ratio_r_1,duty_ratio_r_2,duty_ratio_g_1,
  duty_ratio_g_2,duty_ratio_b_1,duty_ratio_b_2;
const float  resistance[154]=
{382.300,358.686,336.457,315.560,295.938,277.531,260.278,244.117,228.987,214.829,
  201.585,189.199,177.617,166.789,156.665,147.200,138.349,130.073,122.333,115.093,
  108.319,101.979,96.045,90.489,85.284,80.409,75.839,71.554,67.537,63.767,
  60.230,56.908,53.789,50.859,48.105,45.515,43.080,40.789,38.632,36.602,
  34.665,32.888,31.189,29.588,28.078,26.653,25.309,24.039,22.840,21.708,
  20.690,19.625,18.669,17.764,16.907,16.097,15.329,14.603,13.914,13.262,
  12.644,12.058,11.502,10.975,10.474,10.000,9.549,9.121,8.714,8.327,
  7.960,7.611,7.279,6.964,6.663,6.377,6.105,5.846,5.600,5.365,
  5.141,4.928,4.725,4.531,4.346,4.170,4.001,3.841,3.688,3.541,
  3.410,3.268,3.141,3.019,2.902,2.791,2.685,2.583,2.485,2.392,
  2.303,2.217,2.135,2.057,1.982,1.910,1.841,1.775,1.712,1.651,
  1.592,1.537,1.483,1.431,1.382,1.334,1.289,1.245,1.203,1.162,
  1.123,1.086,1.050,1.015,0.982,0.950,0.919,0.889,0.860,0.833,
  0.806,0.780,0.756,0.732,0.709,0.687,0.666,0.645,0.625,0.606,
  0.588,0.570,0.552,0.536,0.520,0.504 };


float get_ia(void)
{
	unsigned short i;
	unsigned long ia_sum=0;
	for(i=0;i<samp_sum;i++)
	{
		ia_sum+=((Sys_samp.AC_samp.ia_samp[i]-vref)*(Sys_samp.AC_samp.ia_samp[i]-vref));
	}
	ia_sum=ia_sum/samp_sum;
	Sys_samp.AC.ia=sqrt(ia_sum)*ia_k;
	ia_sum=0;
	return Sys_samp.AC.ia;
}
float get_va(void)
{
	unsigned short i;
	unsigned long long va_sum=0;
	for(i=0;i<samp_sum;i++)
	{
		va_sum+=((Sys_samp.AC_samp.va_samp[i]-vref)*(Sys_samp.AC_samp.va_samp[i]-vref));
	}
	va_sum=va_sum/samp_sum;
	Sys_samp.AC.va=sqrt(va_sum)*va_k;
	va_sum=0;
	return Sys_samp.AC.va;
}
float get_leakage_current(void)
{
	unsigned short i;
	for(i=0;i<samp_sum;i++)
	{
		leakage_current_sum+=(Sys_samp.AC_samp.leakage_current_samp[i]*Sys_samp.AC_samp.leakage_current_samp[i]);
	}
	leakage_current_sum=leakage_current_sum/samp_sum;
	Sys_samp.AC.leakage_current=sqrt(leakage_current_sum)*leakage_current_k;
	return Sys_samp.AC.leakage_current;
}
float get_CD4067(void)
{
	unsigned short i;
	for(i=0;i<samp_sum;i++)
	{
		CD4067_sum+=Sys_samp.DC_samp.CD4067[i];
	}
	Sys_samp.DC.CD4067=(CD4067_sum/samp_sum);//*temper_k;
	return Sys_samp.DC.CD4067;
}
float get_dc_massage(void)
{
	uint16_t j,ad_samp_value;
	float ad_value,re_value;

  Chip1.a_select=DC_channel>>0&0x01;
	Chip1.b_select=DC_channel>>1&0x01;
	Chip1.c_select=DC_channel>>2&0x01;
	Chip1.d_select=DC_channel>>3&0x01;
	Chip1.cs1_select=1;
	write_pca9554_1();
	Delay_ms(1);
	ad_samp_value=get_CD4067();
	switch (DC_channel)
	{
		case 0:
		ad_value=(float)(ad_samp_value*3)/4096;
		re_value=(ad_value*30)/(3-ad_value);
		for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP1=j-40;
				}
			}
						DC_channel++;
		break;
		case 1:
		ad_value=(float)ad_samp_value*3/4096;
		re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP2=j-40;
				}
			}
						DC_channel++;
		break;
				case 2:
		ad_value=(float)ad_samp_value*3/4096;
		re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP3=j-40;
				}
			}
				DC_channel++;
		break;
		case 3:
		ad_value=(float)ad_samp_value*3/4096;
	  re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP4=j-40;
				}
			}
							DC_channel++;
		break;
				case 4:
		ad_value=(float)ad_samp_value*3/4096;
		re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM1=j-40;
				}
			}
							DC_channel++;
		break;
		case 5:
		ad_value=(float)ad_samp_value*3/4096;
		re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM2=j-40;
				}
			}
							DC_channel++;
		break;
					case 6:
		ad_value=(float)ad_samp_value*3/4096;
		re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM3=j-40;
				}
			}
							DC_channel++;
		break;
		case 7:
		ad_value=(float)ad_samp_value*3/4096;
		re_value=(ad_value*30)/(3-ad_value);
				for(j = 0;j<=150;j++)
            {
				if((re_value>=(resistance[j+1]))&&(re_value<(resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM4=j-40;
				}
			}
							DC_channel++;
		break;
		case 8:
				DC_channel++;
		break;
		case 9:
				DC_channel++;
		break;
		case 10:
				DC_channel++;
		break;
		case 11:
			DC_channel++;
		break;
		case 12:
				DC_channel++;
		break;
		case 13:
				DC_channel++;
		break;
		case 14:
				DC_channel++;
		break;
		case 15:
		vref=ad_samp_value;
		DC_channel++;
		break;

		default :
			DC_channel=0;
			break;
	}

}
float get_CP1(void)
{
	unsigned short i;
	for(i=0;i<samp_sum;i++)
	{
		if(Sys_samp.DC_samp.CP1[i]>=680)
		{
		  CP1_sum_sys+=Sys_samp.DC_samp.CP1[i];
			num_cp1++;
		}
	}
	if(num_cp1<=20)
	{
		Sys_samp.DC.CP1=0;

	}
	else
	{
		Sys_samp.DC.CP1=(CP1_sum_sys*CP1_k)/num_cp1+0.32;
	}
	CP1_sum_sys=0;
	num_cp1=0;
	return Sys_samp.DC.CP1;
}
float get_CP2(void)
{
	unsigned short i;

	for(i=0;i<samp_sum;i++)
	{
		if(Sys_samp.DC_samp.CP2[i]>=680)
		{
		CP2_sum_sys+=Sys_samp.DC_samp.CP2[i];
			num_cp2++;
		}
	}
	if(num_cp2<=20)
	{
		Sys_samp.DC.CP2=0;

	}
	else
	{
	Sys_samp.DC.CP2=(CP2_sum_sys*CP2_k)/num_cp2+0.32;
	}
			CP2_sum_sys=0;
	    num_cp2=0;
	return Sys_samp.DC.CP2;
}
void get_samp_point(void)//ÓÃÊ±30¦ÌS
{
	unsigned short i,j;
	for(i=0;i<samp_dma;i++)
		{
		CD4067_sum+=AD_samp_dma[i].CD4067;
		ia_samp_sum+=AD_samp_dma[i].ia_samp;
		va_samp_sum+=AD_samp_dma[i].va_samp;
		leakage_current_sum+=AD_samp_dma[i].leakage_current;
		CP1_sum+=AD_samp_dma[1].CP1;
	  CP2_sum+=AD_samp_dma[1].CP2;
		}
		for(j=0;j<samp_sum-1;j++)
		{
			Sys_samp.AC_samp.ia_samp[j]=Sys_samp.AC_samp.ia_samp[j+1];
			Sys_samp.AC_samp.va_samp[j]=Sys_samp.AC_samp.va_samp[j+1];
			Sys_samp.AC_samp.leakage_current_samp[j]=Sys_samp.AC_samp.leakage_current_samp[j+1];
			Sys_samp.DC_samp.CD4067[j]=Sys_samp.DC_samp.CD4067[j+1];
			Sys_samp.DC_samp.CP1[j]=Sys_samp.DC_samp.CP1[j+1];
			Sys_samp.DC_samp.CP2[j]=Sys_samp.DC_samp.CP2[j+1];
		}
  Sys_samp.AC_samp.ia_samp[samp_sum-1]=ia_samp_sum/samp_dma;
	Sys_samp.AC_samp.va_samp[samp_sum-1]=va_samp_sum/samp_dma;
	Sys_samp.AC_samp.leakage_current_samp[samp_sum-1]=leakage_current_sum/samp_dma;
	Sys_samp.DC_samp.CD4067[samp_sum-1]=CD4067_sum/samp_dma;
  Sys_samp.DC_samp.CP1[samp_sum-1]=CP1_sum/samp_dma;
	Sys_samp.DC_samp.CP2[samp_sum-1]=CP2_sum/samp_dma;
	CD4067_sum=0;
	leakage_current_sum=0;
	va_samp_sum=0;
	ia_samp_sum=0;
	CP2_sum=0;
	CP1_sum=0;
}
void Delay_ms(unsigned long long time)
{
    unsigned int x,y;
	  x=time*1000;
    for(;x>0;x--)
        for(y=180;y>0;y--);
}
void Delay_us(unsigned long long time)
{
    unsigned int y;
        for(y=190;y>0;y--);
}
void Close_gun_1(void)
{
	flag_gun_Open=1;
	if(flag_gun_Close==1)
	{
	A_KEY_ON;
	B_KEY_OFF;
	Delay_ms(600);
	A_KEY_OFF;
	flag_gun_Close=0;

	}

}
void Open_gun_1(void)
{ flag_gun_Close=1;
	if(flag_gun_Open==1)
	{
	A_KEY_OFF;
	B_KEY_ON;
	Delay_ms(600);
	B_KEY_OFF;
	flag_gun_Open=0;
	}
}
void Close_gun_2(void)
{
	A_KEY_ON;
	Delay_ms(10);
	B_KEY_ON;
	Delay_ms(200);
	A_KEY_OFF;

}
void Open_gun_2(void)
{
	A_KEY_OFF;
	Delay_ms(10);
	B_KEY_ON;
	Delay_ms(100);
	B_KEY_OFF;
}
void Gun_test(void)
{
	cp1=get_CP1();
   if(((	cp1<12.8f)&&(	cp1>11.2f))||(GET_CC1==1))
		{
			POWER_L_OFF;
	    POWER_N_OFF;
	   	PWM1_1000;
			Open_gun_1();
			//cp1=get_CP1();
		}
   else if((cp1<9.8f)&&(cp1>8.2f)&&(GET_CC1==0))
		{
	   PWM1_535;
		POWER_L_OFF;
	  POWER_N_OFF;
		//	cp1=get_CP1();
	  }
    else if((cp1<6.8f)&&(cp1>5.2f)&&(GET_CC1==0))
		{
		 PWM1_535;
		 Close_gun_1();
		 POWER_L_ON;
	   POWER_N_ON;

		}
		else
	{
	   PWM1_1000;

		 POWER_L_OFF;
	   POWER_N_OFF;

		}
}
void led_ctrl_r(uint8_t num,uint16_t state)
{
	if(num==1)
	{
	if(state==keep_on)
	{
		flag_breath_r_1=0;
		pwm_r_1=1000;
	}
	else if(state==keep_off)
	{
		flag_breath_r_1=0;
		pwm_r_1=0;
	}
	else if(state==breath)
	{
		flag_breath_r_1=1;
	}
 }
	else if(num==2)
	{

	if(state==keep_on)
	{
		flag_breath_r_2=0;
		pwm_r_2=1000;
	}
	else if(state==keep_off)
	{
		flag_breath_r_2=0;
		pwm_r_2=0;
	}
	else if(state==breath)
	{
		flag_breath_r_2=1;
	}
	}
}
void led_ctrl_g(uint8_t num,uint16_t state)
{
	if(num==1)
	{
	if(state==keep_on)
	{
		flag_breath_g_1=0;
		pwm_g_1=1000;
	}
	else if(state==keep_off)
	{
		flag_breath_g_1=0;
		pwm_g_1=0;
	}
	else if(state==breath)
	{
		flag_breath_g_1=1;
	}
 }
	else if(num==2)
	{

	if(state==keep_on)
	{
		flag_breath_g_2=0;
		pwm_g_2=1000;
	}
	else if(state==keep_off)
	{
		flag_breath_g_2=0;
		pwm_g_2=0;
	}
	else if(state==breath)
	{
		flag_breath_g_2=1;
	}
	}
}
void led_ctrl_b(uint8_t num,uint16_t state)
{
	if(num==1)
	{
	if(state==keep_on)
	{
		flag_breath_b_1=0;
		pwm_b_1=1000;
	}
	else if(state==keep_off)
	{
		flag_breath_b_1=0;
		pwm_b_1=0;
	}
	else if(state==breath)
	{
		flag_breath_b_1=1;
	}
 }
	else if(num==2)
	{

	if(state==keep_on)
	{
		flag_breath_b_2=0;
		pwm_b_2=1000;
	}
	else if(state==keep_off)
	{
		flag_breath_g_2=0;
		pwm_b_2=0;
	}
	else if(state==breath)
	{
		flag_breath_b_2=1;
	}
	}
}
void led_ctrl(uint8_t num,uint8_t colour,uint8_t state)
{
	if(colour==red)
	{
		led_ctrl_r(num,state);
	}
	else if(colour==green)
	{
		led_ctrl_g(num,state);
	}
	else if(colour==blue)
	{
		led_ctrl_b(num,state);
	}
}
void led_breath_r(void)
{
if(flag_breath_r_1==1)
		{

			if(pwm_r_flag_1==0)
			{
				pwm_r_1--;
				if(pwm_r_1<=1)
				{
					pwm_r_flag_1=1;
				}
			}
			if(pwm_r_flag_1==1)
			{
			 pwm_r_1++;
				if(pwm_r_1>=100)
				{
					pwm_r_flag_1=0;
				}
			}
		}
		if(flag_breath_r_2==1)
		{

			if(pwm_r_flag_2==0)
			{
				pwm_r_2--;
				if(pwm_r_2<=1)
				{
					pwm_r_flag_2=1;
				}
			}
			if(pwm_r_flag_2==1)
			{
			  pwm_r_2++;
				if(pwm_r_2>=100)
				{
					pwm_r_flag_2=0;
				}
			}
		}
}
void led_breath_g(void)
{
if(flag_breath_g_1==1)
		{

			if(pwm_g_flag_1==0)
			{
				pwm_g_1--;
				if(pwm_g_1<=1)
				{
					pwm_g_flag_1=1;
				}
			}
			if(pwm_g_flag_1==1)
			{
			 pwm_g_1++;
				if(pwm_g_1>=100)
				{
					pwm_g_flag_1=0;
				}
			}
		}
		if(flag_breath_g_2==1)
		{

			if(pwm_g_flag_2==0)
			{
				pwm_g_2--;
				if(pwm_g_2<=1)
				{
					pwm_g_flag_2=1;
				}
			}
			if(pwm_g_flag_2==1)
			{
			  pwm_g_2++;
				if(pwm_g_2>=100)
				{
					pwm_g_flag_2=0;
				}
			}
		}
}
void led_breath_b(void)
{
if(flag_breath_b_1==1)
		{

			if(pwm_b_flag_1==0)
			{
				pwm_b_1--;
				if(pwm_b_1<=1)
				{
					pwm_b_flag_1=1;
				}
			}
			if(pwm_b_flag_1==1)
			{
			 pwm_b_1++;
				if(pwm_b_1>=100)
				{
					pwm_b_flag_1=0;
				}
			}
		}
		if(flag_breath_b_2==1)
		{

			if(pwm_b_flag_2==0)
			{
				pwm_b_2--;
				if(pwm_b_2<=1)
				{
					pwm_b_flag_2=1;
				}
			}
			if(pwm_b_flag_2==1)
			{
			  pwm_b_2++;
				if(pwm_b_2>=100)
				{
					pwm_b_flag_2=0;
				}
			}
		}
}
void led_output(void)
{
	if(flag_breath_r_1==0)
	{
	if(pwm_r_1>=duty_ratio_r_1)
	{
		LED1_R_RUN;
	}
	else
	{
		LED1_R_OFF;
	}
 }
	if(flag_breath_g_1==1)
	{
	if(pwm_g_1>=duty_ratio_g_1)
	{
		LED1_G_RUN;
	}
	else
	{
		LED1_G_OFF;
  }
 }
if(flag_breath_b_1==1)
{
	if(pwm_b_1>=duty_ratio_b_1)
	{
		LED1_B_RUN;
	}
	else
	{
		LED1_B_OFF;
	}
}
if(flag_breath_r_2==1)
{
	if(pwm_r_2>=duty_ratio_r_2)
	{
		LED2_R_RUN;
	}
	else
	{
		LED2_R_OFF;
	}
}
if(flag_breath_g_2==1)
{
	if(pwm_g_2>=duty_ratio_g_2)
	{
		LED2_G_RUN;
	}
	else
	{
		LED2_G_OFF;
	}
}
if(flag_breath_b_2==1)
{
	if(pwm_b_2>=duty_ratio_b_2)
	{
		LED2_B_RUN;
	}
	else
	{
		LED2_B_OFF;
	}
}
}
void Power_out_pwm_ctrl(void)
{
	if(pwm_ms/2==0)
	{
		POWER_L_ON;
		POWER_N_ON;
	}
	  else
	{
		POWER_L_OFF;
		POWER_N_OFF;
	}
}
void Peripheral_Init(void)
{
	MX_GPIO_Init();
	PCA9554_init();
    MX_DMA_Init();
    MX_ADC1_Init();
	MX_TIM2_Init();//1ºÅÇ¹PWMÆµÂÊ1K
	MX_TIM3_Init();//ÏµÍ³¶¨Ê±Æ÷100uS
	MX_TIM4_Init();//2ºÅÇ¹PWMÆµÂÊ1K
	MX_TIM5_Init();//ÅäºÏA/D²ÉÑù¶¨Ê±Æ÷´¥·¢Ê±¼ä100¦ÌS
	DMA_START();
	TIMER5_ON;
	TIMER3_ON;
	PWM1_ON;
	PWM2_ON;

}
