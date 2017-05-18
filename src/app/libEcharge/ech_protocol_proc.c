/**
* @file ech_protocol_proc.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-21
*/
#include "gdsl_list.h"
#include "ech_protocol_proc.h"
#include "libEcharge/ech_err.h"


/** @brief 复制待插入的元素所有内容到新申请的空间
 *
 * @param pechCmd void*
 * @return gdsl_element_t
 *
 */
static gdsl_element_t echCmdListAlloc(gdsl_element_t pechCmd)
{
    echCmdElem_t* copyCmdListElem;

    copyCmdListElem = (echCmdElem_t *)malloc(sizeof(echCmdElem_t));
    if(copyCmdListElem != NULL)
    {
        memcpy(copyCmdListElem, pechCmd, sizeof(echCmdElem_t));
    }
    else
    {
        return NULL;
    }
    copyCmdListElem->pbuff = (uint8_t *)malloc(copyCmdListElem->len * sizeof(uint8_t));
    if(copyCmdListElem->pbuff != NULL)
    {
        memcpy(copyCmdListElem->pbuff, ((echCmdElem_t *)pechCmd)->pbuff, ((echCmdElem_t *)pechCmd)->len);
    }
    else
    {
        return NULL;
    }

    return (gdsl_element_t)copyCmdListElem;
}

static void echCmdListFree (gdsl_element_t e)
{
    free(((echCmdElem_t *)e)->pbuff);
    free (e);
    e = NULL;
}

void echCmdProcess(void)
{
    echCmdElem_t *pechCmdElem;
    uint32_t ulSendCmdCount;
    uint32_t ulRecvCmdCount;
    int i;

    ulSendCmdCount = gdsl_list_get_size(pechProto->plechSendCmd);
    ulRecvCmdCount = gdsl_list_get_size(pechProto->plechRecvCmd);

    for(i = 0; i < ulSendCmdCount; i++)
    {
        pechCmdElem = gdsl_list_search_by_position(pechProto->plechSendCmd, i);
        /* 1. 判断协议是否需要发送 */
        if(pechCmdElem ->status == 0)
        {

        }
    }
}
