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


/** @brief ���ƴ������Ԫ���������ݵ�������Ŀռ�
 *
 * @param pechCmd void*
 * @return gdsl_element_t
 *
 */
static gdsl_element_t echCmdListAlloc(gdsl_element_t pechCmd)
{
    echCmdElem_t *copyCmdListElem;

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

void vTaskRemoteCmdProc(void)
{
    echCmdElem_t *pechCmdElem;
    gdsl_list_cursor_t c;

    uint32_t ulSendCmdCount;
    uint32_t ulRecvCmdCount;
    EventBits_t uxBitsLwip;
    int i;

    while(1)
    {
        /* ����SendCmd */
        c = gdsl_list_cursor_alloc (pechProto->plechSendCmd);
        for(gdsl_list_cursor_move_to_head (c); pechCmdElem = gdsl_list_cursor_get_content (c); gdsl_list_cursor_step_forward (c))
        {
            /* 1. �ж�Э���Ƿ���Ҫ���� */
            if(pechCmdElem ->status == 0)
            {
                memcpy(tcp_client_sendbuf, pechCmdElem->pbuff, pechCmdElem->len);
                send_len = pechCmdElem->len;
                uxBitsLwip = xEventGroupSync(xHandleEventLwIP,
                                             defEventBitTCPClientSendReq,
                                             defEventBitTCPClientSendOK,
                                             0);
                if((uxBitsLwip & defEventBitTCPClientSendOK) == defEventBitTCPClientSendOK)
                {
                    pechCmdElem->status = 1;
                }
                else//û����ȥ
                {

                }
            }

            /* 2. �жϳ�ʱ */
            if((time(NULL) - pechCmdElem->timestamp) > pechCmdElem->timeout)
            {
                gdsl_list_cursor_delete(c);
                gdsl_list_cursor_step_backward(c);//ɾ��֮������Զ�forwardһ����forѭ����forward�ͻ���һ��Ԫ�أ���������һ����
                continue;
            }

            /* 3. */
        }
        gdsl_list_free(c);

        /* ����RecvCmd */
        c = gdsl_list_cursor_alloc (pechProto->plechRecvCmd);
        for(gdsl_list_cursor_move_to_head (c); pechCmdElem = gdsl_list_cursor_get_content (c); gdsl_list_cursor_step_forward (c))
        {
        }
        gdsl_list_free(c);
    }
}
