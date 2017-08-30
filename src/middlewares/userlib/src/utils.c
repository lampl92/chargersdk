/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\userlib\src\utils.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#include "includes.h"
#include "xprintf.h"

uint16_t utils_htons(uint16_t n)
{
  return ((n & 0xff) << 8) | ((n & 0xff00) >> 8);
}

uint16_t utils_ntohs(uint16_t n)
{
  return utils_htons(n);
}

uint32_t utils_htonl(uint32_t n)
{
  return ((n & 0xff) << 24) |
    ((n & 0xff00) << 8) |
    ((n & 0xff0000UL) >> 8) |
    ((n & 0xff000000UL) >> 24);
}

uint32_t utils_ntohl(uint32_t n)
{
  return utils_htonl(n);
}

uint32_t StrToBCD(const char *Str, char *Des, int iDesLen)
{
    if (NULL == Str)
    {
        return 0;
    }
    if (NULL == Des)
    {
        return 0;
    }
    if (0 == iDesLen)
    {
        return 0;
    }

    int iStrLen = strlen(Str);
    if (iStrLen > iDesLen * 2)
    {
        return 0;
    }
    char chTemp = 0;
    int i;
    for (i = 0; i < iStrLen; i++)
    {
        if (i % 2 == 0)
        {
            chTemp = (Str[i] << 4) & 0xF0;
        }
        else
        {
            chTemp = chTemp & 0xF0 | (Str[i] & 0x0F);
            Des[i / 2] = chTemp;
        }
    }
    if (i % 2 != 0)
    {
        Des[i / 2] = chTemp;
    }

    return 1;
}


uint32_t BCDToStr(const char *Src, char *Des, int iSrcLen)
{
    if (NULL == Src)
    {
        return 0;
    }
    if (NULL == Src)
    {
        return 0;
    }

    char chTemp = 0;
    char chDes = 0;
    for (int i = 0; i < iSrcLen * 2; i++)
    {
        chTemp = Src[i];
        chDes = chTemp >> 4;
        Des[i * 2] = chDes + '0';
        chDes = (chTemp & 0x0F) + '0';
        Des[i * 2 + 1] = chDes;
    }
    Des[iSrcLen * 2] = '\0';
    return 1;
}

uint32_t HexToChar(uint8_t Hex, uint8_t *c)
{
    xsprintf(c, "%02X", Hex);
}

uint32_t HexToStr(uint8_t *Hex, uint8_t *Str, int Hexlen)
{
    int i;
    for(i = 0; i < Hexlen; i++)
    {
        HexToChar(Hex[i], &(Str[i * 2]));
    }
    Str[i * 2] = '\0';
}

uint32_t StrToHex(uint8_t *Str, uint8_t *Hex, int Strlen)
{
    uint8_t hexbuff[2];
    uint8_t *src;
    int i;
    src = Str;

    for(i = 0; i < Strlen / 2; i++)
    {
        strncpy((char *)hexbuff, src, 2);
//        if(hexbuff[0] > 'A' || hexbuff[1] > 'A')
//        {
            Hex[i] = strtol(hexbuff, NULL, 16);
//        }
//        else
//        {
//            Hex[i] = strtol(hexbuff, NULL, 10);
//        }
        src += 2;
    }
}

int utils_abs(int num)
{
	if(num<0)
		return -num;
	else
		return num;
}
