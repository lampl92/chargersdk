/**
* @file taskmonitor.c
* @brief 监控任务
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"

void vTaskEVSEMonitor(void *pvParameters)
{
    ChargePoint_t *pPoint = NULL;
    uint32_t ulTotalPoint;
    int i;
    EventBits_t uxBitsTimerCB;

    ulTotalPoint = pListChargePoint->Total;
    uxBitsTimerCB = 0;
    //vTaskDelay(10000);
    while(1)
    {
        /* 获取EVSE和ChargePoint状态 */

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(i, pPoint->status.GetACLTemp(pPoint), ERR_LEVEL_WARNING);
                THROW_ERROR(i, pPoint->status.GetACNTemp(pPoint), ERR_LEVEL_WARNING);
                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(i, pPoint->status.GetBTypeConnectorTemp1(pPoint), ERR_LEVEL_WARNING);
                    THROW_ERROR(i, pPoint->status.GetBTypeConnectorTemp2(pPoint), ERR_LEVEL_WARNING);
                }
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagTemp);
        }
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBLockState, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                if(pPoint->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(i, pPoint->status.GetBTypeConnectorLock(pPoint), ERR_LEVEL_WARNING);
                }
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagLockState);
        }
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBPlugState, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBPlugState) == defEventBitTimerCBPlugState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(i, pPoint->status.GetPlugState(pPoint), ERR_LEVEL_CRITICAL);//在GetPlugState中获取了CC与CP状态
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagPlugState);
        }
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(i, pPoint->status.GetChargingVoltage(pPoint), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint->status.GetChargingCurrent(pPoint), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint->status.GetChargingFrequence(pPoint), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pPoint->status.GetRelayState(pPoint), ERR_LEVEL_CRITICAL);
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagChargingData);
        }
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBEVSEState, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetScramState(pEVSE), ERR_LEVEL_CRITICAL);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPEState(pEVSE), ERR_LEVEL_CRITICAL);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetKnockState(pEVSE), ERR_LEVEL_TIPS);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetArresterState(pEVSE), ERR_LEVEL_TIPS);
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPowerOffState(pEVSE), ERR_LEVEL_TIPS);
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagEVSEState);
        }
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBRFID) == defEventBitTimerCBRFID)
        {
            THROW_ERROR(defDevID_RFID, pRFIDDev->status.GetUID(pRFIDDev), ERR_LEVEL_CRITICAL);
        }

        /* end of 获取EVSE和ChargePoint状态 */

#if DEBUG_MONITOR
        printf_safe("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(20);//要比timer中的检测周期快
    }/* end of while(1)*/
}
