/**
* @file bsp_uart.h
* @brief 串口驱动
* @author rgw
* @version v1.0
* @date 2016-11-01
*/
#ifndef BSP_USART_H_
#define BSP_USART_H_

#include "stm32f4xx.h"
#include "userlib_queue.h"

typedef enum
{
    UART_PORT_CLI,
    UART_PORT_GPRS,
    UART_PORT_RFID,
    UART_PORT_WIFI
} UART_Portdef;

#ifndef EVSE_DEBUG
#define CLI_USARTx_BASE                             UART4
#define CLI_USARTx_BAUDRATE                         115200
#define CLI_USARTx_IRQHandler                       void UART4_IRQHandler(void)
#define CLI_QUEUE_SIZE                              1500

#define RFID_USARTx_BASE                            USART1
#define RFID_USARTx_BAUDRATE                        115200
#define RFID_USARTx_IRQHandler                      void USART1_IRQHandler(void)
#define RFID_QUEUE_SIZE                             100

#define GPRS_USARTx_BASE                            UART5
#define GPRS_USARTx_BAUDRATE                        115200
#define GPRS_USARTx_IRQHandler                      void UART5_IRQHandler(void)
#define GPRS_QUEUE_SIZE                             (1024*1024)

#define WIFI_USARTx_BASE                            UART5
#define WIFI_USARTx_BAUDRATE                        115200
#define WIFI_USARTx_IRQHandler                      void UARTXXX_IRQHandler(void)
#define WIFI_QUEUE_SIZE                             100

#endif
#ifdef EVSE_DEBUG
#define CLI_USARTx_BASE                             USART2
#define CLI_USARTx_BAUDRATE                         115200
#define CLI_USARTx_IRQHandler                       void USART2_IRQHandler(void)
#define CLI_QUEUE_SIZE                              10

#define RFID_USARTx_BASE                            USART1
#define RFID_USARTx_BAUDRATE                        115200
#define RFID_USARTx_IRQHandler                      void USART1_IRQHandler(void)
#define RFID_QUEUE_SIZE                             100

#define GPRS_USARTx_BASE                            USART3
#define GPRS_USARTx_BAUDRATE                        115200
#define GPRS_USARTx_IRQHandler                      void USART3_IRQHandler(void)
#define GPRS_QUEUE_SIZE                             (1024*1024)

#define WIFI_USARTx_BASE                            UART5
#define WIFI_USARTx_BAUDRATE                        115200
#define WIFI_USARTx_IRQHandler                      void UARTXXX_IRQHandler(void)
#define WIFI_QUEUE_SIZE                             100

#endif

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

extern UART_HandleTypeDef CLI_UARTx_Handler;
extern UART_HandleTypeDef RFID_UARTx_Handler;
extern UART_HandleTypeDef GPRS_UARTx_Handler;
extern UART_HandleTypeDef WIFI_UARTx_Handler;

void gprs_uart_putc(uint8_t ch);
void bsp_Uart_Init(UART_Portdef uartport, uint8_t mode);
uint32_t uart_write(UART_Portdef uart, uint8_t *data, uint32_t len);
uint32_t uart_read(UART_Portdef uartport, uint8_t *data, uint32_t len, uint32_t timeout_ms);
uint32_t uart_read_ymodem(UART_Portdef uartport, uint8_t *data, uint32_t len, uint32_t timeout_ms);

#endif
