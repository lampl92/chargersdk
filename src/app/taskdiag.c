/**
* @file taskerror.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskdiag.h"

void vTaskEVSEDiag(void *pvParameters)
{
    BaseType_t xResult;
    ErrorPackage_t errpack;

    xResult = pdFALSE;

    while(1)
    {
        xResult = xQueueReceive(xHandleQueueErrorPackage, &errpack, 50);
        if(xResult == pdTRUE && errpack.level == ERR_LEVEL_CRITICAL)
        {
            printf_safe("%s(code: %d,level: %d)\n", strErrorCode[errpack.code], errpack.code, errpack.level);
        }

#if DEBUG_DIAG
        printf_safe("%s\n", TASKNAME_EVSEDiag);
#endif
//        vTaskDelay(1000);
    }
}
