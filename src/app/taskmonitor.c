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
        //xprintf("%s\n", TASKNAME_EVSEMonitor);
        vTaskDelay(1000);
    }
}
