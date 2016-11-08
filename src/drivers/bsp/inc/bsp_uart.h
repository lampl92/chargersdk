/**
* @file bsp_uart.h
* @brief ����Ƭ�ϴ���
* @author rgw
* @version v1.0
* @date 2016-11-01
*/

#ifndef _BSP_USART_H_
#define _BSP_USART_H_

#include "bsp.h"

#define uart2RXBUFFERSIZE   1


/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void bsp_InitUart(void);


#endif

