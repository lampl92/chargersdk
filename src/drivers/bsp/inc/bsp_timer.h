#ifndef _TIMER_H
#define _TIMER_H
#include "bsp.h"
#include "stm32f4xx_hal_tim.h"

extern TIM_HandleTypeDef TIM3_Handler;
extern void MX_TIM6_Init(void);
extern void MX_TIM7_Init(void);
extern void TIM3_Init(unsigned int  arr,unsigned int psc);
extern void MX_TIM4_Init(void);
extern void MX_TIM5_Init(void);
#endif

