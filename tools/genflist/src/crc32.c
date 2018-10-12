#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
int GetFileCrc32(FILE *fp, uint32_t *pulCrc32)
{
    uint8_t pbuff[1024];
    uint32_t br;
    uint32_t ulCrc32 = 0xFFFFFFFF;
    uint32_t ulCrc32Table[256] = { 0 };

    crc32_init(ulCrc32Table);
    fseek(fp, 0, SEEK_SET);
    br = fread((void *)pbuff, 1, sizeof(pbuff), fp);
    while (br)
    {
        for (int i = 0; i < br; i++)
        {
            CalcCrc32(pbuff[i], &ulCrc32, ulCrc32Table);
        }
        br = fread((void *)pbuff, 1, sizeof(pbuff), fp);
    }
    *pulCrc32 = ~ulCrc32;

    return 0;
}
