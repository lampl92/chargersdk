/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\userlib\src\utils.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#include "includes.h"
#include "xprintf.h"

char *utils_strdup(const char *s)
{
    size_t len = strlen(s) + 1;//计算字符串的长度
    void *new = malloc(len);//分配一个新的空间给new
    if(new == NULL)
    {
        return NULL;
    }
    return (char *)memcpy(new, s, len);//拷贝s数据到new中
}

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

/** @brief "1234567890123456"  --> bcd[0]= 0x12  bcd[1]= 0x34 ......
 *
 * @param Str const char*
 * @param Des char*
 * @param iDesLen int
 * @return uint32_t
 *
 */
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

#if 0 //这个测试有问题,先不用
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
#endif

uint32_t HexToChar(uint8_t Hex, uint8_t *c)
{
    xsprintf(c, "%02X", Hex);
}

/** @brief hex[0] = 0xE1, hex[1] = 0xFF,  hex[2] = 0x99  --> "E1FF99000..."
 *
 * @param Hex uint8_t*
 * @param Str uint8_t*
 * @param Hexlen int
 * @return uint32_t
 *
 */
uint32_t HexToStr(uint8_t *Hex, uint8_t *Str, int Hexlen)
{
    int i;
    for(i = 0; i < Hexlen; i++)
    {
        HexToChar(Hex[i], &(Str[i * 2]));
    }
    Str[i * 2] = '\0';
}

/** @brief "E1FF991234567890" --> hex[0] = 0xE1, hex[1] = 0xFF,  hex[2] = 0x99 ......
 *
 * @param Str uint8_t*
 * @param Hex uint8_t*
 * @param Strlen int
 * @return uint32_t
 *
 */
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
    if(num < 0)
    {
        return -num;
    }
    else
    {
        return num;
    }
}

static void crc32_init(uint32_t *pulCrc32Table)
{
    // This is the official polynomial used by CRC32 in PKZip.
	// Often times the polynomial shown reversed as 0x04C11DB7.
    //uint32_t ulPolynomial = 0xEDB88320;
    uint32_t ulPolynomial = 0xEDB88320;
    int i, j;

    uint32_t ulCrc;
    for (i = 0; i < 256; i++)
    {
        ulCrc = i;
        for (j = 8; j > 0; j--)
        {
            if (ulCrc & 1)
                ulCrc = (ulCrc >> 1) ^ ulPolynomial;
            else
                ulCrc >>= 1;
        }
        pulCrc32Table[i] = ulCrc;
    }
}

static void CalcCrc32(const uint8_t byte, uint32_t *pulCrc32, uint32_t *pulCrc32Table)
{
    *pulCrc32 = ((*pulCrc32) >> 8) ^ pulCrc32Table[(byte) ^ ((*pulCrc32) & 0x000000FF)];
}

int GetFileCrc32(uint8_t *path, uint32_t *pulCrc32)
{
    FIL f;
    FRESULT res;
    uint8_t pbuff[1024];
    UINT fsize;
    UINT br;
    uint32_t ulCrc32 = 0xFFFFFFFF;
    uint32_t ulCrc32Table[256] = { 0 };
    int i;
    
    res = f_open(&f, path, FA_OPEN_EXISTING | FA_READ);
    if (res != FR_OK)
    {
        return 0;
    }
    fsize = f_size(&f);
    crc32_init(ulCrc32Table);
    res = f_read(&f, (void *)pbuff, sizeof(pbuff), &br);
    while (br)
    {
        for (i = 0; i < br; i++)
        {
            CalcCrc32(pbuff[i], &ulCrc32, ulCrc32Table);
        }
        res = f_read(&f, (void *)pbuff, sizeof(pbuff), &br);
    }
    *pulCrc32 = ~ulCrc32;

    f_close(&f);
    return 1;
}
