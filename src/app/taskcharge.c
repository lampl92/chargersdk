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
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    while(1)
    {
        if(pPoint[0]->state.ulCPState == 1)
        {

            //printf_safe("CP = 1\n");
        }

        #if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSECharge);
        #endif
        vTaskDelay(10);
    }
}
