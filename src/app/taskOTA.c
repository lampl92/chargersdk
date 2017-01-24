/**
* @file taskOTA.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskOTA.h"
void vTaskOTA(void *pvParameters)
{
    while(1)
    {
        //xprintf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}
