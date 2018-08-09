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
#include "taskcreate.h"
#include "interface_network.h"
#include "evse_debug.h"

void vTaskRemoteCmdProc(void *pvParameters)
{
    echProtocol_t *pProto;
    echProtoElem_t *pechProtoElem;
    gdsl_list_cursor_t cs;
    gdsl_list_cursor_t cr;

    uint32_t ulSendCmdCount;
    uint32_t ulRecvCmdCount;
    EventBits_t uxBitsTCP;
    int res;
    int i_deb;

    pProto = (echProtocol_t *)pvParameters;
    cs = gdsl_list_cursor_alloc(pProto->plechSendCmd);
    cr = gdsl_list_cursor_alloc(pProto->plechRecvCmd);
    while (1)
    {
//        cli_tasklist_fnt();
//        printf_safe("send elem = %d\n", gdsl_list_get_size(pProto->plechSendCmd));
//        printf_safe("recv elem = %d\n", gdsl_list_get_size(pProto->plechRecvCmd));
//        printf_safe("\n");

        /* 遍历RecvCmd */

        if (xSemaphoreTake(pProto->xMutexProtoRecv, 1000) == pdPASS)
        {
            gdsl_list_cursor_move_to_head(cr);
            while ((pechProtoElem = gdsl_list_cursor_get_content(cr)) != NULL)
            {
                if (pechProtoElem->status == 0)
                {
                    res = pProto->pCMD[pechProtoElem->cmd_id]->analyProc(pProto,
                        pechProtoElem->cmd_id,
                        pechProtoElem->pbuff,
                        pechProtoElem->len);
                    if (res == 1)
                    {
                        pechProtoElem->status = 1; //接收协议入队等待处理
                    }
                    else//接收的协议帧序列有问题，直接删除
                    {
                        gdsl_list_cursor_delete(cr);
                        continue;
                    }
                }

                if (pechProtoElem->status == 1)
                {
                    gdsl_list_cursor_delete(cr);
                    continue;
                }
#if 0 //接收无超时处理
                /* 2. 判断超时 */
                if ((time(NULL) - pechProtoElem->timestamp) > pechProtoElem->timeout_s)
                {
                    gdsl_list_cursor_delete(cs);
                    continue;
                }
#endif
                gdsl_list_cursor_step_forward(cr);
            }//while
            xSemaphoreGive(pProto->xMutexProtoRecv);
        }//if mutex

        /* 遍历SendCmd */
        if (xSemaphoreTake(pProto->xMutexProtoSend, 1000) == pdPASS)
        {
            gdsl_list_cursor_move_to_head(cs);
            while ((pechProtoElem = gdsl_list_cursor_get_content(cs)) != NULL)
            {
                /* 1. 判断协议是否发送 */
                if (pechProtoElem->status == 0)
                {
                    printf_protolog("SendCmd %d[0x%02X]\n", pechProtoElem->cmd.usSendCmd, pechProtoElem->cmd.usSendCmd);
                    netSend(pechProtoElem->pbuff, pechProtoElem->len);
                    {//debug
                        printf_protodetail3("\nTCP Send: ");
                        for (i_deb = 0; i_deb < pechProtoElem->len; i_deb++)
                        {
                            printf_protodetail3("%02X ", pechProtoElem->pbuff[i_deb]);
                        }
                        printf_protodetail3("\n"); 
                    }
                    pechProtoElem->status = 1;
                    pechProtoElem->trycount++;
                    vTaskDelay(1000);
                }
                /* 2. 已发送，判断发送情况*///由于要根据枪ID判断删除命令，已经将发送处理放到协议接收（inerface_remote.c）中处理
                if (pechProtoElem->status == 1)
                {
//                    /* 预留:判断命令字，
//                       如果是请求命令，则等待主机回复
//                       如果是回复命令，则直接超时删除
//                    */
                }
                /* 3. 判断超时 ，超时后置状态为0，再次进行发送*/
                if ((time(NULL) - pechProtoElem->timestamp) > pechProtoElem->timeout_s)
                {
                    if (pechProtoElem->trycount >= pechProtoElem->trycountmax)
                    {
                        xEventGroupSetBits(pProto->pCMD[pechProtoElem->cmd_id]->xHandleEventCmd, defEventBitProtoCmdDataTimeout);
                        gdsl_list_cursor_delete(cs);
                        printf_protolog("SendCmd %d[0x%02X] Delete\n", pechProtoElem->cmd.usSendCmd, pechProtoElem->cmd.usSendCmd);
                        continue;
                    }
                    pechProtoElem->timestamp = time(NULL);
                    pechProtoElem->status = 0;
                    continue;//跳过后面的语句立即发送，否则需要再等一轮
                }
                /* 4. */
                gdsl_list_cursor_step_forward(cs);
            }//while
            xSemaphoreGive(pProto->xMutexProtoSend);
        }//if mutex

        vTaskDelay(2000);
    }
}