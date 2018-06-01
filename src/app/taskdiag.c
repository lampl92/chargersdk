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

/**
 * @fn  void vTaskEVSEDiag(void *pvParameters)
 *
 * @brief   Task evse diagram
 *
 * @param [in,out]  pvParameters    If non-null, options for controlling the pv.
 */

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
#ifdef EVSE_DEBUG
            printf_safe("%X %s(code: %d,level: %d)\n", errpack.ulDevID, strErrorCode[errpack.code], errpack.code, errpack.level);
            printf_safe("   %s\n", errpack.msg);
#endif
            switch(errpack.code) // ！！！ 这里一定要仔细查看errpack.ulDevID是否可以用作 CONID， 主要是看设备是否是归属于枪还是桩。 ！！！
            {
            case ERR_RFID_FAULT:
                break;
            case ERR_CON_METER_FAULT:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionMeter);
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
                if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID &&
                    pCON->order.ucStartType == defOrderStartType_Remote)
                {
                    if (pCON->status.ulSignalAlarm != 0 ||
                    pCON->status.ulSignalFault != 0 ||
                    pEVSE->status.ulSignalAlarm != 0 ||
                    (pEVSE->status.ulSignalFault & ~defSignalEVSE_Fault_RFID) != 0)
                    {
                        //其他异常清除认证标志
                        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                    }
                }
                else if (pCON->status.ulSignalAlarm != 0 ||
                    pCON->status.ulSignalFault != 0 ||
                    pEVSE->status.ulSignalAlarm != 0 ||
                    pEVSE->status.ulSignalFault != 0)
                {
                    //其他异常清除认证标志
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
                }
            }//end of if isworking
            if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)
            {
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFID);
            }
            else
            {
                xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRFID);
            }
        }
        
        /* 处理系统报警 */
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException, 
                                                    defEventBitExceptionTempW, 
                                                    pdTRUE, pdFALSE, 0);
            if ((uxBitsException & defEventBitExceptionTempW) == defEventBitExceptionTempW)
            {
                if (pCON->state == STATE_CON_CHARGING)
                {
                    pCON->status.SetLoadPercent(pCON, 70);
                }
            }
        }
        /* end of 处理系统报警 */

        /* 诊断各状态 */

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagTemp, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagTemp) == defEventBitDiagTemp)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                DiagTempError(pCON);
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

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagChargingData, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagChargingData) == defEventBitDiagChargingData)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
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
