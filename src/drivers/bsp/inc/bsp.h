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


#define USE_UART1
#define USE_UART2


/* NVIC PreemptPriority ------------------------------------------------------*/
#ifdef USE_UART1
#define bspUSART1_PreemptPriority       3
#define bspUSART1_SubPriority           0
#endif
#ifdef USE_UART2
#define bspUSART2_PreemptPriority       4
#define bspUSART2_SubPriority           0
#endif



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

/* 这个宏仅用于调试阶段排错 */
#define BSP_Printf      printf
//#define BSP_Printf(...)

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

/* Public function prototypes ------------------------------------------------*/
void bsp_Init(void);
void bsp_Idle(void);
void bsp_Error_Handler(void);

/* private function prototypes -----------------------------------------------*/
#endif

