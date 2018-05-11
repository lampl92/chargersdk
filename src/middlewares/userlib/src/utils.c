/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\userlib\src\utils.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#include "includes.h"
#include "xprintf.h"
#include "yaffsfs.h"

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
    sprintf(c, "%02X", Hex);
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
uint32_t StrToHex(const char *Str, uint8_t *Hex, int Strlen)
{
    char hexbuff[3] = {0};
    const char *src;
    int i;
    src = Str;

    for(i = 0; i < Strlen / 2; i++)
    {
        strncpy(hexbuff, src, 2);
        Hex[i] = strtol(hexbuff, NULL, 16);
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
    int fd;
    int res = 0;
    uint8_t pbuff[1024];
    uint32_t fsize;
    struct yaffs_stat st;
    uint32_t br;
    uint32_t ulCrc32 = 0xFFFFFFFF;
    uint32_t ulCrc32Table[256] = { 0 };
    int i;
    
    fd = yaffs_open(path, O_RDONLY, 0);
    if (fd < 0)
    {
        res = yaffs_get_error();
    }
    if (res != 0)
    {
        return 0;
    }
    yaffs_stat(path, &st);
    fsize = st.st_size;
    crc32_init(ulCrc32Table);
    taskENTER_CRITICAL();
    br = yaffs_read(fd, (void *)pbuff, sizeof(pbuff));
    taskEXIT_CRITICAL();
    while (br)
    {
        for (i = 0; i < br; i++)
        {
            CalcCrc32(pbuff[i], &ulCrc32, ulCrc32Table);
        }
        taskENTER_CRITICAL();
        br = yaffs_read(fd, (void *)pbuff, sizeof(pbuff));
        taskEXIT_CRITICAL();
    }
    *pulCrc32 = ~ulCrc32;

    yaffs_close(fd);
    return 1;
}

int GetBufferCrc32(uint8_t *pbuff, uint32_t size, uint32_t *pulCrc32)
{
    uint32_t ulCrc32 = 0xFFFFFFFF;
    uint32_t ulCrc32Table[256] = { 0 };
    int i;
    
    crc32_init(ulCrc32Table);
    for (i = 0; i < size; i++)
    {
        CalcCrc32(pbuff[i], &ulCrc32, ulCrc32Table);
    }
    *pulCrc32 = ~ulCrc32;

    return 1;
}
