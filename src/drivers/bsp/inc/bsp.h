/**
* @file bsp.h
* @brief ����弶�������ܿ���,��Ӧͷ�ļ�����.
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



/* CPU����ʱִ�еĺ��� */
//#define CPU_IDLE()        bsp_Idle()

#ifdef USE_FreeRTOS
    #define DISABLE_INT()    taskENTER_CRITICAL()
    #define ENABLE_INT()     taskEXIT_CRITICAL()
#else
    /* ����ȫ���жϵĺ� */
    #define ENABLE_INT()    __set_PRIMASK(0)    /* ʹ��ȫ���ж� */
    #define DISABLE_INT()   __set_PRIMASK(1)    /* ��ֹȫ���ж� */
#endif

/* ���������ڵ��Խ׶��Ŵ� */
#define BSP_Printf      printf
//#define BSP_Printf(...)

/*
    EXTI9_5_IRQHandler ���жϷ�������ɢ�ڼ��������� bsp�ļ��С�
    ��Ҫ���ϵ� stm32f4xx_it.c �С�
    ���������б�ʾEXTI9_5_IRQHandler��ں������зŵ� stm32f4xx_it.c��
*/
#define EXTI9_5_ISR_MOVE_OUT

#define DEBUG_GPS_TO_COM1   /* ��ӡGPS���ݵ�����1 */

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



/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "bsp_uart.h"

/* Public function prototypes ------------------------------------------------*/
void bsp_Init(void);
void bsp_Idle(void);
void bsp_Error_Handler(void);

/* private function prototypes -----------------------------------------------*/
#endif

