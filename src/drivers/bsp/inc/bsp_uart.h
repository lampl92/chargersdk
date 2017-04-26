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
#include "stm32f4xx.h"
#include "userlib_queue.h"

#ifdef EVSE
#define CLI_USARTx_BASE                             UART4
#define CLI_USARTx_BAUDRATE                         9600
#define CLI_USARTx_IRQHandler                       void UART4_IRQHandler(void)
#define CLI_QUEUE_SIZE                              10
#endif
#ifdef EVSE_DEV
#define CLI_USARTx_BASE                             USART2
#define CLI_USARTx_BAUDRATE                         115200
#define CLI_USARTx_IRQHandler                       void USART2_IRQHandler(void)
#define CLI_QUEUE_SIZE                              10

#define RFID_USARTx_BASE                            USART3
#define RFID_USARTx_BAUDRATE                        115200
#define RFID_USARTx_IRQHandler                      void USART3_IRQHandler(void)
#define RFID_QUEUE_SIZE                             100

#define GPRS_USARTx_BASE                            USART1
#define GPRS_USARTx_BAUDRATE                        115200
#define GPRS_USARTx_IRQHandler                      void USART1_IRQHandler(void)
#define GPRS_QUEUE_SIZE                             1500
#endif

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

extern UART_HandleTypeDef CLI_UARTx_Handler;
extern UART_HandleTypeDef RFID_UARTx_Handler;
extern UART_HandleTypeDef GPRS_UARTx_Handler;

extern Queue *pCliRecvQue;
extern Queue *pRfidRecvQue;
extern Queue *pGprsRecvQue;

void bsp_Uart_Init(void);
uint8_t readRecvQue(Queue *q, uint8_t *ch, uint16_t time_out);
uint8_t recvReadEx(Queue *q, uint8_t *pbuff, uint32_t ulRecvLen, uint32_t *puiRecvdLen);

#endif

