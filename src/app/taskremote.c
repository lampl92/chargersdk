/**
* @file taskremote.c
* @brief 远程通信的操作，接收通信实体的指针
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "includes.h"
#include "interface.h"
#include "taskcreate.h"
#include "taskremote.h"

/** @todo (rgw#1#): 如果状态时Charging，那么Remote的状态如果是No或者是err超过5分钟，则判断系统断网，应该停止充电 */

//#define DEBUG_NO_TASKREMOTE

typedef enum
{
    REMOTE_NO,
    REMOTE_CONNECTED,
    REMOTE_REGEDITED,
    REMOTE_RECONNECT,
    REMOTE_ERROR
} RemoteState_t;

void vTaskEVSERemote(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsRFID;
    EventBits_t uxBitsTimerCB;
    EventBits_t uxBitLwip;
    RemoteState_t remotestat;
    Heartbeat_t *pHeart;
    ErrorCode_t errcode;
    int network_res;

    ulTotalCON = pListCON->Total;
    uxBitsRFID = 0;
    uxBitsTimerCB = 0;
    remotestat = REMOTE_NO;
    errcode = 0;
    network_res = 0;

    while(1)
    {
#ifndef DEBUG_NO_TASKREMOTE
        switch(remotestat)
        {
        case REMOTE_NO:
            /** @todo (rgw#1#): 尝试连接网络 */
            uxBitLwip = xEventGroupWaitBits(xHandleEventLwIP, defEventBitPPPup, pdFALSE, pdTRUE, portMAX_DELAY);
            if((uxBitLwip & defEventBitPPPup) == defEventBitPPPup)
            {
                RemoteRegist(pEVSE, pechProto, &network_res);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_CONNECTED:
            /* 注册 */
            //            uxBitLwip = xEventGroupWaitBits(xHandleEventLwIP, defEventBitCmdRegedit, pdTRUE, pdTRUE, 0);
//            if((uxBitLwip & defEventBitCmdRegedit) == defEventBitCmdRegedit)
//            {
            RemoteRegistRes(pEVSE, pechProto, &network_res);
            if(network_res == 1)
            {
                xTimerStart(xHandleTimerHeartbeat, 0);
                remotestat = REMOTE_REGEDITED;
            }

            break;
        case REMOTE_REGEDITED:

            /* 心跳 */
            uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                                defEventBitTimerCBHeartbeat,
                                                pdTRUE, pdTRUE , 0);
            if((uxBitsTimerCB & defEventBitTimerCBHeartbeat) == defEventBitTimerCBHeartbeat)
            {
                RemoteHeart(pEVSE, pechProto);
            }
            /* 获取帐户信息*/
            uxBitsRFID = xEventGroupWaitBits(xHandleEventRemote,
                                             defEventBitRemoteGetAccount,
                                             pdTRUE, pdFALSE, 0);
            if((uxBitsRFID & defEventBitRemoteGetAccount) == defEventBitRemoteGetAccount)
            {
                THROW_ERROR(defDevID_Cloud,
                            errcode = RemoteGetBalance(pRFIDDev->order.ucCardID,
                                                       defCardIDLength,
                                                       &(pRFIDDev->order.ucAccountStatus),
                                                       &(pRFIDDev->order.dBalance)),
                            ERR_LEVEL_CRITICAL, "Remote GetBalance");
                if(errcode == ERR_NO)
                {
                    xEventGroupSetBits(xHandleEventRemote, defEventBitRemoteGotAccount);
                }
            }
            break;
        }

#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
#endif //#ifndef DEBUG_NO_TASKREMOTE
        vTaskDelay(100);
    }
}
