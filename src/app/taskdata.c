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
#include "cfg_parse.h"
#include "stringName.h"

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
    //THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING, "<taskdata> Create Order.txt");//创建order.txt
//    OrderDBCreate();
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        /************ 订单管理 *******************/
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
                pCON->status.statRemoteProc.card.stat = CARDCTRL_WAIT_START;
                pCON->status.statRemoteProc.card.timestamp = time(NULL);
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
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeOK);//充电前数据准备完成, Clear in proto
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
                    /* (rgw#1): 在这里存储订单*/
                    //OrderDBInsertItem(&(pCON->order));
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderFinishToRemote);
                    xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONOrderFinish);
                    OrderInit(&(pCON->order));//状态变为IDLE
                }
//                uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
//                                                 defEventBitOrder_HMIDispOK,
//                                                 pdTRUE, pdTRUE, 0);
//                if((uxBitsData & defEventBitOrder_HMIDispOK) == defEventBitOrder_HMIDispOK)
//                {
//                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
//                    /* @todo (rgw#1): 在这里存储订单*/
//                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderFinishToRemote);
//                    xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONOrderFinish);
//                    OrderInit(&(pCON->order));//状态变为IDLE
//                }

                break;
            }
        }//for CONid


        /********** 读取文件配置 ***************/
        uxBitsTimer = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                          defEventBitTimerCBDataRefresh,
                                          pdTRUE, pdFALSE, 0);
        if((uxBitsTimer & defEventBitTimerCBDataRefresh) == defEventBitTimerCBDataRefresh)
        {
            THROW_ERROR(defDevID_File, pEVSE->info.GetEVSECfg(pEVSE, NULL), ERR_LEVEL_WARNING, "taskdata GetEVSECfg");
            THROW_ERROR(defDevID_File, pechProto->info.GetProtoCfg(pechProto, NULL), ERR_LEVEL_WARNING, "taskdata GetProtoCfg");

            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                THROW_ERROR(defDevID_File, pCON->info.GetCONCfg(pCON, NULL), ERR_LEVEL_WARNING, "taskdata GetCONCfg");
            }
        }
        /********** end of 读取文件配置 **************/

//        uxBits = xEventGroupWaitBits(xHandleEventData, defEventBitAddOrder, pdTRUE, pdFALSE, 0);
//        if((uxBits & defEventBitAddOrder) == defEventBitAddOrder)
//        {
//            DataAddOrder();
//            xEventGroupSetBits(xHandleEventData, defEventBitAddOrderOK);
//        }
        /********** 更新密钥 **************/
        if(pechProto->info.tNewKeyChangeTime <= time(NULL))
        {
            //32位系统最大时间戳4294967295
            uint32_t max_time = 4294967295;

            pechProto->info.SetProtoCfg(jnProtoKey, ParamTypeString, NULL, 0, pechProto->info.strNewKey);
            pechProto->info.SetProtoCfg(jnProtoNewKeyChangeTime, ParamTypeU32, NULL, 0, &max_time);
        }
        
        /********** 告警记录 **************/
        uint32_t ulSignalOld;
        for (i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            ulSignalOld = pCON->status.ulSignalAlarm;
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketLock) == defSignalCON_Alarm_SocketLock)
            {
                AddEVSELog(pathEVSELog, 1, 1, "枪锁");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "枪锁");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp1_War) == defSignalCON_Alarm_SocketTemp1_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "枪座温度1");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "枪座温度1");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp2_War) == defSignalCON_Alarm_SocketTemp2_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "枪座温度2");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "枪座温度2");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp1_Cri) == defSignalCON_Alarm_SocketTemp1_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "枪座温度1");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "枪座温度1");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp2_Cri) == defSignalCON_Alarm_SocketTemp2_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "枪座温度2");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "枪座温度2");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_Temp_War) == defSignalCON_Alarm_AC_A_Temp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "A相温度(L)");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "A相温度(L)");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_B_Temp_War) == defSignalCON_Alarm_AC_B_Temp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "B相温度");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "B相温度");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_C_Temp_War) == defSignalCON_Alarm_AC_C_Temp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "C相温度");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "C相温度");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_N_Temp_War) == defSignalCON_Alarm_AC_N_Temp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "N相温度");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "N相温度");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_Temp_Cri) == defSignalCON_Alarm_AC_A_Temp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "A相温度(L)");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "A相温度(L)");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_B_Temp_Cri) == defSignalCON_Alarm_AC_B_Temp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "B相温度");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "B相温度");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_C_Temp_Cri) == defSignalCON_Alarm_AC_C_Temp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "C相温度");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "C相温度");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_N_Temp_Cri) == defSignalCON_Alarm_AC_N_Temp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "N相温度");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "N相温度");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp)
            {
                AddEVSELog(pathEVSELog, 1, 1, "A相电压过压");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "A相电压过压");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_B_VoltUp) == defSignalCON_Alarm_AC_B_VoltUp)
            {
                AddEVSELog(pathEVSELog, 1, 1, "B相电压过压");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "B相电压过压");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_C_VoltUp) == defSignalCON_Alarm_AC_C_VoltUp)
            {
                AddEVSELog(pathEVSELog, 1, 1, "C相电压过压");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "C相电压过压");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow)
            {
                AddEVSELog(pathEVSELog, 1, 1, "A相电压欠压");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "A相电压欠压");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_B_VoltLow) == defSignalCON_Alarm_AC_B_VoltLow)
            {
                AddEVSELog(pathEVSELog, 1, 1, "B相电压欠压");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "B相电压欠压");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_C_VoltLow) == defSignalCON_Alarm_AC_C_VoltLow)
            {
                AddEVSELog(pathEVSELog, 1, 1, "C相电压欠压");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "C相电压欠压");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_War) == defSignalCON_Alarm_AC_A_CurrUp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "A相电流过流");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "A相电流过流");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_B_CurrUp_War) == defSignalCON_Alarm_AC_B_CurrUp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "B相电流过流");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "B相电流过流");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_C_CurrUp_War) == defSignalCON_Alarm_AC_C_CurrUp_War)
            {
                AddEVSELog(pathEVSELog, 1, 1, "C相电流过流");
            }
            else
            {
                AddEVSELog(pathEVSELog, 1, 0, "C相电流过流");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_Cri) == defSignalCON_Alarm_AC_A_CurrUp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "A相电流过流");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "A相电流过流");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_B_CurrUp_Cri) == defSignalCON_Alarm_AC_B_CurrUp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "B相电流过流");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "B相电流过流");   
            }
            if ((pCON->status.ulSignalAlarm & defSignalCON_Alarm_AC_C_CurrUp_Cri) == defSignalCON_Alarm_AC_C_CurrUp_Cri)
            {
                AddEVSELog(pathEVSELog, 2, 1, "C相电流过流");
            }
            else
            {
                AddEVSELog(pathEVSELog, 2, 0, "C相电流过流");   
            }

            }
        }

#if DEBUG_DATA

#endif
#endif //DEBUG_NO_TASKDATA
        vTaskDelay(100);
    }
}
