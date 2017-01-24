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
        //xprintf("%s\n", TASKNAME_EVSECharge);
        vTaskDelay(1000);
    }
}
