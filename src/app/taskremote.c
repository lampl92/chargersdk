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

void vTaskEVSERemote(void *pvParameters)
{
    EventBits_t uxBitsRFID = 0;
    OrderData_t OrderData;
    ErrorCode_t errcode;

    uxBitsRFID = 0;
    OrderData.ucAccountStatus = 0;
    OrderData.dBalance = 0;
    errcode = 0;

    while(1)
    {
        uxBitsRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                     defEventBitIsNewUID,
                                     pdTRUE, pdFALSE, 0);
        if((uxBitsRFID & defEventBitIsNewUID) == defEventBitIsNewUID)
        {
            xQueueReceive(xHandleQueueOrders, &OrderData, 1000);
            THROW_ERROR(defDevID_Cloud,
                        errcode = RemoteGetBalance(OrderData.ucUID, defUIDLength, &OrderData.ucAccountStatus, &OrderData.dBalance),
                        ERR_LEVEL_CRITICAL);
            if(errcode == ERR_NO)
            {
                xQueueSend(xHandleQueueOrders, &OrderData, 0);
                xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID,defEventBitGetAccountStatus);
            }
        }
#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
        vTaskDelay(100);
    }
}
