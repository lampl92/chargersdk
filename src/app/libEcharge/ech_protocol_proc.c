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
        printf_safe("send elem = %d\n", gdsl_list_get_size(pProto->plechSendCmd));
        printf_safe("recv elem = %d\n", gdsl_list_get_size(pProto->plechRecvCmd));
        printf_safe("\n");

        /* 遍历RecvCmd */

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
                    pechProtoElem->status = 1; //接收协议入队等待处理
                }
                else//接收的协议帧序列有问题，直接删除
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
#if 0 //接收无超时处理
            /* 2. 判断超时 */
            if((time(NULL) - pechProtoElem->timestamp) > pechProtoElem->timeout_s)
            {
                gdsl_list_cursor_delete(cs);
                continue;
            }
#endif
            gdsl_list_cursor_step_forward (cr);
        }

        /* 遍历SendCmd */

        gdsl_list_cursor_move_to_head (cs);
        while(pechProtoElem = gdsl_list_cursor_get_content (cs))
        {
            /* 1. 判断协议是否发送 */
            if(pechProtoElem->status == 0)
            {
                memmove(tcp_client_sendbuf, pechProtoElem->pbuff, pechProtoElem->len);
                send_len = pechProtoElem->len;
                xEventGroupSetBits(xHandleEventTCP, defEventBitTCPClientSendReq);
                uxBitsTCP = xEventGroupWaitBits(xHandleEventTCP, defEventBitTCPClientSendOK, pdTRUE, pdTRUE, 200);
                //等不等得到都置1
                pechProtoElem->status = 1;

            }
           /* 2. 已发送，判断发送情况*/
            if(pechProtoElem->status == 1)
            {
                /* 判断命令字，
                   如果是请求命令，则等待主机回复
                   如果是回复命令，则删除
                */
                #if 0 //注释的这部分的思路以后用来检测UID
                /** @todo (rgw#1#): 后期需要在这里比较协议UID，删除接受到的UID与发送UID相同的命令 */
                if(xSemaphoreTake(pProto->pCMD[pechProtoElem->cmd_id]->xMutexCmd, 1000) == pdTRUE)
                {
                    ccmd = gdsl_list_cursor_alloc(pProto->pCMD[pechProtoElem->cmd_id]->plRecvCmd);
                    gdsl_list_cursor_move_to_head (ccmd);
                    while(pechCmdElem = gdsl_list_cursor_get_content(ccmd))
                    {
                        if(pechCmdElem->status == 1)//命令在各条res函数中已经被读取并处理。
                        {
                            gdsl_list_cursor_delete(cs);//请求命令收到主机回复, 删除命令
                            break;
                        }
                        gdsl_list_cursor_step_forward (ccmd);
                    }

                    gdsl_list_cursor_move_to_head (ccmd);
                    while(pechCmdElem = gdsl_list_cursor_get_content(ccmd))
                    {
                        if(pechCmdElem->status == 1)//命令在各条res函数中已经被读取并处理。
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
                    gdsl_list_cursor_delete(cs);//请求命令收到平台回复并已处理, 删除命令
                    continue;
                }
                if(pechProtoElem->trycount >= pechProtoElem->trycountmax)
                {
                    gdsl_list_cursor_delete(cs);
                    continue;
                }

            }
#if 1
            /* 3. 判断超时 ，超时后置状态为0，再次进行发送*/
            if((time(NULL) - pechProtoElem->timestamp) > pechProtoElem->timeout_s)
            {
                pechProtoElem->trycount++;
                pechProtoElem->timestamp = time(NULL);
                pechProtoElem->status = 0;
                continue;//跳过后面的语句立即发送，否则需要再等一轮
            }
#endif
            /* 4. */
            gdsl_list_cursor_step_forward (cs);
        }


        vTaskDelay(2000);
    }
}
