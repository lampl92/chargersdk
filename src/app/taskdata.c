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
        //printf("%s\n", TASKNAME_EVSEData);
        vTaskDelay(1000);
    }
}
