/**
* @file taskdiag.c
* @brief 系统诊断任务。
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskdiag.h"
#include "interface.h"

//#define DEBUG_NO_TASKDIAG

void vTaskEVSEDiag(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsDiag;
    EventBits_t uxBitsException;
    BaseType_t xResult;
    ErrorPackage_t errpack;

    ulTotalCON = pListCON->Total;
    uxBitsDiag = 0;
    uxBitsException = 0;
    xResult = pdFALSE;
    while(1)
    {
#ifndef DEBUG_NO_TASKDIAG
        /* 处理系统失效故障 */
        xResult = xQueueReceive(xHandleQueueErrorPackage, &errpack, 0);
        if(xResult == pdTRUE)
        {
#ifdef EVSE_ERRLOG
            printf_safe("%X %s(code: %d,level: %d)\n", errpack.ulDevID, strErrorCode[errpack.code], errpack.code, errpack.level);
            printf_safe("   %s\n", errpack.msg);
#endif
            switch(errpack.code) // ！！！ 这里一定要仔细查看errpack.ulDevID是否可以用作 CONID， 主要是看设备是否是归属于枪还是桩。 ！！！
            {
            case ERR_RFID_FAULT:
                break;
            case ERR_CON_CP_FAULT:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionCPSwitch);
                break;
            case ERR_CON_ACTEMP_DECT_FAULT:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionTempSensor);
                break;
            case ERR_CON_BTEMP1_DECT_FAULT:
            case ERR_CON_BTEMP2_DECT_FAULT:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionSocketTempSensor);
                break;
            default:
                break;
            }
        }

        /* end of 处理系统失效故障 */
        
        for (i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            if ((pCON->status.ulSignalState & defSignalCON_State_Working) != defSignalCON_State_Working)
            {
                //发生温度告警（max_temp-10 ~ max_temp）
                if ((pCON->status.ulSignalAlarm & defSignalGroupCON_Alarm_Temp_War) != 0 ||
                    (pEVSE->status.ulSignalAlarm & defSignalGroupEVSE_Alarm_Temp_War) != 0)
                {
                    pCON->status.SetLoadPercent(pCON, 50);
                }
                //温度\电流警告（非严重告警）、电压过欠压算正常，不进行处理
                if ((pCON->status.ulSignalAlarm & ~defSignalGroupCON_Alarm_DontCare) != 0 ||
                    pCON->status.ulSignalFault != 0 ||
                   (pEVSE->status.ulSignalAlarm & ~defSignalGroupEVSE_Alarm_Temp_War) != 0 ||
                    pEVSE->status.ulSignalFault != 0)
                {
                    //其他异常清除认证标志
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
            }
            if ((pCON->status.ulSignalFault & defSignalCON_Fault_Meter) == defSignalCON_Fault_Meter)
            {
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionMeter);
            }
            else
            {
                xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionMeter);
            }
            
            if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)
            {
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFID);
            }
            else
            {
                xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRFID);
            }
        }

        /* 诊断各状态 CONTemp和ChargingData在Monitor的单独任务中,因此需要单独判断每个CON的diag标志 */

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagTemp, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagTemp) == defEventBitDiagTemp)
        {
                DiagEVSETempError(pEVSE);
        }

        for (i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsDiag = xEventGroupWaitBits(pCON->status.xHandleEventDiag, defEventBitDiagTemp, pdTRUE, pdFALSE, 0);
            if ((uxBitsDiag & defEventBitDiagTemp) == defEventBitDiagTemp)
            {
                DiagCONTempError(pCON);
            }
        }
        
        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagLockState, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagLockState) == defEventBitDiagLockState)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                DiagLockError(pCON);
            }
        }

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagPlugState, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagPlugState) == defEventBitDiagPlugState)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                DiagPlugError(pCON);
            }
        }

        for (i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsDiag = xEventGroupWaitBits(pCON->status.xHandleEventDiag, defEventBitDiagChargingData, pdTRUE, pdFALSE, 0);
            if ((uxBitsDiag & defEventBitDiagChargingData) == defEventBitDiagChargingData)
            {
                DiagVoltageError(pCON);
                DiagCurrentError(pCON);
                DiagFreqError(pCON);
            }
        }

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagEVSEState, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagEVSEState) == defEventBitDiagEVSEState)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                DiagEVSEError(pCON);//EVSE启动相关条件判断
            }
        }
        /* end of 判断状态 */
#endif
        vTaskDelay(10);
    }
}
