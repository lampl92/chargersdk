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
    ChargePoint_t *pPoint = NULL;
    uint32_t ulTotalPoint;
    int i;
    EventBits_t uxBitsTimer;

    ulTotalPoint = pListChargePoint->Total;
    uxBitsTimer = 0;

    THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING);//创建order.txt
    while(1)
    {
        /* 读取文件配置 */
        uxBitsTimer = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBDataRefresh, pdTRUE, pdFALSE, 0);
        if((uxBitsTimer & defEventBitTimerCBDataRefresh) == defEventBitTimerCBDataRefresh)
        {
            THROW_ERROR(defDevID_File, pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetTotalPoint(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);
            for(i = 0; i < ulTotalPoint; i++)
            {
                pPoint = ChargePointGetHandle(i);
                THROW_ERROR(defDevID_File, pPoint->info.GetConnectorType(pPoint), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pPoint->info.GetVolatageUpperLimits(pPoint), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pPoint->info.GetVolatageLowerLimits(pPoint), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pPoint->info.GetRatedCurrent(pPoint), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pPoint->info.GetRatedPower(pPoint), ERR_LEVEL_WARNING);
            }
        }
        /* end of 读取文件配置 */

//        uxBits = xEventGroupWaitBits(xHandleEventData, defEventBitAddOrder, pdTRUE, pdFALSE, 0);
//        if((uxBits & defEventBitAddOrder) == defEventBitAddOrder)
//        {
//            DataAddOrder();
//            xEventGroupSetBits(xHandleEventData, defEventBitAddOrderOK);
//        }

#if DEBUG_DATA
        //printf_safe("%s\n", TASKNAME_EVSEData);
#endif
        vTaskDelay(100);
    }
}
