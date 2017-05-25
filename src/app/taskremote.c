/**
* @file taskremote.c
* @brief Զ��ͨ�ŵĲ���������ͨ��ʵ���ָ��
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "includes.h"
#include "interface.h"
#include "taskcreate.h"
#include "taskremote.h"

/** @todo (rgw#1#): ���״̬ʱCharging����ôRemote��״̬�����No������err����5���ӣ����ж�ϵͳ������Ӧ��ֹͣ��� */

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
            /** @todo (rgw#1#): ������������ */
            uxBitLwip = xEventGroupWaitBits(xHandleEventLwIP,
                                            defEventBitPPPup,
                                            pdFALSE, pdTRUE, portMAX_DELAY);
            if((uxBitLwip & defEventBitPPPup) == defEventBitPPPup)
            {
                RemoteRegist(pEVSE, pechProto);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_CONNECTED:
            /* ע�� */
            RemoteRegistRes(pEVSE, pechProto, &network_res);
            if(network_res == 1)
            {
                xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
                                   pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
                                   100);//����timer period ����timer start ����
                xTimerChangePeriod(xHandleTimerRemoteStatus,
                                   pdMS_TO_TICKS(pechProto->info.ulStatusCyc_ms),
                                   100);//����timer period ����timer start ����
                remotestat = REMOTE_REGEDITED;
            }

            break;
        case REMOTE_REGEDITED:

            /* ���� */
            uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                                defEventBitTimerCBHeartbeat,
                                                pdTRUE, pdTRUE , 0);
            if((uxBitsTimerCB & defEventBitTimerCBHeartbeat) == defEventBitTimerCBHeartbeat)
            {
                RemoteHeart(pEVSE, pechProto);
            }
            /* ״̬*/
            uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                                defEventBitTimerCBStatus,
                                                pdTRUE, pdTRUE , 0);
            if((uxBitsTimerCB & defEventBitTimerCBStatus) == defEventBitTimerCBStatus)
            {
                for(i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    RemoteStatus(pEVSE, pechProto, pCON);
                }
            }
            /* ��ȡ�ʻ���Ϣ*/
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
