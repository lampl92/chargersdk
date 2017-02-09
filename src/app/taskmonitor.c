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

    ((ChargePoint_t *)(pListChargePoint->pListPointArray[0]))->uiCPState = 1;
    while(1)
    {
        dChargingVoltage = GetChargingVoltage();
        dChargingCurrent = GetChargingCurrent();
        //≤Â«πºÏ≤‚

#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(1000);
    }
}
