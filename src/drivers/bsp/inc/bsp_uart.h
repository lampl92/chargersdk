/**
* @file bsp_uart.h
* @brief ����Ƭ�ϴ���
* @author rgw
* @version v1.0
* @date 2016-11-01
*/

#ifndef BSP_USART_H_
#define BSP_USART_H_

#include "bsp.h"

#define cliRXBUFFERSIZE   1


/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void bsp_Uart_Init(void);


#endif

