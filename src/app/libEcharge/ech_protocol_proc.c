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
    echProtoElem_t *pechProtoElem;
    echCmdElem_t *pechCmdElem;
    gdsl_list_cursor_t cs;
    gdsl_list_cursor_t cr;
    gdsl_list_cursor_t ccmd;

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
//        printf_safe("\n");

        /* ����RecvCmd */

        gdsl_list_cursor_move_to_head (cr);
        while(pechProtoElem = gdsl_list_cursor_get_content (cr))
        {
            if(pechProtoElem->status == 0)
            {
                res = pProto->pCMD[pechProtoElem->cmd_id]->analyProc(pProto,
                        pechProtoElem->cmd_id,
                        pechProtoElem->pbuff,
                        pechProtoElem->len);
                if(res == 1)
                {
                    pechProtoElem->status = 1;
                }
                else//���յ�Э��֡���������⣬ֱ��ɾ��
                {
                    gdsl_list_cursor_delete(cr);
                    continue;
                }
            }

            if(pechProtoElem->status == 1)
            {
                gdsl_list_cursor_delete(cr);
                continue;
            }
#if 1
            /* 2. �жϳ�ʱ */
            if((time(NULL) - pechProtoElem->timestamp) > pechProtoElem->timeout_s)
            {
                gdsl_list_cursor_delete(cs);
                continue;
            }
#endif
            gdsl_list_cursor_step_forward (cr);
        }

        /* ����SendCmd */

        gdsl_list_cursor_move_to_head (cs);
        while(pechProtoElem = gdsl_list_cursor_get_content (cs))
        {
            /* 1. �ж�Э���Ƿ��� */
            if(pechProtoElem->status == 0)
            {
                memmove(tcp_client_sendbuf, pechProtoElem->pbuff, pechProtoElem->len);
                send_len = pechProtoElem->len;
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendReq);
                //uxBitsTCP = xEventGroupWaitBits(xHandleEventTCP, defEventBitTCPClientSendOK, pdTRUE, pdTRUE, 200);
                //�Ȳ��ȵõ�����1
                pechProtoElem->status = 1;

            }
           /* 2. �ѷ��ͣ��жϷ������*/
            if(pechProtoElem->status == 1)
            {
                /* �ж������֣�
                   ��������������ȴ������ظ�
                   ����ǻظ������ɾ��
                */
                #if 0 //ע�͵��ⲿ�ֵ�˼·�Ժ��������UID
                /** @todo (rgw#1#): ������Ҫ������Ƚ�Э��UID��ɾ�����ܵ���UID�뷢��UID��ͬ������ */
                if(xSemaphoreTake(pProto->pCMD[pechProtoElem->cmd_id]->xMutexCmd, 1000) == pdTRUE)
                {
                    ccmd = gdsl_list_cursor_alloc(pProto->pCMD[pechProtoElem->cmd_id]->plRecvCmd);
                    gdsl_list_cursor_move_to_head (ccmd);
                    while(pechCmdElem = gdsl_list_cursor_get_content(ccmd))
                    {
                        if(pechCmdElem->status == 1)//�����ڸ���res�������Ѿ�����ȡ������
                        {
                            gdsl_list_cursor_delete(cs);//���������յ������ظ�, ɾ������
                            break;
                        }
                        gdsl_list_cursor_step_forward (ccmd);
                    }

                    gdsl_list_cursor_move_to_head (ccmd);
                    while(pechCmdElem = gdsl_list_cursor_get_content(ccmd))
                    {
                        if(pechCmdElem->status == 1)//�����ڸ���res�������Ѿ�����ȡ������
                        {
                            gdsl_list_cursor_delete(ccmd);
                            continue;
                        }
                        gdsl_list_cursor_step_forward (ccmd);
                    }
                    gdsl_list_cursor_free(ccmd);
                    xSemaphoreGive(pProto->xMutexCmd);
                }
                #endif
                uxBitsTCP = xEventGroupWaitBits(pProto->pCMD[pechProtoElem->cmd_id]->xHandleEventCmd,
                                                defEventBitProtoCmdHandled,
                                                pdTRUE, pdTRUE, 0);
                if((uxBitsTCP & defEventBitProtoCmdHandled) == defEventBitProtoCmdHandled)
                {
                    gdsl_list_cursor_delete(cs);//���������յ�ƽ̨�ظ����Ѵ���, ɾ������
                    continue;
                }
                if(pechProtoElem->trycount >= pechProtoElem->trycountmax)
                {
                    gdsl_list_cursor_delete(cs);
                    continue;
                }

            }
#if 1
            /* 2. �жϳ�ʱ ����ʱ����״̬Ϊ0���ٴν��з���*/
            if((time(NULL) - pechProtoElem->timestamp) > pechProtoElem->timeout_s)
            {
                pechProtoElem->trycount++;
                pechProtoElem->timestamp = time(NULL);
                pechProtoElem->status = 0;
                continue;//��������������������
            }
#endif
            /* 3. */
            gdsl_list_cursor_step_forward (cs);
        }


        vTaskDelay(10);
    }
}
