/**
* @file taskremote.c
* @brief 远程通信的操作，接收通信实体的指针
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "taskcreate.h"
#include "taskremote.h"

void vTaskEVSERemote(void *pvParameters)
{
    EventBits_t uxBits = 0;
    while(1)
    {
        uxBits = xEventGroupWaitBits(xHandleEventGroupRFID, defEventBitGETUID, pdTRUE, pdTRUE, 100);
        if((uxBits & defEventBitGETUID) != 0)
        {
            xprintf("find card\n");
        }
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSERemote);
#endif
        vTaskDelay(1000);
    }
}
