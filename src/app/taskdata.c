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
#include "log_evse.h"
#include "cfg_order.h"

//#define DEBUG_NO_TASKDATA

extern ErrorCode_t makeOrder(CON_t *pCON);

void vTaskEVSEData(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int id, i;
    uint32_t ulSignalPoolXor;
    uint32_t ulSignalCONAlarmOld_CON[defMaxCON] = { 0 };
    uint32_t ulSignalCONFaultOld_CON[defMaxCON] = { 0 };
    uint32_t ulSignalEVSEAlarmOld = 0;
    uint32_t ulSignalEVSEFaultOld = 0;
    EventBits_t uxBitsTimer;
    EventBits_t uxBitsData;
    EventBits_t uxBitsCharge;

    ulTotalCON = pListCON->Total;
    ulSignalPoolXor = 0;
    uxBitsTimer = 0;
    uxBitsData = 0;
    uxBitsCharge = 0;
    if (ulTotalCON > defMaxCON)
    {
        while (1)
            ;//你看, 你设置的ulSignalCONAlarmOld_CON 数组小了
    }
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        /************ 订单管理 *******************/
        //1. 等待刷卡完成事件
        for(id = 0; id < ulTotalCON; id++)
        {
            pCON = CONGetHandle(id);
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

        for(id = 0; id < ulTotalCON; id++)
        {
            pCON = CONGetHandle(id);
            switch(pCON->order.statOrder)
            {
            case STATE_ORDER_IDLE:
                break;
            case STATE_ORDER_TMP:
                makeOrder(pCON);
                pCON->order.statRemoteProc.card.stat = CARDCTRL_WAIT_START;
                pCON->order.statRemoteProc.card.timestamp = time(NULL);
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
                xTimerStart(pCON->OrderTmp.xHandleTimerOrderTmp, 100);
                makeOrder(pCON);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeOK);//充电前数据准备完成, Clear in proto
                pCON->order.statOrder = STATE_ORDER_UPDATE;
                break;
            case STATE_ORDER_UPDATE:
                /** 等待充电停止，进入Finish状态 */
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if ((uxBitsCharge & defEventBitCONStartOK) != defEventBitCONStartOK)
                {
                    xTimerStop(pCON->OrderTmp.xHandleTimerOrderTmp, 100);
                    pCON->order.statOrder = STATE_ORDER_FINISH;
                }
                //4. 更新充电数据
                makeOrder(pCON);
                uxBitsTimer = xEventGroupWaitBits(pCON->status.xHandleEventOrder, 
                                                  defEventBitOrderTmpTimer, 
                                                  pdTRUE, pdTRUE, 0);
                if ((uxBitsTimer & defEventBitOrderTmpTimer) == defEventBitOrderTmpTimer)
                {
                    AddOrderTmp(pCON->OrderTmp.strOrderTmpPath, &(pCON->order), pechProto);
                }
                
                /*金额不足*/
                if (pCON->order.dTotalFee >= pCON->order.dBalance)
                {
                    xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitFee);
                    pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                    break;
                }
                /*总时间限制*/
                if ((time(NULL) - pCON->order.tStartTime) < 85800)//(24 * 3600 - 600) //充电时间快达到24小时时, 会提前10分钟断电结费.
                {
                    /****电量判断****/
                    if (pCON->order.dLimitPower != 0) //0 时表示自动充满，非0即停止电量 
                    {
                        if (pCON->order.dTotalPower >= pCON->order.dLimitPower) // 达到充电电量
                        {
                            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitPower);
                            pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                            break;
                        }
                    }
                    /****金额判断****/
                    else if (pCON->order.dLimitFee != 0) //0 时表示自动充满，非0即停止金额
                    {
                        if (pCON->order.dTotalFee >= pCON->order.dLimitFee) // 达到充电金额
                        {
                            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitFee);
                            pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                            break;
                        }
                    }
                    //****时间判断***  
                    else if (pCON->order.ulLimitTime != 0) //0表示自动充满 非0表示设定时间
                    {
                        if ((time(NULL) - pCON->order.tStartTime) >= pCON->order.ulLimitTime)//达到或超过设定时间
                        {
                            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitTime);
                            pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                            break;
                        }
                    }
                }
                else
                {
                    xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitTime);
                    pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                    break;
                }
                break;
            case STATE_ORDER_WAITSTOP:
                /** 等待充电停止，进入Finish状态 */
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if ((uxBitsCharge & defEventBitCONStartOK) != defEventBitCONStartOK)
                {
                    xTimerStop(pCON->OrderTmp.xHandleTimerOrderTmp, 100);
                    pCON->order.statOrder = STATE_ORDER_FINISH;
                }
                break;
            case STATE_ORDER_FINISH:
                //5. 结束充电
                makeOrder(pCON);
	            xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeOK);
                /************ make user happy, but boss and i are not happy :( ************/
                if (pCON->order.dTotalFee >= pCON->order.dBalance)
                {
                    pCON->order.dTotalFee = pCON->order.dBalance;
                }
                if (pCON->order.dLimitPower != 0)
                {
                    if (pCON->order.dTotalPower > pCON->order.dLimitPower)
                    {
                        pCON->order.dTotalPower = pCON->order.dLimitPower;
                    }
                }
                else if(pCON->order.dLimitFee != 0)
                {
                    if(pCON->order.dTotalFee > pCON->order.dLimitFee)
                    {
                        pCON->order.dTotalServFee = pCON->order.dLimitFee - pCON->order.dTotalPowerFee;
                        pCON->order.dTotalFee = pCON->order.dLimitFee;
                    }
                }
                else if (pCON->order.ulLimitTime != 0)
                {
                    if (pCON->order.tStopTime - pCON->order.tStartTime > pCON->order.ulLimitTime)
                    {
                        pCON->order.tStopTime = (time_t)(pCON->order.tStartTime + pCON->order.ulLimitTime);
                    }
                }
                /***************************************** :) *******/
                /***判断停止类型***/
                uxBitsData = xEventGroupGetBits(pCON->status.xHandleEventOrder);
                
                //达到充电电量限制
                if ((uxBitsData & defEventBitOrderStopTypeLimitPower) == defEventBitOrderStopTypeLimitPower)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitPower);
                    pCON->order.ucStopType = defOrderStopType_Power;
                }
                
                //达到充电金额限制
                if((uxBitsData & defEventBitOrderStopTypeLimitFee) == defEventBitOrderStopTypeLimitFee)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitFee);
                    pCON->order.ucStopType = defOrderStopType_Fee;
                }
                
                //达到充电时间限制
                if ((uxBitsData & defEventBitOrderStopTypeLimitTime) == defEventBitOrderStopTypeLimitTime)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitTime);
                    pCON->order.ucStopType = defOrderStopType_Time;
                }
                
                //远程停止
                if((uxBitsData & defEventBitOrderStopTypeRemoteStop) == defEventBitOrderStopTypeRemoteStop)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRemoteStop);
                    pCON->order.ucStopType = defOrderStopType_Remote;
                }
                
                //刷卡停止
                if((uxBitsData & defEventBitOrderStopTypeRFIDStop) == defEventBitOrderStopTypeRFIDStop)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRFIDStop);
                    pCON->order.ucStopType = defOrderStopType_RFID;
                }
                
                //自动充满
                if((uxBitsData & defEventBitOrderStopTypeFull) == defEventBitOrderStopTypeFull)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeFull);
                    pCON->order.ucStopType = defOrderStopType_Full;
                }
                
                //用户强制拔枪
                if ((uxBitsData & defEventBitOrderStopTypeUnPlug) == defEventBitOrderStopTypeUnPlug)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeUnPlug);
                    pCON->order.ucStopType = defOrderStopType_UnPlug;
                }
                
                //过流
                if ((uxBitsData & defEventBitOrderStopTypeCurr) == defEventBitOrderStopTypeCurr)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeCurr);
                    pCON->order.ucStopType = defOrderStopType_OverCurr;
                }
                
                //急停
                if ((uxBitsData & defEventBitOrderStopTypeScram) == defEventBitOrderStopTypeScram)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeScram);
                    pCON->order.ucStopType = defOrderStopType_Scram;
                }
                AddOrderTmp(pCON->OrderTmp.strOrderTmpPath, &(pCON->order), pechProto);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);

                xQueueSend(xHandleQueueOrders, &(pCON->order), 0);
                
                /**存储订单 */
