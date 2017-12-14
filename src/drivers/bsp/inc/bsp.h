/**
* @file bsp.h
* @brief 定义板级驱动功能开关,相应头文件引用.
* @author rgw
* @version v1.0
* @date 2016-11-01
*/
#ifndef _BSP_H
#define _BSP_H

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "errorcode.h"

/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_gpio.h"
#include "bsp_uart.h"
#include "bsp_sdram.h"
#include "bsp_dwt.h"
#include "bsp_rtc.h"
//#include "bsp_ltdc.h"
#include "bsp_nand_flash.h"
#include "bsp_user_lib.h"
#include "lcddrv.h"
#include "touch.h"
#include "user_app.h"
#include "iwdg.h"

/* Public function prototypes ------------------------------------------------*/
void bsp_Init(void);
void bsp_Idle(void);
void bsp_Error_Handler(void);

int printf_safe(const char *format, ...);
void bsp_Error_Handler(void);

#endif

