/**
* @file taskmonitor.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"

void vTaskEVSEMonitor(void *pvParameters)
{
    /* ≤Â«πºÏ≤‚*/
    while(1)
    {
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(1000);
    }
}
