/**
* @file taskdiag.c
* @brief 系统诊断任务。
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskdiag.h"

void vTaskEVSEDiag(void *pvParameters)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    EventBits_t uxBits;
    BaseType_t xResult;
    ErrorPackage_t errpack;

    xResult = pdFALSE;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }

    while(1)
    {
        /** 处理系统失效故障 */
        xResult = xQueueReceive(xHandleQueueErrorPackage, &errpack, 0);
        if(xResult == pdTRUE && errpack.level == ERR_LEVEL_CRITICAL)
        {
            #ifdef DEBUG_DIAG
            printf_safe("%s(code: %d,level: %d)\n", strErrorCode[errpack.code], errpack.code, errpack.level);
            #endif
        }
        /** end of 处理系统失效故障 */

        /** 处理系统报警 */
        for(i = 0; i < ulTotalPoint; i++)
        {
            uxBits = xEventGroupWaitBits(pPoint[i]->status.xHandleEventException, defEventBitExceptionTempW,pdTRUE,pdFALSE,0);
            if((uxBits & defEventBitExceptionTempW) == defEventBitExceptionTempW)
            {
                pPoint[i]->status.SetLoadPercent(pPoint[i], 50);
            }
        }

        /** end of 处理系统报警 */

#if DEBUG_DIAG
        printf_safe("%s\n", TASKNAME_EVSEDiag);
#endif
        vTaskDelay(10);
    }
}
