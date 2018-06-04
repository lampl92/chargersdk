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

static TaskHandle_t xHandleTaskChData = NULL ;
static TaskHandle_t xHandleTaskEvseData = NULL;

void vTaskMonitor_ChData(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    EventBits_t uxBitsTimerCB;
    ErrorCode_t errcode;
    int i;
    int trytime = 0;
    int trymax = 3;

    ulTotalCON = pListCON->Total; 
    uxBitsTimerCB = 0;
    errcode = ERR_NO;

    while (1)
    {
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData, pdTRUE, pdFALSE, 0);
        if ((uxBitsTimerCB & defEventBitTimerCBChargingData) == defEventBitTimerCBChargingData)
        {
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
#ifdef TEST_TIME_ChData
                printf_safe("con%d begin %s %d\n", pCON->info.ucCONID, TEST_TIME_ChData, clock());
#endif // TEST_TIME_ChData
                errcode = pCON->status.GetChargingData(pCON);
                if (errcode == ERR_NO)
                {
                    trytime = 0;
                    pCON->status.ulSignalFault &= ~defSignalCON_Fault_Meter;
                    xEventGroupSetBits(pCON->status.xHandleEventDiag, defEventBitDiagChargingData);
                }
                else if (errcode == ERR_CON_METER_FAULT)
                {
                    ++trytime;
                    printf("meter try %d\n", trytime);
                    if (trytime > trymax)
                    {
                        trytime = 0;
                        pCON->status.ulSignalFault |= defSignalCON_Fault_Meter;
                    }
                }
                
#ifdef TEST_TIME_ChData
                printf_safe("con%d end   %s %d\n", pCON->info.ucCONID, TEST_TIME_ChData, clock());
#endif // TEST_TIME_ChData   
            }
        }
        
        vTaskDelay(10);
    }
}

void vTaskMonitor_EvseStatus(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    EventBits_t uxBitsTimerCB;
    ErrorCode_t errcode;
    int i;

    ulTotalCON = pListCON->Total; 
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
           
#ifdef TEST_TIME_EvseData
            printf_safe("end %s %d\n", TEST_TIME_EvseData, clock());
#endif // TEST_TIME_EvseData        
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagEVSEState);
        }
        
        uxBitsTimerCB = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp, pdTRUE, pdFALSE, 0);
        if ((uxBitsTimerCB & defEventBitTimerCBTemp) == defEventBitTimerCBTemp)
        {
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_A_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_B_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_C_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            THROW_ERROR(defDevID_EVSE, pEVSE->status.GetAC_N_Temp_in(pEVSE), ERR_LEVEL_TIPS, "Monitor");
            xEventGroupSetBits(xHandleEventDiag, defEventBitDiagTemp);
            
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
#ifdef TEST_TIME_TEMP
                printf_safe("con%d begin %s %d\n", pCON->info.ucCONID, TEST_TIME_TEMP, clock());
#endif // TEST_TIME_TEMP
                THROW_ERROR(pCON->info.ucCONID, pCON->status.GetACLTemp(pCON), ERR_LEVEL_WARNING, "Monitor");
                THROW_ERROR(pCON->info.ucCONID, pCON->status.GetACNTemp(pCON), ERR_LEVEL_WARNING, "Monitor");
                if (pCON->info.ucSocketType == defSocketTypeB)
                {
                    THROW_ERROR(pCON->info.ucCONID, pCON->status.GetBTypeSocketTemp1(pCON), ERR_LEVEL_WARNING, "Monitor");
                    THROW_ERROR(pCON->info.ucCONID, pCON->status.GetBTypeSocketTemp2(pCON), ERR_LEVEL_WARNING, "Monitor");
                }
#ifdef TEST_TIME_TEMP
                printf_safe("con%d end %s %d\n", pCON->info.ucCONID, TEST_TIME_TEMP, clock());
#endif // TEST_TIME_TEMP
                xEventGroupSetBits(pCON->status.xHandleEventDiag, defEventBitDiagTemp);
            }
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
    
    xTaskCreate(vTaskMonitor_ChData, "TaskChData", 2048, NULL, 23, &xHandleTaskChData);
    xTaskCreate(vTaskMonitor_EvseStatus, "TaskEvseStatus", 2048, NULL, 23, &xHandleTaskEvseData);
    
    while(1)
    {
#ifndef DEBUG_NO_TASKMONITOR
        /* 获取EVSE和CON状态 */

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
                xEventGroupSetBits(pCON->status.xHandleEventDiag, defEventBitDiagLockState);
            }
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
                xEventGroupSetBits(pCON->status.xHandleEventDiag, defEventBitDiagPlugState);
            }
        }

        /* end of 获取EVSE和CON状态 */
#endif //DEBUG_NO_TASKMONITOR
        vTaskDelay(10);//要比timer中的检测周期快
    }/* end of while(1)*/
}
