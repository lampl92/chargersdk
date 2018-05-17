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
#include "bsp_define.h"

///////////////////////////////////////////
#define USART1_RB_SIZE 128
#define USART2_RB_SIZE 1024
#define USART3_RB_SIZE 1024
#define UART4_RB_SIZE 1024
#define UART5_RB_SIZE 1024 * 10
#define USART6_RB_SIZE 1024
#define UART7_RB_SIZE 1024

///////////////////////////////////////////
#define UART_PATH_ERR       -1
#define UART_IS_INITIALIZED -2
#define UART_RB_INIT_FAIL   -3

///////////////////////////////////////////
void uart_driver_init(void);
int uart_open(char *path, uint32_t bps);
int uart_close(int handle);
uint32_t uart_read_fast(int handle, uint8_t *data, uint32_t len);
uint32_t uart_read_wait(int handle, uint8_t *data, uint32_t len, uint32_t timeout_ms);
uint32_t uart_write_fast(int handle, uint8_t *data, uint32_t len);
#endif
