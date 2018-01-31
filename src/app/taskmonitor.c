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
//#define TEST_TIME_TEMP  "TEMP"
//#define TEST_TIME_LOCK  "LOCK"
//#define TEST_TIME_PLUG  "PLUG"
//#define TEST_TIME_ChData  "ChData"
//#define TEST_TIME_EvseData  "EvseData"
//#define TEST_TIME_rfid  "rfid"

static TaskHandle_t xHandleTaskChData[defTotalCON] = { NULL };
static TaskHandle_t xHandleTaskEvseData = NULL;

void vTaskMonitor_ChData(void *pvParameters)
{
    CON_t *pCON = NULL;
    EventBits_t uxBitsTimerCB;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvParameters;
    uxBitsTimerCB = 0;
    errcode = ERR_NO;

    while (1)
    {
        uxBitsTimerCB = xEventGroupWaitBits(pCON->status.xHandleEventTimerCBNotify, 
                                            defEventBitTimerCBChargingData, 
                                            pdTRUE, pdTRUE, 0);
        if ((uxBitsTimerCB & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
                #ifdef TEST_TIME_ChData
                printf_safe("begin %s %d\n", TEST_TIME_ChData, clock());
                #endif // TEST_TIME_ChData
                THROW_ERROR(pCON->info.ucCONID, errcode = pCON->status.GetChargingVoltage(pCON), ERR_LEVEL_CRITICAL, "Monitor");
                THROW_ERROR(pCON->info.ucCONID, errcode = pCON->status.GetChargingCurrent(pCON), ERR_LEVEL_CRITICAL, "Monitor");
                THROW_ERROR(pCON->info.ucCONID, errcode = pCON->status.GetChargingFrequence(pCON), ERR_LEVEL_CRITICAL, "Monitor");
                THROW_ERROR(pCON->info.ucCONID, errcode = pCON->status.GetChargingPower(pCON), ERR_LEVEL_CRITICAL, "Monitor");
                #ifdef TEST_TIME_ChData
                printf_safe("end %s %d\n", TEST_TIME_ChData, clock());
                #endif // TEST_TIME_ChData    
                if (errcode == ERR_NO)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionMeter);
                }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagChargingData);
        }
        vTaskDelay(10);
    }
}

void vTaskMonitor_EvseData(void *pvParameters)
{
    EventBits_t uxBitsTimerCB;
    ErrorCode_t errcode;

    uxBitsTimerCB = 0;
    errcode = ERR_NO;

    while (1)
    {
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBEVSEState, pdTRUE, pdFALSE, 0);
        if ((uxBitsTimerCB & defEventBitTimerCBEVSEState) == defEventBitTimerCBEVSEState)
        {
#ifdef TEST_TIME_EvseData
            printf_safe("begin %s %d\n", TEST_TIME_EvseData, clock());
#endif // TEST_TIME_EvseData
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetScramState(pEVSE), ERR_LEVEL_CRITICAL, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPEState(pEVSE), ERR_LEVEL_CRITICAL, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetKnockState(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetArresterState(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetPowerOffState(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_A_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_B_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_C_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_N_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
#ifdef TEST_TIME_EvseData
            printf_safe("end %s %d\n", TEST_TIME_EvseData, clock());
#endif // TEST_TIME_EvseData        
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagEVSEState);
        }
        vTaskDelay(10);
    }
}

void vTaskEVSEMonitor(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    char pcName[configMAX_TASK_NAME_LEN];
    EventBits_t uxBitsTimerCB;
    ErrorCode_t errcode;

    ulTotalCON = pListCON->Total;
    uxBitsTimerCB = 0;
    errcode = ERR_NO;
    
    for (i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        sprintf(pcName, "CON%d_ChData", i);
        xTaskCreate(vTaskMonitor_ChData, pcName, 1024, pCON, 23, &xHandleTaskChData[i]);
    }
    xTaskCreate(vTaskMonitor_EvseData, "TaskEvseStatus", 1024, NULL, 23, &xHandleTaskEvseData);
    
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
#ifdef TEST_TIME_TEMP
                printf_safe("begin %s %d\n", TEST_TIME_TEMP, clock());
#endif // TEST_TIME_TEMP

                THROW_ERROR(i, pCON->status.GetACLTemp(pCON), ERR_LEVEL_WARNING, "Monitor");
                THROW_ERROR(i, pCON->status.GetACNTemp(pCON), ERR_LEVEL_WARNING, "Monitor");
                if(pCON->info.ucSocketType == defSocketTypeB)
                {
                    THROW_ERROR(i, pCON->status.GetBTypeSocketTemp1(pCON), ERR_LEVEL_WARNING, "Monitor");
                    THROW_ERROR(i, pCON->status.GetBTypeSocketTemp2(pCON), ERR_LEVEL_WARNING, "Monitor");
                }
#ifdef TEST_TIME_TEMP
                printf_safe("end %s %d\n", TEST_TIME_TEMP, clock());
#endif // TEST_TIME_TEMP

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
#ifdef TEST_TIME_LOCK
                    printf_safe("begin %s %d\n", TEST_TIME_LOCK, clock());
#endif // TEST_TIME_LOCK
                    THROW_ERROR(i, pCON->status.GetBTypeSocketLock(pCON), ERR_LEVEL_WARNING, "Monitor");
#ifdef TEST_TIME_LOCK
                    printf_safe("end %s %d\n", TEST_TIME_LOCK, clock());
#endif // TEST_TIME_LOCK
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
#ifdef TEST_TIME_PLUG
                printf_safe("begin %s %d\n", TEST_TIME_PLUG, clock());
#endif // TEST_TIME_PLUG
                THROW_ERROR(i, pCON->status.GetPlugState(pCON), ERR_LEVEL_CRITICAL, "Monitor");//在GetPlugState中获取了CC与CP状态
#ifdef TEST_TIME_PLUG
                printf_safe("end %s %d\n", TEST_TIME_PLUG, clock());
#endif // TEST_TIME_PLUG
            }
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagPlugState);
        }

        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if ((uxBitsTimerCB & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                xEventGroupSetBits(pCON->status.xHandleEventTimerCBNotify, defEventBitTimerCBChargingData);
            }
        }

        /* end of 获取EVSE和CON状态 */
#endif //DEBUG_NO_TASKMONITOR
        vTaskDelay(10);//要比timer中的检测周期快
    }/* end of while(1)*/
}
