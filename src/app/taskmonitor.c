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

//#define DEBUG_NO_TASKMONITOR

void vTaskEVSEMonitor(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsTimerCB;

    ulTotalCON = pListCON->Total;
    uxBitsTimerCB = 0;
#ifndef DEBUG_INIT
    vTaskDelay(10000);
#endif
    while(1)
    {
#ifndef DEBUG_NO_TASKMONITOR
        /* 获取EVSE和CON状态 */

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(i, pCON->status.GetACLTemp(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(i, pCON->status.GetACNTemp(pCON), ERR_LEVEL_WARNING);
                if(pCON->info.ucSocketType == defSocketTypeB)
                {
                    THROW_ERROR(i, pCON->status.GetBTypeSocketTemp1(pCON), ERR_LEVEL_WARNING);
                    THROW_ERROR(i, pCON->status.GetBTypeSocketTemp2(pCON), ERR_LEVEL_WARNING);
                }
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagTemp);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBLockState, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                if(pCON->info.ucSocketType == defSocketTypeB)
                {
                    THROW_ERROR(i, pCON->status.GetBTypeSocketLock(pCON), ERR_LEVEL_WARNING);
                }
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagLockState);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBPlugState, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBPlugState) == defEventBitTimerCBPlugState)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(i, pCON->status.GetPlugState(pCON), ERR_LEVEL_CRITICAL);//在GetPlugState中获取了CC与CP状态
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagPlugState);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(i, pCON->status.GetChargingVoltage(pCON), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pCON->status.GetChargingCurrent(pCON), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pCON->status.GetChargingFrequence(pCON), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pCON->status.GetChargingPower(pCON), ERR_LEVEL_CRITICAL);
                THROW_ERROR(i, pCON->status.GetRelayState(pCON), ERR_LEVEL_CRITICAL);
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
            THROW_ERROR(defDevID_RFID, pRFIDDev->status.GetCardID(pRFIDDev), ERR_LEVEL_CRITICAL);
        }

        /* end of 获取EVSE和CON状态 */
#endif //DEBUG_NO_TASKMONITOR
        vTaskDelay(20);//要比timer中的检测周期快
    }/* end of while(1)*/
}
