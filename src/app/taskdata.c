/**
* @file taskdata.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "includes.h"
#include "taskcreate.h"
#include "taskdata.h"
#include "interface.h"
void vTaskEVSEData(void *pvParameters)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    int i;
    EventBits_t uxBits;

    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    uxBits = 0;
    while(1)
    {
        uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBDataRefresh, pdTRUE, pdFALSE, 0);
        if((uxBits & defEventBitTimerCBDataRefresh) == defEventBitTimerCBDataRefresh)
        {
            THROW_ERROR(pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(pEVSE->info.GetTotalPoint(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);
            for(i = 0; i < pEVSE->info.ucTotalPoint; i++)
            {
                THROW_ERROR(pPoint[i]->info.GetConnectorType(pPoint[i]), ERR_LEVEL_WARNING);
                THROW_ERROR(pPoint[i]->info.GetVolatageUpperLimits(pPoint[i]), ERR_LEVEL_WARNING);
                THROW_ERROR(pPoint[i]->info.GetVolatageLowerLimits(pPoint[i]), ERR_LEVEL_WARNING);
                THROW_ERROR(pPoint[i]->info.GetRatedCurrent(pPoint[i]), ERR_LEVEL_WARNING);
                THROW_ERROR(pPoint[i]->info.GetRatedPower(pPoint[i]), ERR_LEVEL_WARNING);
            }
        }
#if DEBUG_DATA
        printf_safe("%s\n", TASKNAME_EVSEData);
#endif
        vTaskDelay(100);
    }
}
