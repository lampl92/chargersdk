/**
* @file taskerror.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskerror.h"

void vTaskEVSEError(void *pvParameters)
{
    BaseType_t xResult;
    ErrorCode_t errcode;

    xResult = pdFALSE;
    errcode = ERR_NO;
    while(1)
    {
        xResult = xQueueReceive(xHandleQueueErrorCode, &errcode, 50);
        if(xResult == pdTRUE)
        {
            printf_safe("error = %d:%s", errcode, strErrorCode[errcode]);
        }

#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEError);
#endif
//        vTaskDelay(1000);
    }
}
