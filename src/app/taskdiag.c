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
void vTaskEVSEDiag(void *pvParameters)
{
    ChargePoint_t *pPoint = NULL;
    uint32_t ulTotalPoint;
    int i;
    EventBits_t uxBits;
    BaseType_t xResult;
    ErrorPackage_t errpack;

    ulTotalPoint = pListChargePoint->Total;
    uxBits = 0;
    xResult = pdFALSE;

    while(1)
    {
        /* ����ϵͳʧЧ���� */
        xResult = xQueueReceive(xHandleQueueErrorPackage, &errpack, 0);
        if(xResult == pdTRUE && errpack.level == ERR_LEVEL_CRITICAL)
        {
#ifdef DEBUG_DIAG
            printf_safe("%s(code: %d,level: %d)\n", strErrorCode[errpack.code], errpack.code, errpack.level);
#endif
        }
        /* end of ����ϵͳʧЧ���� */

        /* ����ϵͳ���� */
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint = ChargePointGetHandle(i);
            uxBits = xEventGroupWaitBits(pPoint->status.xHandleEventException, defEventBitExceptionTempW, pdTRUE, pdFALSE, 0);
            if((uxBits & defEventBitExceptionTempW) == defEventBitExceptionTempW)
            {
                pPoint->status.SetLoadPercent(pPoint, 50);
            }
        }
        /* end of ����ϵͳ���� */

#if DEBUG_DIAG
        printf_safe("%s\n", TASKNAME_EVSEDiag);
#endif
        vTaskDelay(10);
    }
}
