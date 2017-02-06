/**
* @file taskdata.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskdata.h"

void vTaskEVSEData(void *pvParameters)
{
    while(1)
    {
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEData);
#endif
        vTaskDelay(1000);
    }
}
