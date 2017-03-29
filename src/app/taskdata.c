/**
* @file taskdata.c
* @brief 配置文件读写、订单操作
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
    EventBits_t uxBitsData;
    EventBits_t uxBitsCharge;

    ulTotalCON = pListCON->Total;
    uxBitsTimer = 0;
    uxBitsData = 0;
    uxBitsCharge = 0;
    THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING);//创建order.txt
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        /* 订单管理 */
        //1. 等待刷卡完成事件
        uxBitsData = xEventGroupWaitBits(xHandleEventData,
                                         defEventBitOrderTmp,   //RFID中发出该事件
                                         pdTRUE, pdFALSE, 0);
        if((uxBitsData & defEventBitOrderTmp) == defEventBitOrderTmp)
        {
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            pCON->order.state = STATE_ORDER_TMP;
        }


        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            switch(pCON->order.state)
            {
            case STATE_ORDER_IDLE:
                break;
            case STATE_ORDER_TMP:
                makeOrder(pCON);
                xEventGroupSetBits(xHandleEventData, defEventBitOrderUpdateOK);
                pCON->order.state = STATE_ORDER_WAITSTART;
                break;
            case STATE_ORDER_WAITSTART:
                //2. 等待StartCharge事件
                for(i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                       defEventBitCONStartOK,
                                                       pdFALSE, pdFALSE, 0);
                    if((uxBitsCharge & defEventBitCONStartOK) == defEventBitCONStartOK)
                    {
                        pCON->order.state = STATE_ORDER_MAKE;
                    }
                }
                break;
            case STATE_ORDER_MAKE:
                //3. 开始时的数据准备
                makeOrder(pCON);
                xEventGroupSetBits(xHandleEventData, defEventBitOrderMakeOK);//目前还没有地方用
                pCON->order.state = STATE_ORDER_UPDATE;
                break;
            case STATE_ORDER_UPDATE:
                //4. 更新充电数据
            case STATE_ORDER_FINISH:
                //5. 结束充电
                break;
            }
        }


        /* 读取文件配置 */
        uxBitsTimer = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                          defEventBitTimerCBDataRefresh,
                                          pdTRUE, pdFALSE, 0);
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
