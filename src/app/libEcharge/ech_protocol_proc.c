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
#include "FreeRTOS.h"
#include "event_groups.h"
#include "evse_globals.h"
#include "task_tcp_client.h"
#include "taskcreate.h"


void vTaskRemoteCmdProc(void *pvParameters)
{
    echProtocol_t *pProto;
    echCmdElem_t *pechCmdElem;
    gdsl_list_cursor_t c;

    uint32_t ulSendCmdCount;
    uint32_t ulRecvCmdCount;
    EventBits_t uxBitsLwip;
    int res;
    int i;

    //pProto = (echProtocol_t *)pvParameters;
    pProto = pechProto;
    while(1)
    {
        /* 遍历SendCmd */
        c = gdsl_list_cursor_alloc (pProto->plechSendCmd);

        for(gdsl_list_cursor_move_to_head (c); pechCmdElem = gdsl_list_cursor_get_content (c); gdsl_list_cursor_step_forward (c))
        {
            /* 1. 判断协议是否发送 */
            if(pechCmdElem ->status == 0)
            {
                memcpy(tcp_client_sendbuf, pechCmdElem->pbuff, pechCmdElem->len);
                send_len = pechCmdElem->len;
                uxBitsLwip = xEventGroupSync(xHandleEventLwIP,
                                             defEventBitTCPClientSendReq,
                                             defEventBitTCPClientSendOK,
                                             10000);
                if((uxBitsLwip & defEventBitTCPClientSendOK) == defEventBitTCPClientSendOK)
                {
                    pechCmdElem->status = 1;
                }
                else//没发出去
                {
                    pechCmdElem->trycount++;
                    if(pechCmdElem->trycount > pechCmdElem->trycountmax)
                    {
                        gdsl_list_cursor_delete(c);
                        gdsl_list_cursor_step_backward(c);//删除之后光标会自动forward一步，for循环中forward就会错过一个元素，因此向后移一步。
                        continue;
                        /* @todo (rgw#1#): 通知系统网络发生问题，需要重启gprs */
                    }
                }
            }
            else if(pechCmdElem->status == 1)
            {
                /* 判断命令字，
                   如果是请求命令，则等待主机回复
                   如果是回复命令，则删除
                */
                if(pProto->pCMD[pechCmdElem->cmd_id]->uiRecvdOptLen > 0) //请求命令收到回复
                {
                    gdsl_list_cursor_delete(c);
                    gdsl_list_cursor_step_backward(c);//删除之后光标会自动forward一步，for循环中forward就会错过一个元素，因此向后移一步。
                    continue;
                }

            }
#if 0
            /* 2. 判断超时 ，删除发送未收到回复的命令*/
            if((time(NULL) - pechCmdElem->timestamp) > pechCmdElem->timeout)
            {
                gdsl_list_cursor_delete(c);
                gdsl_list_cursor_step_backward(c);//删除之后光标会自动forward一步，for循环中forward就会错过一个元素，因此向后移一步。
                continue;
            }
#endif
            /* 3. */
        }
        gdsl_list_cursor_free(c);

        /* 遍历RecvCmd */
        c = gdsl_list_cursor_alloc (pProto->plechRecvCmd);
        for(gdsl_list_cursor_move_to_head (c); pechCmdElem = gdsl_list_cursor_get_content (c); gdsl_list_cursor_step_forward (c))
        {
            if(pechCmdElem->status == 0)
            {
                res = pProto->pCMD[pechCmdElem->cmd_id]->analyProc(pProto, pechCmdElem->cmd_id, pechCmdElem->pbuff, pechCmdElem->len);
                if(res == 1)
                {
                    pechCmdElem->status = 1;
                }
                else//接收的协议帧序列有问题，直接删除
                {
                    gdsl_list_cursor_delete(c);
                    gdsl_list_cursor_step_backward(c);//删除之后光标会自动forward一步，for循环中forward就会错过一个元素，因此向后移一步。
                    continue;
                }
            }
            else if(pechCmdElem->status == 1)
            {
                gdsl_list_cursor_delete(c);
                gdsl_list_cursor_step_backward(c);//删除之后光标会自动forward一步，for循环中forward就会错过一个元素，因此向后移一步。
                continue;
            }
#if 0
             /* 2. 判断超时 ，删除发送未收到回复的命令*/
            if((time(NULL) - pechCmdElem->timestamp) > pechCmdElem->timeout)
            {
                gdsl_list_cursor_delete(c);
                gdsl_list_cursor_step_backward(c);//删除之后光标会自动forward一步，for循环中forward就会错过一个元素，因此向后移一步。
                continue;
            }
            #endif
        }
        gdsl_list_cursor_free(c);

        vTaskDelay(100);
    }
    while(1);
}