#if EVSE_USING_NET
#else
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderUseless);   
#endif
                uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                 defEventBitOrderUseless,
                                                 pdTRUE, pdTRUE, 65000);//要比remote中的order超时（60s）长
	            if ((uxBitsData & defEventBitOrderUseless) == defEventBitOrderUseless)
	            {
    	            printf_safe("Order OK.....................\n");
    	            pCON->order.ucPayStatus = 1;
    	            RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
	            }
	            else
	            {
    	            printf_safe("Order TimeOut.....................\n");
    	            pCON->order.ucPayStatus = 0;
		            xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
					/* 临时订单 在这里存储订单*/
    	            AddOrderTmp(pCON->OrderTmp.strOrderTmpPath, &(pCON->order), pechProto);
	            }
                AddOrderCfg(pathOrder, &(pCON->order), pechProto); //存储订单
                xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderFinishToHMI);
                pCON->order.statOrder = STATE_ORDER_HOLD;
                break;
            case STATE_ORDER_HOLD:
                if ((pCON->status.ulSignalState & defSignalCON_State_Plug) == defSignalCON_State_Plug)
                {
                    break;
                }
                else
                {
                    pCON->order.statOrder = STATE_ORDER_RETURN;
                }
                break;
            case STATE_ORDER_RETURN:
                OrderInit(&(pCON->order));//状态变为IDLE
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

            for(id = 0; id < ulTotalCON; id++)
            {
                pCON = CONGetHandle(id);
                THROW_ERROR(defDevID_File, pCON->info.GetCONCfg(pCON, NULL), ERR_LEVEL_WARNING, "taskdata GetCONCfg");
            }
        }
        /********** end of 读取文件配置 **************/

        /********** 更新密钥 **************/
        if(pechProto->info.tNewKeyChangeTime <= time(NULL))
        {
            //32位系统最大时间戳2147483647 -> 2038/1/19 11:14:7
            uint32_t max_time = 2147483647;

            cfg_set_string(pathProtoCfg, pechProto->info.strNewKey, "%s", jnProtoKey);
            cfg_set_uint32(pathProtoCfg, &max_time, "%s", jnProtoNewKeyChangeTime);
        }
        
        /********** 告警记录 **************/
