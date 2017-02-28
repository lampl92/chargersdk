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
    EventBits_t uxBits;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    while(1)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            switch(pPoint[i]->state)
            {
            case POINT_IDLE:
            case POINT_PRECONTRACT:
                uxBits = xEventGroupWaitBits(pPoint[i]->status.xHandleEventStartCondition,
                                             defEventBitStdALL,
                                             pdFALSE, pdTRUE, 0);
                if((uxBits & defEventBitStdALL) == defEventBitStdALL)
                {
                    pPoint[i]->status.StartCharge(pPoint[i]);
                    pPoint[i]->state = POINT_CHARGING;
                    xEventGroupSetBits(pPoint[i]->status.xHandleEventStartCondition, defEventBitStdOK);
                }
                break;
                case POINT_CHARGING:
                break;

            }
        }


#if DEBUG_CHARGE
        xprintf("%s\n", TASKNAME_EVSECharge);
#endif
        vTaskDelay(10);
    }
}
