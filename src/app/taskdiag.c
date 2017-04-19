/**
* @file taskdiag.c
* @brief ϵͳ�������
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
        /* ����ϵͳʧЧ���� */
        xResult = xQueueReceive(xHandleQueueErrorPackage, &errpack, 0);
        if(xResult == pdTRUE)
        {
#ifdef DEBUG_DIAG
            printf_safe("%x %s(code: %d,level: %d)\n", errpack.ulDevID, strErrorCode[errpack.code], errpack.code, errpack.level);
#endif
        }
        /* end of ����ϵͳʧЧ���� */

        /* ����ϵͳ���� */
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsException = xEventGroupWaitBits(pCON->status.xHandleEventException, defEventBitExceptionTempW, pdTRUE, pdFALSE, 0);
            if((uxBitsException & defEventBitExceptionTempW) == defEventBitExceptionTempW)
            {
                pCON->status.SetLoadPercent(pCON, 50);
            }
        }
        /* end of ����ϵͳ���� */

        /* ��ϸ�״̬ */

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
                DiagEVSEError(pCON);//EVSE������������ж�
            }
        }
        /* end of �ж�״̬ */

#if DEBUG_DIAG
        //printf_safe("%s\n", TASKNAME_EVSEDiag);
#endif
#endif
        vTaskDelay(10);
    }
}
