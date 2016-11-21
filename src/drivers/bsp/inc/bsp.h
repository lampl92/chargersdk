/**
* @file bsp.h
* @brief 定义板级驱动功能开关,相应头文件引用.
* @author rgw
* @version v1.0
* @date 2016-11-01
*/
#ifndef _BSP_H_
#define _BSP_H
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public define -------------------------------------------------------------*/
#define STM32F4
//#define STM32F0
#define USE_FreeRTOS


/* NVIC PreemptPriority ------------------------------------------------------*/

#define bspUSART1_PreemptPriority       3
#define bspUSART1_SubPriority           0

#define bspUSART2_PreemptPriority       4
#define bspUSART2_SubPriority           0

#define bspLTDC_PreemptPriority         15
#define bspLTDC_SubPriority             0



/* CPU空闲时执行的函数 */
//#define CPU_IDLE()        bsp_Idle()

#ifdef USE_FreeRTOS
    #define DISABLE_INT()    taskENTER_CRITICAL()
    #define ENABLE_INT()     taskEXIT_CRITICAL()
#else
    /* 开关全局中断的宏 */
    #define ENABLE_INT()    __set_PRIMASK(0)    /* 使能全局中断 */
    #define DISABLE_INT()   __set_PRIMASK(1)    /* 禁止全局中断 */
#endif


/*
    EXTI9_5_IRQHandler 的中断服务程序分散在几个独立的 bsp文件中。
    需要整合到 stm32f4xx_it.c 中。
    定义下面行表示EXTI9_5_IRQHandler入口函数集中放到 stm32f4xx_it.c。
*/
#define EXTI9_5_ISR_MOVE_OUT

#define DEBUG_GPS_TO_COM1   /* 打印GPS数据到串口1 */

#ifndef TRUE
    #define TRUE  1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "includes.h"



/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */

#include "bsp_uart.h"
#include "bsp_sdram.h"
#include "bsp_dwt.h"
#include "bsp_rtc.h"
//#include "bsp_ltdc.h"
#include "malloc.h"

/* Public function prototypes ------------------------------------------------*/
void bsp_Init(void);
void bsp_Idle(void);
void bsp_Error_Handler(void);

/* private function prototypes -----------------------------------------------*/

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  
typedef const int16_t sc16;  
typedef const int8_t sc8;  

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;  
typedef __I int16_t vsc16; 
typedef __I int8_t vsc8;   

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  
typedef const uint16_t uc16;  
typedef const uint8_t uc8; 

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;  
typedef __I uint16_t vuc16; 
typedef __I uint8_t vuc8;
#endif

