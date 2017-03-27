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

//#define DEBUG_NO_TASKREMOTE

typedef enum
{
    REMOTE_NO,
    REMOTE_CONNECTED,
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
    OrderData_t tmpRfidOrderData;
    RemoteState_t remotestat;
    Heartbeat_t *pHeart;
    ErrorCode_t errcode;

    ulTotalCON = pListCON->Total;
    uxBitsRFID = 0;
    uxBitsTimerCB = 0;
    tmpRfidOrderData.ucAccountStatus = 0;
    tmpRfidOrderData.dBalance = 0;
    remotestat = REMOTE_NO;
    errcode = 0;

    while(1)
    {
#ifndef DEBUG_NO_TASKREMOTE
        switch(remotestat)
        {
        case REMOTE_NO:
            /** @todo (rgw#1#): ������������ */
            xTimerStart(xHandleTimerHeartbeat, 0);
            remotestat = REMOTE_CONNECTED;
            break;
        case REMOTE_CONNECTED:
            /* ���� */
            uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBHeartbeat,
                                                pdTRUE, pdFALSE, 0);
            if((uxBitsTimerCB & defEventBitTimerCBHeartbeat) == defEventBitTimerCBHeartbeat)
            {
                for(i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    //pHeart = makeHeart(pEVSE, pCON);
                }
            }
            /* ��ȡ�ʻ���Ϣ*/
            uxBitsRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                             defEventBitIsNewID,
                                             pdTRUE, pdFALSE, 0);
            if((uxBitsRFID & defEventBitIsNewID) == defEventBitIsNewID)
            {
                xQueueReceive(xHandleQueueOrders, &tmpRfidOrderData, 1000);
                THROW_ERROR(defDevID_Cloud,
                            errcode = RemoteGetBalance(tmpRfidOrderData.ucCardID, defCardIDLength, &tmpRfidOrderData.ucAccountStatus, &tmpRfidOrderData.dBalance),
                            ERR_LEVEL_CRITICAL);
                if(errcode == ERR_NO)
                {
                    xQueueSend(xHandleQueueOrders, &tmpRfidOrderData, 0);
                    xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGetAccountStatus);
                }
            }
        }

#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
#endif //#ifndef DEBUG_NO_TASKREMOTE
        vTaskDelay(100);
    }
}
