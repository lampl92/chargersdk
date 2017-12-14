/**
* @file taskOTA.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "taskcreate.h"
#include "taskOTA.h"
#include  "interface.h"
void vTaskOTA(void *pvParameters)
{
    while(1)
    {
//        CON_t *pCON;
//        pCON = CONGetHandle(0);
//        pCON->status.SetRelay(pCON, SWITCH_OFF);
//        vTaskDelay(1000);
//        pCON->status.SetRelay(pCON, SWITCH_ON);
        
        IWDG_Feed();
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_OTA);
#endif
        vTaskDelay(1000);
    }
}
