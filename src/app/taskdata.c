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

//#define DEBUG_NO_TASKDATA

void vTaskEVSEData(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBitsTimer;

    ulTotalCON = pListCON->Total;
    uxBitsTimer = 0;

    THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING);//创建order.txt
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            switch(pCON->state)
            {
            case STATE_CON_IDLE:
            case STATE_CON_PLUGED:
            case STATE_CON_PRECONTRACT:
            case STATE_CON_PRECONTRACT_LOSEPLUG:
            case STATE_CON_STARTCHARGE:
                break;
            case STATE_CON_CHARGING:
                //DataSetOrder(pCON, pEVSE);
            case STATE_CON_STOPCHARGE:
            case STATE_CON_ERROR:
            default:
                break;
            }
        }
        /* 读取文件配置 */
        uxBitsTimer = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBDataRefresh, pdTRUE, pdFALSE, 0);
        if((uxBitsTimer & defEventBitTimerCBDataRefresh) == defEventBitTimerCBDataRefresh)
        {
            THROW_ERROR(defDevID_File, pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetTotalCON(pEVSE), ERR_LEVEL_WARNING);
            THROW_ERROR(defDevID_File, pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(defDevID_File, pCON->info.GetSocketType(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetVolatageUpperLimits(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetVolatageLowerLimits(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetRatedCurrent(pCON), ERR_LEVEL_WARNING);
                THROW_ERROR(defDevID_File, pCON->info.GetRatedPower(pCON), ERR_LEVEL_WARNING);
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

#endif
#endif //DEBUG_NO_TASKDATA
        vTaskDelay(100);
    }
}
