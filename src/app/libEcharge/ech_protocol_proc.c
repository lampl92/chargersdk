/**
* @file ech_protocol_proc.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-21
*/
#include "gdsl_list.h"
#include "libEcharge/ech_protocol_proc.h"
#include "libEcharge/ech_err.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "evse_globals.h"
#include "task_tcp_client.h"
#include "taskcreate.h"
#include "lwip_init.h"

void vTaskRemoteCmdProc(void *pvParameters)
{
    echProtocol_t *pProto;
    echCmdElem_t *pechCmdElem;
    gdsl_list_cursor_t cs;
    gdsl_list_cursor_t cr;

    uint32_t ulSendCmdCount;
    uint32_t ulRecvCmdCount;
    EventBits_t uxBitsTCP;
    int res;
    int i;

    pProto = (echProtocol_t *)pvParameters;
    cs = gdsl_list_cursor_alloc (pProto->plechSendCmd);
    cr = gdsl_list_cursor_alloc (pProto->plechRecvCmd);
    while(1)
    {
//        printf_safe("send elem = %d\n", gdsl_list_get_size(pProto->plechSendCmd));
//        printf_safe("recv elem = %d\n", gdsl_list_get_size(pProto->plechRecvCmd));
        printf_safe("\n");

        /** @todo (rgw#1#): ������ ��Ҫ��ʱ������һ������ĳ�ʱ���ط��������߼��������� */

        /* ����SendCmd */

        gdsl_list_cursor_move_to_head (cs);
        while(pechCmdElem = gdsl_list_cursor_get_content (cs))
        {
            /* 1. �ж�Э���Ƿ��� */
            if(pechCmdElem->status == 0)
            {
                memmove(tcp_client_sendbuf, pechCmdElem->pbuff, pechCmdElem->len);
                send_len = pechCmdElem->len;
                uxBitsTCP = xEventGroupSync(xHandleEventTCP,
                                             defEventBitTCPClientSendReq,
                                             defEventBitTCPClientSendOK,
                                             10000);
                if((uxBitsTCP & defEventBitTCPClientSendOK) == defEventBitTCPClientSendOK)
                {
                    pechCmdElem->status = 1;
                }
                else//û����ȥ
                {
                    pechCmdElem->trycount++;
                    if(pechCmdElem->trycount > pechCmdElem->trycountmax)
                    {
                        gdsl_list_cursor_delete(cs);

                        continue;
                        /* @todo (rgw#1#): ֪ͨϵͳ���緢�����⣬��Ҫ����gprs */
                    }
                }
            }

            if(pechCmdElem->status == 1)
            {
                /* �ж������֣�
                   ��������������ȴ������ظ�
                   ����ǻظ������ɾ��
                */
                if(pProto->pCMD[pechCmdElem->cmd_id]->uiRecvdOptLen > 0) //���������յ��ظ�
                {
                    gdsl_list_cursor_delete(cs);
                    continue;
                }

            }
#if 1
            /* 2. �жϳ�ʱ ��ɾ������δ�յ��ظ�������*/
            if((time(NULL) - pechCmdElem->timestamp) > pechCmdElem->timeout_s)
            {
                gdsl_list_cursor_delete(cs);
                continue;
            }
#endif
            vTaskDelay(1000);
            /* 3. */
            gdsl_list_cursor_step_forward (cs);
        }

        /* ����RecvCmd */

        gdsl_list_cursor_move_to_head (cr);
        while(pechCmdElem = gdsl_list_cursor_get_content (cr))
        {
            if(pechCmdElem->status == 0)
            {
                res = pProto->pCMD[pechCmdElem->cmd_id]->analyProc(pProto,
                        pechCmdElem->cmd_id,
                        pechCmdElem->pbuff,
                        pechCmdElem->len);
                if(res == 1)
                {
                    pechCmdElem->status = 1;
                }
                else//���յ�Э��֡���������⣬ֱ��ɾ��
                {
                    gdsl_list_cursor_delete(cr);
                    continue;
                }
            }

            if(pechCmdElem->status == 1)
            {
                gdsl_list_cursor_delete(cr);
                continue;
            }
#if 1
            /* 2. �жϳ�ʱ ��ɾ������δ�յ��ظ�������*/
            if((time(NULL) - pechCmdElem->timestamp) > pechCmdElem->timeout_s)
            {
                gdsl_list_cursor_delete(cr);
                continue;
            }
#endif
            vTaskDelay(1000);
            gdsl_list_cursor_step_forward (cr);
        }
        vTaskDelay(1000);
    }
}
