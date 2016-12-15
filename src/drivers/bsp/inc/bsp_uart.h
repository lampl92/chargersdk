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
#define CLI_USARTx_IRQHandler                       void USART2_IRQHandler(void)
#define CLI_BUFFER_SIZE                             10

#define RFID_USARTx_BASE                            USART1
#define RFID_USARTx_BAUDRATE                        115200
#define RFID_USARTx_IRQHandler                      void USART1_IRQHandler(void)
#define RFID_BUFFER_SIZE                            100


typedef struct
{
    uint8_t *front;
    uint8_t *rear;
}uart_recv_t;

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))


void bsp_Uart_Init(void);


#endif

