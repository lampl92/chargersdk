/**
* @file bsp_uart.h
* @brief ´®¿ÚÇý¶¯
* @author rgw
* @version v1.0
* @date 2016-11-01
*/

#ifndef BSP_USART_H_
#define BSP_USART_H_

#include "bsp.h"
#include "userlib_queue.h"

#define CLI_USARTx_BASE                             USART1
#define CLI_USARTx_BAUDRATE                         115200
#define CLI_USARTx_IRQHandler                       void USART1_IRQHandler(void)
#define CLI_BUFFER_SIZE                             10

#define RFID_USARTx_BASE                            USART2
#define RFID_USARTx_BAUDRATE                        115200
#define RFID_USARTx_IRQHandler                      void USART2_IRQHandler(void)
#define RFID_BUFFER_SIZE                            100


/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void bsp_Uart_Init(void);
int8_t readRecvQue(Queue *q, uint8_t *ch, uint16_t time_out);

#endif

