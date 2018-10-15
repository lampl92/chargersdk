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
uint64_t utils_htonll(uint64_t n);
uint64_t utils_ntohll(uint64_t n);
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

#ifndef htonll
#define htonll(x) utils_htonll(x)
#endif

#ifndef ntohll
#define ntohll(x) utils_ntohll(x)
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

typedef union
{
    uint64_t ullVal;
    uint8_t ucArray[8];
}ull2uc;
uint32_t HexToStr(uint8_t *Hex, char *Src, int Hexlen);
uint32_t StrToHex(const char *Str, uint8_t *Hex, int Strlen);
int utils_abs(int num);

void crc32_init(uint32_t *pulCrc32Table);
void CalcCrc32(const uint8_t byte, uint32_t *pulCrc32, uint32_t *pulCrc32Table);
uint32_t StrCrc32ToUint32(char *strCrc32);
void *utils_memfrob(void *s, size_t n);
#endif
