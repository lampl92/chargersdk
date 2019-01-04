#ifndef _SPI_GPIO_H
#define _SPI_GPIO_H
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	


#define ADE_MOSI_H           HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET)
#define ADE_MOSI_L           HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET)

#define ADE_SCLK_H           HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)
#define ADE_SCLK_L           HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)

#define ADE_SPI_ENABLE       HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_RESET)
#define ADE_SPI_DISABLE      HAL_GPIO_WritePin(GPIOH,GPIO_PIN_5,GPIO_PIN_SET)

#define ADE_SPI_RST_DISABLE  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_SET)
#define ADE_SPI_RST_ENABLE   HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_RESET)

#define ADE_MISO             HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5)

void Init_ADE7953_SPI(void);
void Write_ADE7953_SPI(unsigned int ADE_Addr, unsigned char Nr_Bytes,unsigned char *pReg_Data);
void Read_ADE7953_SPI(unsigned int ADE_Addr, unsigned char Nr_Bytes,unsigned char *pReg_Data);

#endif
