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
#ifdef EVSE_DEBUG_ERRLOG
        xResult = xQueueReceive(xHandleQueueErrorPackage, &errpack, 0);
        if(xResult == pdTRUE)
        {
            printf_safe("%X %s(code: %d,level: %d)\n", errpack.ulDevID, strErrorCode[errpack.code], errpack.code, errpack.level);
            printf_safe("   %s\n", errpack.msg);
            switch(errpack.code) // ！！！ 这里一定要仔细查看errpack.ulDevID是否可以用作 CONID， 主要是看设备是否是归属于枪还是桩。 ！！！
            {
            default:
                break;
            }
        }
#endif

        /* end of 处理系统失效故障 */
        
        for (i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            /** 未启动时有故障清除认证标志*/
            if ((pCON->status.ulSignalState & defSignalCON_State_Working) != defSignalCON_State_Working)
            {
                if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID &&
                    pCON->order.ucStartType == defOrderStartType_Remote)
                {
                    if ((pCON->status.ulSignalAlarm & ~defSignalGroupCON_Alarm_Temp_War) != 0 ||
                        pCON->status.ulSignalFault != 0 ||
                        (pEVSE->status.ulSignalAlarm & ~defSignalGroupEVSE_Alarm_Temp_War) != 0 ||
                        (pEVSE->status.ulSignalFault & ~defSignalEVSE_Fault_RFID) != 0)
                    {
                        //其他异常清除认证标志
                        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                    }
                }
                else if ((pCON->status.ulSignalAlarm & ~defSignalGroupCON_Alarm_Temp_War) != 0 ||
                    pCON->status.ulSignalFault != 0 ||
                    (pEVSE->status.ulSignalAlarm & ~defSignalGroupEVSE_Alarm_Temp_War) != 0 ||
                    pEVSE->status.ulSignalFault != 0)
                {
                    //异常清除认证标志
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
            }//end of Not working
            //发生温度告警（max_temp-10 ~ max_temp）
            if ((pCON->status.ulSignalAlarm & defSignalGroupCON_Alarm_Temp_War) != 0 ||
                (pEVSE->status.ulSignalAlarm & defSignalGroupEVSE_Alarm_Temp_War) != 0)
            {
                pCON->status.SetLoadPercent(pCON, 70);
            }
            else
            {
                pCON->status.SetLoadPercent(pCON, 100);
            }
            //发生电流告警
//            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_War) != 0)
//            {
//                pCON->status.SetLoadPercent(pCON, 70);
//            }
//            else
//            {
//                pCON->status.SetLoadPercent(pCON, 100);
//            }
        }//end of for() id

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
        
        for (i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsDiag = xEventGroupWaitBits(pCON->status.xHandleEventDiag, defEventBitDiagLockState, pdTRUE, pdFALSE, 0);
            if ((uxBitsDiag & defEventBitDiagLockState) == defEventBitDiagLockState)
            {
                DiagLockError(pCON);
            }
        }

        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsDiag = xEventGroupWaitBits(pCON->status.xHandleEventDiag, defEventBitDiagPlugState, pdTRUE, pdFALSE, 0);
            if((uxBitsDiag & defEventBitDiagPlugState) == defEventBitDiagPlugState)
            {
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
