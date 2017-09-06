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
#ifdef DEBUG_DIAG
            printf_safe("%X %s(code: %d,level: %d)\n", errpack.ulDevID, strErrorCode[errpack.code], errpack.code, errpack.level);
            printf_safe("   %s\n", errpack.msg);
#endif
            switch(errpack.code) // ！！！ 这里一定要仔细查看errpack.ulDevID是否可以用作 CONID， 主要是看设备是否是归属于枪还是桩。 ！！！
            {
            case ERR_RFID_FAULT:
                for(i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFID);
                }
                break;
            case ERR_CON_METER_FAULT:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionMeter);
                break;
            case ERR_RELAY_PASTE:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRelayPaste);
                break;
            case ERR_CON_CP_FAULT:
                pCON = CONGetHandle(errpack.ulDevID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionCPSwitch);
                break;
            case ERR_CON_ACLTEMP_DECT_FAULT:
            case ERR_CON_ACNTEMP_DECT_FAULT:
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

//        for (i = 0; i < ulTotalCON; i++)
//        {
//            pCON = CONGetHandle(i);
//            pCON->status.
//        }
        
        
        /* 处理系统报警 */
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException, defEventBitExceptionTempW, pdTRUE, pdFALSE, 0);
            if((uxBitsException & defEventBitExceptionTempW) == defEventBitExceptionTempW)
            {
                pCON->status.SetLoadPercent(pCON, 50);
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

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagVolt, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagVolt) == defEventBitDiagVolt)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                DiagVoltageError(pCON);
            }
        }

        uxBitsDiag = xEventGroupWaitBits(xHandleEventDiag, defEventBitDiagChargingData, pdTRUE, pdFALSE, 0);
        if((uxBitsDiag & defEventBitDiagChargingData) == defEventBitDiagChargingData)
        {
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
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

#if DEBUG_DIAG
        //printf_safe("%s\n", TASKNAME_EVSEDiag);
#endif
#endif
        vTaskDelay(10);
    }
}
