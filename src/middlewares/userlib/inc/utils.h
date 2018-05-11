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

char *utils_strdup(const char *s);

uint16_t utils_htons(uint16_t n);
uint16_t utils_ntohs(uint16_t n);
uint32_t utils_htonl(uint32_t n);
uint32_t utils_ntohl(uint32_t n);

#define htons(x) utils_htons(x)
#define ntohs(x) utils_ntohs(x)
#define htonl(x) utils_htonl(x)
#define ntohl(x) utils_ntohl(x)

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
uint32_t StrToHex(const char *Str, uint8_t *Hex, int Strlen);
int utils_abs(int num);

int GetFileCrc32(uint8_t *path, uint32_t *pulCrc32);
int GetBufferCrc32(uint8_t *pbuff, uint32_t size, uint32_t *pulCrc32);
#endif
