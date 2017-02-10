/**
* @file taskmonitor.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"

void vTaskEVSEMonitor(void *pvParameters)
{
    double dChargingVoltage, dChargingCurrent;
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray++);
    }
    pPoint[0]->status.uiCPState = 1;
    while(1)
    {
        //dChargingVoltage = GetChargingVoltage(((ChargePoint_t *)(pListChargePoint->pListPointArray[0])));
        //dChargingCurrent = GetChargingCurrent(((ChargePoint_t *)(pListChargePoint->pListPointArray[0])));
        //≤Â«πºÏ≤‚
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(50);
    }
}
