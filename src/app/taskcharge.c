/**
* @file taskcharge.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskcharge.h"
#include "interface.h"

void vTaskEVSECharge(void *pvParameters)
{

    while(1)
    {

    if(((ChargePoint_t *)(pListChargePoint->pListPointArray[0]))->uiCPState == 1)
    {
        //printf_safe("CP = 1\n");
    }

        #if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSECharge);
        #endif
        vTaskDelay(10);
    }
}
