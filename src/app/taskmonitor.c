/**
* @file taskmonitor.c
* @brief �������
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"

void vTaskEVSEMonitor(void *pvParameters)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    uint8_t ucCurrentId;
    EventBits_t uxBits;

    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    ucCurrentId = 0;
    uxBits = 0;
    while(1)
    {
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                THROW_ERROR(pPoint[i]->status.GetACLTemp(pPoint[i]), ERR_LEVEL_WARNING);
                THROW_ERROR(pPoint[i]->status.GetACNTemp(pPoint[i]), ERR_LEVEL_WARNING);
                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(pPoint[i]->status.GetBTypeConnectorTemp1(pPoint[i]), ERR_LEVEL_WARNING);
                    THROW_ERROR(pPoint[i]->status.GetBTypeConnectorTemp2(pPoint[i]), ERR_LEVEL_WARNING);
                }
                //printf_safe("num = %d, ulIntervalOfGetTemp\n", i);
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBLockState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBLockState) == defEventBitTimerCBLockState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
                {
                    THROW_ERROR(pPoint[i]->status.GetBTypeConnectorLock(pPoint[i]), ERR_LEVEL_WARNING);
//              printf_safe("num = %d, ulIntervalOfGetLock\n", i);
                }
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBCPCCState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBCPCCState) == defEventBitTimerCBCPCCState)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                THROW_ERROR(pPoint[i]->status.GetCCState(pPoint[i]), ERR_LEVEL_CRITICAL);
                THROW_ERROR(pPoint[i]->status.GetCPState(pPoint[i]), ERR_LEVEL_CRITICAL);
//            printf_safe("num = %d, defTIMERID_CPCCState\n", i);
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalPoint; i++)
            {
                THROW_ERROR(pPoint[i]->status.GetChargingVoltage(pPoint[i]), ERR_LEVEL_CRITICAL);
                THROW_ERROR(pPoint[i]->status.GetChargingCurrent(pPoint[i]), ERR_LEVEL_CRITICAL);
                THROW_ERROR(pPoint[i]->status.GetChargingFrequence(pPoint[i]), ERR_LEVEL_CRITICAL);
            }
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBEVSEState, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
            THROW_ERROR(pEVSE->status.GetScramState(pEVSE), ERR_LEVEL_CRITICAL);
            THROW_ERROR(pEVSE->status.GetPEState(pEVSE), ERR_LEVEL_CRITICAL);
            THROW_ERROR(pEVSE->status.GetKnockState(pEVSE), ERR_LEVEL_TIPS);
            THROW_ERROR(pEVSE->status.GetArresterState(pEVSE), ERR_LEVEL_TIPS);
            THROW_ERROR(pEVSE->status.GetPowerOffState(pEVSE), ERR_LEVEL_TIPS);
            //printf_safe("EVSE State,TimerTicks = %d\n",ulHighFrequencyTimerTicks);
        }
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBRFID) == defEventBitTimerCBRFID)
        {
            THROW_ERROR(pRFIDDev->status.GetUID(pRFIDDev), ERR_LEVEL_CRITICAL);
        }

//        /** �ж�ǹ״̬ */
//        do
//        {
//            if(pPoint[ucCurrentId]->status.xBTypeConnectorLockState == LOCK)
//            {
//
//            }
//            //if(pPoint[ucCurrentId]->state.dACLTemp)
//            ucCurrentId++;
//        }while(ucCurrentId < ulTotalPoint);
//        ucCurrentId = 0;
        /** end of �ж�ǹ״̬ */

#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(50);
    }
}
