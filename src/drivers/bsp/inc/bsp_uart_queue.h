/**
* @file bsp_uart_queue.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#ifndef  __BSP_UART_QUEUE_H
#define  __BSP_UART_QUEUE_H

#include "gdsl_queue.h"

extern gdsl_queue_t queGPRS;
extern gdsl_queue_t queCLI;

void uart_queue_init(void);
#endif
