#ifndef _TIMER_H
#define _TIMER_H
#include "bsp.h"
#include "stm32f4xx_hal_tim.h"
uint16_t pwm,pwm4,timer_ms,timer_s,delay_breath,timer_relay_ms,timer_min;
uint16_t delay_ms,pwm_ms,pwm_r_1,pwm_r_2,pwm_g_1,pwm_g_2,
            pwm_b_1,pwm_b_2;
uint8_t flag_power_pwm_on,flag_pwm_r_on_1,flag_pwm_r_on_2,flag_pwm_g_on_1,
        flag_pwm_g_on_2,flag_pwm_b_on_1,flag_pwm_b_on_2;
uint8_t yy_test;
uint8_t pwm_r_flag_1,pwm_r_flag_2,pwm_g_flag_1,pwm_g_flag_2,pwm_b_flag_1,pwm_b_flag_2;
TIM_HandleTypeDef TIM3_Handler;
TIM_HandleTypeDef TIM3_Handler;      //¶¨Ê±Æ÷¾ä±ú
TIM_HandleTypeDef TIM2_Handler;         //¶¨Ê±Æ÷3PWM¾ä±ú
TIM_OC_InitTypeDef TIM2_CH1Handler;	    //¶¨Ê±Æ÷3Í¨µÀ4¾ä±ú
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

 void MX_TIM3_Init(void);
 void MX_TIM2_Init(void);
 void MX_TIM1_Init(void);
 void MX_TIM6_Init(void);
 void MX_TIM7_Init(void);
 void MX_TIM4_Init(void);
 void MX_TIM5_Init(void);

#endif

