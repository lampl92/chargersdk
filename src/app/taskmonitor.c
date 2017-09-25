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
    ErrorCode_t errcode;

    ulTotalCON = pListCON->Total;
    uxBitsTimerCB = 0;
    errcode = ERR_NO;
#ifndef DEBUG_INIT
    vTaskDelay(1000);
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
                THROW_ERROR(i, pCON->status.GetACLTemp(pCON), ERR_LEVEL_WARNING, "Monitor");
                THROW_ERROR(i, pCON->status.GetACNTemp(pCON), ERR_LEVEL_WARNING, "Monitor");
                if(pCON->info.ucSocketType == defSocketTypeB)
                {
                    THROW_ERROR(i, pCON->status.GetBTypeSocketTemp1(pCON), ERR_LEVEL_WARNING, "Monitor");
                    THROW_ERROR(i, pCON->status.GetBTypeSocketTemp2(pCON), ERR_LEVEL_WARNING, "Monitor");
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
                    THROW_ERROR(i, pCON->status.GetBTypeSocketLock(pCON), ERR_LEVEL_WARNING, "Monitor");
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
                THROW_ERROR(i, pCON->status.GetPlugState(pCON), ERR_LEVEL_CRITICAL, "Monitor");//在GetPlugState中获取了CC与CP状态
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagPlugState);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBVolt, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBVolt) == defEventBitTimerCBVolt)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(i, pCON->status.GetChargingVoltage(pCON), ERR_LEVEL_CRITICAL, "Monitor");
#ifdef EVSE_DEBUG
                THROW_ERROR(i, errcode = pCON->status.GetChargingPower(pCON), ERR_LEVEL_CRITICAL, "Monitor");
#endif
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagVolt);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                //THROW_ERROR(i, pCON->status.GetChargingVoltage(pCON), ERR_LEVEL_CRITICAL, "Monitor");
                THROW_ERROR(i, errcode = pCON->status.GetChargingCurrent(pCON), ERR_LEVEL_CRITICAL, "Monitor");
                THROW_ERROR(i, errcode = pCON->status.GetChargingFrequence(pCON), ERR_LEVEL_CRITICAL, "Monitor");
#ifndef EVSE_DEBUG
                THROW_ERROR(i, errcode = pCON->status.GetChargingPower(pCON), ERR_LEVEL_CRITICAL, "Monitor");
#endif
                if(errcode == ERR_NO)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionMeter);
                }
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagChargingData);
        }
        else
        {
            /// TODO (zshare#1#): 后续更改这块，这块会一直占用cpu
            for(i = 0;i <ulTotalCON;i++)
            {
                pCON = CONGetHandle(i);
                if(pCON->state != STATE_CON_CHARGING)
                {
                    xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONCurrOK);
                    xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONFreqOK);
                }
            }
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBEVSEState, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetScramState(pEVSE), ERR_LEVEL_CRITICAL, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPEState(pEVSE), ERR_LEVEL_CRITICAL, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetKnockState(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetArresterState(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPowerOffState(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_A_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_B_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_C_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_N_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
	        
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagEVSEState);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID, pdTRUE, pdFALSE, 0);
        if((uxBitsTimerCB & defEventBitTimerCBRFID) == defEventBitTimerCBRFID)
        {
            THROW_ERROR(defDevID_RFID, errcode = pRFIDDev->status.GetCardID(pRFIDDev), ERR_LEVEL_CRITICAL, "Monitor");
            if (errcode == ERR_NO)
            {
                xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRFID);
                pEVSE->status.ulSignalFault &= ~defSignalEVSE_Fault_RFID;
            }
            else
            {
                pEVSE->status.ulSignalFault |= defSignalEVSE_Fault_RFID;
            }
        }

        /* end of 获取EVSE和CON状态 */
#endif //DEBUG_NO_TASKMONITOR
        vTaskDelay(20);//要比timer中的检测周期快
    }/* end of while(1)*/
}
