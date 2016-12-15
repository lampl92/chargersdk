/**
* @file bsp_uart.h
* @brief 定义片上串口
* @author rgw
* @version v1.0
* @date 2016-11-01
*/

#ifndef BSP_USART_H_
#define BSP_USART_H_

#include "bsp.h"



#define CLI_USARTx_BASE                             USART2
#define CLI_USARTx_BAUDRATE                         115200
#define CLI_USARTx_IRQHandler(USARTx)               void ##USARTx##_IRQHandler(void)









#define cliRXBUFFERSIZE   1


/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void bsp_Uart_Init(void);


#endif

