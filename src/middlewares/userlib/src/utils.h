/**
* @file utils.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-24
*/
#ifndef  __UTILS_H
#define  __UTILS_H

#include "stm32f4xx.h"

typedef union
{
	uint32_t ulVal;
	uint8_t ucVal[4];
}ul2uc;

typedef union
{
	uint16_t usVal;
	uint8_t ucVal[2];
}us2uc;
uint32_t HexToStr(uint8_t *Hex, uint8_t *Src, int Hexlen);
uint32_t StrToHex(uint8_t *Str, uint8_t *Hex, int Strlen);
int utils_abs(int num);

#endif
