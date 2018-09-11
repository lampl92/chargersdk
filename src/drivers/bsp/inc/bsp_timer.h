#ifndef _TIMER_H
#define _TIMER_H
#include "bsp.h"
#include "stm32f4xx_hal_tim.h"

extern uint16_t timer_relay_ms;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim8;

void MX_TIM3_Init(void);
void MX_TIM2_Init(void);
void MX_TIM1_Init(void);
void MX_TIM6_Init(void);
void MX_TIM7_Init(void);
void MX_TIM4_Init(void);
void MX_TIM5_Init(void);
void MX_TIM8_Init(void);
void TIM_SetTIM2Compare1(unsigned int compare);
void TIM_SetTIM2Compare4(unsigned int compare);
void TIM_SetTIM3Compare1(unsigned int compare);
void TIM_SetTIM8Compare1(unsigned int compare);
#endif
