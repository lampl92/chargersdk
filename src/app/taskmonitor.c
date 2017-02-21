/**
* @file taskmonitor.c
* @brief ¼à¿ØÈÎÎñ
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"
void vTaskEVSEMonitor(void *pvParameters)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    uint8_t ucCurrentId;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    ucCurrentId = 0;
    while(1)
    {
        /** ÅÐ¶ÏÇ¹×´Ì¬ */
        do
        {
            if(pPoint[ucCurrentId]->status.xBTypeConnectorLockState == LOCK)
            {

            }
            //if(pPoint[ucCurrentId]->state.dACLTemp)
            ucCurrentId++;
        }while(ucCurrentId < ulTotalPoint);
        ucCurrentId = 0;
        /** end of ÅÐ¶ÏÇ¹×´Ì¬ */

#if DEBUG_TASK
            xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(50);
    }
}