#if EVSE_USING_STORE_LOG
        for (id = 0; id < ulTotalCON; id++)
        {
            pCON = CONGetHandle(id);
            //proc con alarm
            ulSignalPoolXor = ulSignalCONAlarmOld_CON[id] ^ pCON->status.ulSignalAlarm;
            if (ulSignalPoolXor != 0)
            {
                for (i = 0; i < 32; i++)//ulSignalPoolXor = 32bit
                {
                    if ((ulSignalPoolXor & (1 << i)) == (1 << i))
                    {
                        switch (1 << i)
                        {
                        case defSignalCON_Alarm_SocketLock:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "枪锁");
                            break;
                        case defSignalCON_Alarm_SocketTemp1_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度1");
                            break;
                        case defSignalCON_Alarm_SocketTemp2_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度2");
                            break;
                        case defSignalCON_Alarm_SocketTemp1_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度1");
                            break;
                        case defSignalCON_Alarm_SocketTemp2_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度2");
                            break;
                        case defSignalCON_Alarm_AC_A_Temp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相温度");
                            break;
                        case defSignalCON_Alarm_AC_B_Temp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相温度");
                            break;
                        case defSignalCON_Alarm_AC_C_Temp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相温度");
                            break;
                        case defSignalCON_Alarm_AC_N_Temp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "N相温度");
                            break;
                        case defSignalCON_Alarm_AC_A_Temp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相温度");
                            break;
                        case defSignalCON_Alarm_AC_B_Temp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "B相温度");
                            break;
                        case defSignalCON_Alarm_AC_C_Temp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "C相温度");
                            break;
                        case defSignalCON_Alarm_AC_N_Temp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "N相温度");
                            break;
                        case defSignalCON_Alarm_AC_A_VoltUp:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电压过压");
                            break;
                        case defSignalCON_Alarm_AC_B_VoltUp:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相电压过压");
                            break;
                        case defSignalCON_Alarm_AC_C_VoltUp:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相电压过压");
                            break;
                        case defSignalCON_Alarm_AC_A_VoltLow:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电压欠压");
                            break;
                        case defSignalCON_Alarm_AC_B_VoltLow:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相电压欠压");
                            break;
                        case defSignalCON_Alarm_AC_C_VoltLow:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相电压欠压");
                            break;
                        case defSignalCON_Alarm_AC_A_CurrUp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_B_CurrUp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_C_CurrUp_War:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_A_CurrUp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_B_CurrUp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "B相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_C_CurrUp_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "C相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_Freq_Cri:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "频率");
                            break;
                        default:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelWarning, 1, "充电枪未知警告");
                            break;
                            
                        }
                    }
                }
            }// if (ulSignalPoolXor != 0)
            ulSignalCONAlarmOld_CON[id] = pCON->status.ulSignalAlarm;   //别忘了给old赋值, 要不下次进来没法检测差异哦 :)
            
            //proc con fault
            ulSignalPoolXor = ulSignalCONFaultOld_CON[id] ^ pCON->status.ulSignalFault;
            if (ulSignalPoolXor != 0)
            {
                for (i = 0; i < 32; i++)
                {
                    if ((ulSignalPoolXor & (1 << i)) == (1 << i))
                    {
                        switch (1 << i)
                        {
                        case defSignalCON_Fault_SocketLock: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "枪锁");
                            break;
                        case defSignalCON_Fault_AC_A_Temp: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "A(L)相温度检测");
                            break;
                        case defSignalCON_Fault_AC_B_Temp: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "B相温度检测");
                            break;
                        case defSignalCON_Fault_AC_C_Temp: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "C相温度检测");
                            break;
                        case defSignalCON_Fault_AC_N_Temp: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "N相温度检测");
                            break;
                        case defSignalCON_Fault_RelayPaste: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "继电器粘连");
                            break;
                        case defSignalCON_Fault_CP: 
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "CP检测");
                            break;
                        case defSignalCON_Fault_Plug:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "插枪检测");
                            break;
                        case defSignalCON_Fault_Meter:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "电能计量故障");
                            break;
                        default:
                            AddEVSELog(pathEVSELog, id + 1, defLogLevelFault, 1, "充电枪未知故障");
                            break;
                        }
                    }
                }
            }//if (ulSignalPoolXor != 0)
            ulSignalCONFaultOld_CON[id] = pCON->status.ulSignalFault;   //别忘了给old赋值, 要不下次进来没法检测差异哦 :)
        }// id
        
        //proc evse alarm
        ulSignalPoolXor = ulSignalEVSEAlarmOld ^ pEVSE->status.ulSignalAlarm;
        if (ulSignalPoolXor != 0)
        {
            for (i = 0; i < 32; i++)
            {
                if ((ulSignalPoolXor & (1 << i)) == (1 << i))
                {
                    switch (1 << i)
                    {
                    case defSignalEVSE_Alarm_Scram: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "急停");
                        break;
                    case defSignalEVSE_Alarm_Knock: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "撞击");
                        break;
                    case defSignalEVSE_Alarm_PE: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "接地");
                        break;
                    case defSignalEVSE_Alarm_PowerOff: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "掉电");
                        break;
                    case defSignalEVSE_Alarm_Arrester: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "防雷");
                        break;
                    case defSignalEVSE_Alarm_AC_A_Temp_War: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电A(L)相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_B_Temp_War: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelWarning, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电B相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_C_Temp_War: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelWarning, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电C相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_N_Temp_War: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelWarning, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电N相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_A_Temp_Cri: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电A(L)相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_B_Temp_Cri: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电B相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_C_Temp_Cri: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电C相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_N_Temp_Cri: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电N相过温");
                        break;
                    default:
                        AddEVSELog(pathEVSELog, 0, defLogLevelCritical, 1, "EVSE未知警告");
                        break;
                    }
                }
            }
        }//if (ulSignalPoolXor != 0)
        ulSignalEVSEAlarmOld = pEVSE->status.ulSignalAlarm;   //别忘了给old赋值, 要不下次进来没法检测差异哦 :)
        
        //proc evse fault
        ulSignalPoolXor = ulSignalEVSEFaultOld ^ pEVSE->status.ulSignalFault;
        if (ulSignalPoolXor != 0)
        {
            for (i = 0; i < 32; i++)
            {
                if ((ulSignalPoolXor & (1 << i)) == (1 << i))
                {
                    switch (1 << i)
                    {
                    case defSignalEVSE_Fault_RFID: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "读卡器");
                        break;
                    case defSignalEVSE_Fault_Bluetooth: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "蓝牙");
                        break;
                    case defSignalEVSE_Fault_Wifi: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "WI-FI");
                        break;
                    case defSignalEVSE_Fault_GPRS: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "GPRS");
                        break;
                    case defSignalEVSE_Fault_GSensor: 
                        AddEVSELog(pathEVSELog, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "加速度传感器");
                        break;
                    default:
                        AddEVSELog(pathEVSELog, 0, defLogLevelFault, 1, "EVSE未知故障");
                        break;
                    }
                }
            }
        }//if (ulSignalPoolXor != 0)
        ulSignalEVSEFaultOld = pEVSE->status.ulSignalFault;   //别忘了给old赋值, 要不下次进来没法检测差异哦 :)
#endif //if 1
        
#endif //DEBUG_NO_TASKDATA
        vTaskDelay(100);
    }
}
