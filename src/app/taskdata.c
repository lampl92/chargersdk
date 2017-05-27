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

extern ErrorCode_t makeOrder(CON_t *pCON);

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
    THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING, "<taskdata> Create Order.txt");//创建order.txt
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        /* 订单管理 */
        //1. 等待刷卡完成事件
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                             defEventBitOrderTmp,   //RFID中发出该事件
                                             pdTRUE, pdFALSE, 0);
            if((uxBitsData & defEventBitOrderTmp) == defEventBitOrderTmp)
            {
                pCON->order.ucStartType = 4; //有卡
                pCON = CONGetHandle(pRFIDDev->order.ucCONID);
                pCON->order.statOrder = STATE_ORDER_TMP;
            }
        }
        /** @todo (rgw#1#): !!! 这里没有做扫码启动判断。目前在扫码接受到开启充电处置pCON->order.statOrder = STATE_ORDER_WAITSTART; */

        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            switch(pCON->order.statOrder)
            {
            case STATE_ORDER_IDLE:
                break;
            case STATE_ORDER_TMP:
                makeOrder(pCON);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderUpdateOK);
                pCON->order.statOrder = STATE_ORDER_WAITSTART;
                break;
            case STATE_ORDER_WAITSTART:
                //2. 等待StartCharge事件
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONStartOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitCONStartOK) == defEventBitCONStartOK)
                {
                    pCON->order.statOrder = STATE_ORDER_MAKE;
                }
                break;
            case STATE_ORDER_MAKE:
                //3. 开始充电时数据准备
                makeOrder(pCON);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeOK);//目前还没有地方用
                pCON->order.statOrder = STATE_ORDER_UPDATE;
                break;
            case STATE_ORDER_UPDATE:
                //4. 更新充电数据
                /** 获取离开Update条件，进入Finish状态 */
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONStartOK) != defEventBitCONStartOK)
                {
                    pCON->order.statOrder = STATE_ORDER_FINISH;
                }
                else
                {
                    makeOrder(pCON);
                }

                if(pCON->order.dLimitFee != 0) //0 时表示自动充满，非0即停止金额
                {
                    if(pCON->order.dTotalFee >= pCON->order.dLimitFee) // 达到充电金额
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitFee);
                    }
                }
                break;
            case STATE_ORDER_FINISH:
                //5. 结束充电
                makeOrder(pCON);
                /************ make user happy ************/
                if(pCON->order.dLimitFee != 0)
                {
                    if(pCON->order.dTotalFee > pCON->order.dLimitFee)
                    {
                        pCON->order.dTotalFee = pCON->order.dLimitFee;
                    }
                }
                /*****************************************/
                uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                 defEventBitOrderStopType,
                                                 pdTRUE, pdFALSE, 0);
                if((uxBitsData & defEventBitOrderStopTypeLimitFee) == defEventBitOrderStopTypeLimitFee)    //达到充电金额限制
                {
                    pCON->order.ucStopType = defOrderStopType_Fee;
                }
                if((uxBitsData & defEventBitOrderStopTypeRemoteStop) == defEventBitOrderStopTypeRemoteStop)    //远程停止
                {
                    pCON->order.ucStopType = defOrderStopType_Remote;
                }
                if((uxBitsData & defEventBitOrderStopTypeRFIDStop) == defEventBitOrderStopTypeRFIDStop)    //刷卡停止
                {
                    pCON->order.ucStopType = defOrderStopType_RFID;
                }
                if((uxBitsData & defEventBitOrderStopTypeFull) == defEventBitOrderStopTypeFull)    //自动充满
                {
                    pCON->order.ucStopType = defOrderStopType_Full;
                }
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);

                /** @todo (rgw#1#): 存储订单 */

                uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                 defEventBitOrderUseless,
                                                 pdTRUE, pdTRUE, 0);
                if((uxBitsData & defEventBitOrderUseless) == defEventBitOrderUseless)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
                    /* @todo (rgw#1): 在这里存储订单*/
                    xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONOrderFinish);
                    OrderInit(&(pCON->order));//状态变为IDLE
                }
                break;
            }
        }


        /* 读取文件配置 */
        uxBitsTimer = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                          defEventBitTimerCBDataRefresh,
                                          pdTRUE, pdFALSE, 0);
        if((uxBitsTimer & defEventBitTimerCBDataRefresh) == defEventBitTimerCBDataRefresh)
        {
            THROW_ERROR(defDevID_File, pEVSE->info.GetEVSECfg(pEVSE, NULL), ERR_LEVEL_WARNING, "taskdata GetEVSECfg");

            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(defDevID_File, pCON->info.GetCONCfg(pCON, NULL), ERR_LEVEL_WARNING, "taskdata GetCONCfg");
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
