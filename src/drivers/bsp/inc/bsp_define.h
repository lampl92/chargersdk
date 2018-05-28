#ifndef _BSP_DEFINE_H
#define _BSP_DEFINE_H

#include "stm32f4xx.h"

/* NVIC PreemptPriority ------------------------------------------------------*/

#define bspUSART1_PreemptPriority       5
#define bspUSART1_SubPriority           0

#define bspUSART2_PreemptPriority       5
#define bspUSART2_SubPriority           0

#define bspUSART3_PreemptPriority       5
#define bspUSART3_SubPriority           0

#define bspUART4_PreemptPriority       5
#define bspUART4_SubPriority           0

#define bspUART5_PreemptPriority       5
#define bspUART5_SubPriority           0

#define bspUSART6_PreemptPriority       5
#define bspUSART6_SubPriority           0

#define bspUART7_PreemptPriority       5
#define bspUART7_SubPriority           0

#define bspLTDC_PreemptPriority         7
#define bspLTDC_SubPriority             0

#define bspWKUP_PreemptPriority         3
#define bspWKUP_SubPriority             0

#define bspAlarm_PreemptPriority        3
#define bspAlarm_SubPriority            0

#define bspDMA2_Stream0_PreemptPriority        1
#define bspDMA2_Stream0_SubPriority            0

#define bspTIM2_PreemptPriority        1
#define bspTIM2_SubPriority            0

#define bspTIM3_PreemptPriority        1
#define bspTIM3_SubPriority            0

#define bspTIM4_PreemptPriority        1
#define bspTIM4_SubPriority            0

#define bspTIM8_PreemptPriority        1
#define bspTIM8_SubPriority            0

#define bspTIM5_PreemptPriority        1
#define bspTIM5_SubPriority            0

#if USE_FreeRTOS
#define DISABLE_INT()    taskENTER_CRITICAL()
#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
/* 开关全局中断的宏 */
#define ENABLE_INT()    __set_PRIMASK(0)    /* 使能全局中断 */
#define DISABLE_INT()   __set_PRIMASK(1)    /* 禁止全局中断 */
#endif

#endif
