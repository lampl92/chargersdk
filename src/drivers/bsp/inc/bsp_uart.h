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

#define     CLI_USE_USART2

#ifdef CLI_USE_USART1
#define CLI_USARTx_BASE                     USART1
#define CLI_USARTx_IRQHandler_PROTOTYPE     void USART1_IRQHandler(void)
#define CLI_UARTx_Handler                   UART1_Handler
#endif

#ifdef CLI_USE_USART2
#define CLI_USARTx_BASE                     USART2
#define CLI_USARTx_IRQHandler_PROTOTYPE     void USART2_IRQHandler(void)
#define CLI_UARTx_Handler                   UART2_Handler
#endif

#define cliRXBUFFERSIZE   1


/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void bsp_Uart_Init(void);


#endif

