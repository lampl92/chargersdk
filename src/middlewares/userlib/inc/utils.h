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

#define utils_min(a, b) (((a) < (b)) ? (a) : (b))

char *utils_strdup(const char *s);

uint16_t utils_htons(uint16_t n);
uint16_t utils_ntohs(uint16_t n);
uint32_t utils_htonl(uint32_t n);
uint32_t utils_ntohl(uint32_t n);

#ifndef htons
#define htons(x) utils_htons(x)
#endif

#ifndef ntohs
#define ntohs(x) utils_ntohs(x)
#endif

#ifndef htonl
#define htonl(x) utils_htonl(x)
#endif

#ifndef ntohl
#define ntohl(x) utils_ntohl(x)
#endif

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
uint32_t HexToStr(uint8_t *Hex, char *Src, int Hexlen);
uint32_t StrToHex(const char *Str, uint8_t *Hex, int Strlen);
int utils_abs(int num);

int GetFileCrc32(char *path, uint32_t *pulCrc32);
int GetBufferCrc32(uint8_t *pbuff, uint32_t size, uint32_t *pulCrc32);
uint32_t StrCrc32ToUint32(char *strCrc32);
#endif
