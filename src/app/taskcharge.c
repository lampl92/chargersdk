/**
* @file taskcharge.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskcharge.h"

void vTaskEVSECharge(void *pvParameters)
{
    while(1)
    {
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSECharge);
#endif
        vTaskDelay(1000);
    }
}
