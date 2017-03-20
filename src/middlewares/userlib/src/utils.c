/**
* @file D:\Documents\Projects\chargersdk\src\middlewares\userlib\src\utils.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#include "includes.h"

uint32_t StrToBCD(const char *Src, char *Des, int iDesLen)

{
    if (NULL == Src)
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

    int iSrcLen = strlen(Src);
    if (iSrcLen > iDesLen * 2)
    {
        return 0;
    }
    char chTemp = 0;
    int i;
    for (i = 0; i < iSrcLen; i++)
    {
        if (i % 2 == 0)
        {
            chTemp = (Src[i] << 4) & 0xF0;
        }
        else
        {
            chTemp = chTemp & 0xF0 | (Src[i] & 0x0F);
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
    Des[iSrcLen*2] = '\0';
    return 1;
}
