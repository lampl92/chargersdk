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
cJSON *jsEVSELogObj;
cJSON *jsEVSEOrderObj;
void vTaskEVSEData(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int id, i;
    uint32_t ulSignalPoolXor;
    uint32_t ulSignalCONAlarmOld_CON[defMaxCON] = { 0 };
    uint32_t ulSignalCONFaultOld_CON[defMaxCON] = { 0 };
    uint32_t ulSignalEVSEStateOld = 0;
    uint32_t ulSignalEVSEAlarmOld = 0;
    uint32_t ulSignalEVSEFaultOld = 0;
    EventBits_t uxBitsTimer;
    EventBits_t uxBitsData;
    EventBits_t uxBitsCharge;
    ErrorCode_t errcode;

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
    jsEVSELogObj = GetCfgObj(pathEVSELog, &errcode);
    jsEVSEOrderObj = GetCfgObj(pathOrder, &errcode);
    if (jsEVSELogObj == NULL || jsEVSEOrderObj == NULL)
    {
        while (1)
        {
            vTaskDelay(1000);
        }
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
                pCON->order.ucStartType = defOrderStartType_Card;  //有卡
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
                    break;
                }
                switch (pCON->order.ucStartType)
                {
                case defOrderStartType_Card:
                    if (time(NULL) - pCON->order.statRemoteProc.card.timestamp > defStartChargeTimeOut_s)
                    {
                        pCON->order.statOrder = STATE_ORDER_IDLE;
                        break;
                    }
                    break;
                case defOrderStartType_Remote:
                    if (time(NULL) - pCON->order.statRemoteProc.rmt_ctrl.timestamp > defStartChargeTimeOut_s)
                    {
                        pCON->order.statOrder = STATE_ORDER_IDLE;
                        break;
                    }
                    break;
                default:
                    pCON->order.statOrder = STATE_ORDER_IDLE;
                    break;
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
                if (pCON->order.ucStartType == defOrderStartType_Card)
                {
                    if (pCON->order.dTotalFee + (get_current_totalfee(time(NULL)) * 0.1) >= pCON->order.dBalance)
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitFee);
                        pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                        break;
                    }
                }
                
                /*总时间限制*/
                if ((time(NULL) - pCON->order.tStartTime) < 85800)//(24 * 3600 - 600) //充电时间快达到24小时时, 会提前10分钟断电结费.
                {
                    /****电量判断****/
                    if (pCON->order.dLimitEnergy != 0) //0 时表示自动充满，非0即停止电量 
                    {
                        if (pCON->order.dTotalEnergy >= pCON->order.dLimitEnergy) // 达到充电电量
                        {
                            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitEnergy);
                            pCON->order.statOrder = STATE_ORDER_WAITSTOP;
                            break;
                        }
                    }
                    /****金额判断****/
                    else if (pCON->order.dLimitFee != 0) //0 时表示自动充满，非0即停止金额
                    {
                        if (pCON->order.dTotalFee + (get_current_totalfee(time(NULL)) * 0.1) >= pCON->order.dLimitFee) // 达到充电金额
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
                xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderTimeOut);//防止上次结束订单是因为超时60s强制结束后又产生了TimeOut
                /************ make user happy, but boss and i are not happy :( ************/
                if (pCON->order.ucStartType == defOrderStartType_Card)
                {
                    if (pCON->order.dTotalFee >= pCON->order.dBalance)
                    {
                        pCON->order.dTotalServFee = pCON->order.dBalance - pCON->order.dTotalEnergyFee;
                        pCON->order.dTotalFee = pCON->order.dBalance;
                    }
                }
                if (pCON->order.dLimitEnergy != 0)
                {
                    if (pCON->order.dTotalEnergy > pCON->order.dLimitEnergy)
                    {
                        pCON->order.dTotalEnergy = pCON->order.dLimitEnergy;
                    }
                }
                else if(pCON->order.dLimitFee != 0)
                {
                    if(pCON->order.dTotalFee > pCON->order.dLimitFee)
                    {
                        pCON->order.dTotalServFee = pCON->order.dLimitFee - pCON->order.dTotalEnergyFee;
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
                if ((uxBitsData & defEventBitOrderStopTypeLimitEnergy) == defEventBitOrderStopTypeLimitEnergy)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeLimitEnergy);
                    pCON->order.ucStopType = defOrderStopType_Energy;
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
                //离线
                if ((uxBitsData & defEventBitOrderStopTypeOffline) == defEventBitOrderStopTypeOffline)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeOffline);
                    pCON->order.ucStopType = defOrderStopType_Offline;
                }
                //过温
                if((uxBitsData & defEventBitOrderStopTypeTemp) == defEventBitOrderStopTypeTemp)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeTemp);
                    pCON->order.ucStopType = defOrderStopType_Temp;
                }
                //远程急停
                if((uxBitsData & defEventBitOrderStopTypeRemoteEmergencyStop) == defEventBitOrderStopTypeRemoteEmergencyStop)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderStopTypeRemoteEmergencyStop);
                    pCON->order.ucStopType = defOrderStopType_RemoteEmergencyStop;
                }
                AddOrderTmp(pCON->OrderTmp.strOrderTmpPath, &(pCON->order), pechProto);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinishToRemote);

                //xQueueSend(xHandleQueueOrders, &(pCON->order), 0);
                
                pCON->order.statOrder = STATE_ORDER_WAITUSE;
                break;
            case STATE_ORDER_WAITUSE:
#if EVSE_USING_NET
#else
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderUseless);   
#endif
                if ((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Logined) == defSignalEVSE_State_Network_Logined)
                {
                    uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder, defEventBitOrderUseless, pdTRUE, pdTRUE, 0);
                    if ((uxBitsData & defEventBitOrderUseless) == defEventBitOrderUseless)
                    {
                        printf_safe("CON%d Order OK.....................\n", pCON->info.ucCONID);
                        pCON->order.ucPayStatus = 1;
                        pCON->order.statOrder = STATE_ORDER_STORE;
                        break;
                    }
                    else
                    {
                        if (time(NULL) - pCON->order.tStopTime > 60)//如果协议未返回TimeOut事件，则强制TimeOut
                        {
                            printf_safe("CON%d Order TimeOut force.....................\n", pCON->info.ucCONID);
                            pCON->order.statOrder = STATE_ORDER_STORE;
                            break;
                        }
                        uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderTimeOut, pdTRUE, pdTRUE, 0);
                        if ((uxBitsData & defEventBitOrder_RemoteOrderTimeOut) == defEventBitOrder_RemoteOrderTimeOut)
                        {
                            printf_safe("CON%d Order TimeOut 3times.....................\n", pCON->info.ucCONID);
                            pCON->order.statOrder = STATE_ORDER_STORE;
                            break;
                        }
                    }
                }
                else
                {
                    printf_safe("CON%d Order(NoPay) OK.....................\n", pCON->info.ucCONID);
                    pCON->order.statOrder = STATE_ORDER_STORE;
                    break;
                }
                break;
            case STATE_ORDER_STORE:
                AddOrderCfg(jsEVSEOrderObj, &(pCON->order), pechProto); //存储订单
                RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
                pCON->order.statOrder = STATE_ORDER_HOLD;
                break;
            case STATE_ORDER_HOLD:
#if DEBUG_DIAG_DUMMY
#else
                if ((pCON->status.ulSignalState & defSignalCON_State_Plug) == defSignalCON_State_Plug)
                {
                    break;
                }
                else
#endif
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
            extern RemoteState_t remotestat;
            remotestat = REMOTE_OFFLINE;
        }
        /********** 预约状态 **************/
        /* 0：未知 1：无预约 2：已预约 3：预约失败*/
        for (id = 0; id < ulTotalCON; id++)
        {
            pCON = CONGetHandle(id);
            if (pCON->appoint.status == 2)
            {
                if (time(NULL) - pCON->appoint.timestamp > pCON->appoint.time_s)
                {
                    pCON->appoint.status = 1;
                }
            }
        }

        /********** 状态记录 **************/
        //proc evse state
        ulSignalPoolXor = ulSignalEVSEStateOld ^ pEVSE->status.ulSignalState;
        if (ulSignalPoolXor != 0)
        {
            for (i = 0; i < 32; i++)
            {
                if ((ulSignalPoolXor & (1 << i)) == (1 << i))
                {
                    switch (1 << i)
                    {
                    case defSignalEVSE_State_Network_Logined: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelState, (pEVSE->status.ulSignalState >> i) & 1, "Login");
                        break;
                    default:
                        break;
                    }
                }
            }
        }//if (ulSignalPoolXor != 0)
        ulSignalEVSEStateOld = pEVSE->status.ulSignalState;    //别忘了给old赋值, 要不下次进来没法检测差异哦 :)
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
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "枪锁");
                            break;
                        case defSignalCON_Alarm_SocketTemp1_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度1");
                            break;
                        case defSignalCON_Alarm_SocketTemp2_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度2");
                            break;
                        case defSignalCON_Alarm_SocketTemp1_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度1");
                            break;
                        case defSignalCON_Alarm_SocketTemp2_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "枪座温度2");
                            break;
                        case defSignalCON_Alarm_AC_A_Temp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相温度");
                            break;
                        case defSignalCON_Alarm_AC_B_Temp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相温度");
                            break;
                        case defSignalCON_Alarm_AC_C_Temp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相温度");
                            break;
                        case defSignalCON_Alarm_AC_N_Temp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "N相温度");
                            break;
                        case defSignalCON_Alarm_AC_A_Temp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相温度");
                            break;
                        case defSignalCON_Alarm_AC_B_Temp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "B相温度");
                            break;
                        case defSignalCON_Alarm_AC_C_Temp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "C相温度");
                            break;
                        case defSignalCON_Alarm_AC_N_Temp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "N相温度");
                            break;
                        case defSignalCON_Alarm_AC_A_VoltUp:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电压过压");
                            break;
                        case defSignalCON_Alarm_AC_B_VoltUp:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相电压过压");
                            break;
                        case defSignalCON_Alarm_AC_C_VoltUp:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相电压过压");
                            break;
                        case defSignalCON_Alarm_AC_A_VoltLow:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电压欠压");
                            break;
                        case defSignalCON_Alarm_AC_B_VoltLow:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相电压欠压");
                            break;
                        case defSignalCON_Alarm_AC_C_VoltLow:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相电压欠压");
                            break;
                        case defSignalCON_Alarm_AC_A_CurrUp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_B_CurrUp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "B相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_C_CurrUp_War:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, (pCON->status.ulSignalAlarm >> i) & 1, "C相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_A_CurrUp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "A(L)相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_B_CurrUp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "B相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_C_CurrUp_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "C相电流过流");
                            break;
                        case defSignalCON_Alarm_AC_Freq_Cri:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelCritical, (pCON->status.ulSignalAlarm >> i) & 1, "频率");
                            break;
                        default:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelWarning, 1, "充电枪未知警告");
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
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "枪锁");
                            break;
                        case defSignalCON_Fault_AC_A_Temp: 
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "A(L)相温度检测");
                            break;
                        case defSignalCON_Fault_AC_B_Temp: 
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "B相温度检测");
                            break;
                        case defSignalCON_Fault_AC_C_Temp: 
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "C相温度检测");
                            break;
                        case defSignalCON_Fault_AC_N_Temp: 
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "N相温度检测");
                            break;
                        case defSignalCON_Fault_RelayPaste: 
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "继电器粘连");
                            break;
                        case defSignalCON_Fault_CP: 
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "CP检测");
                            break;
                        case defSignalCON_Fault_Plug:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "插枪检测");
                            break;
                        case defSignalCON_Fault_Meter:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, (pCON->status.ulSignalFault >> i) & 1, "电表故障");
                            break;
                        default:
                            AddEVSELogObj(jsEVSELogObj, id + 1, defLogLevelFault, 1, "充电枪未知故障");
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
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "急停");
                        break;
                    case defSignalEVSE_Alarm_Knock: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "撞击");
                        break;
                    case defSignalEVSE_Alarm_PE: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "接地");
                        break;
                    case defSignalEVSE_Alarm_PowerOff: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "掉电");
                        break;
                    case defSignalEVSE_Alarm_Arrester: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "防雷");
                        break;
                    case defSignalEVSE_Alarm_AC_A_Temp_War: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电A(L)相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_B_Temp_War: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelWarning, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电B相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_C_Temp_War: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelWarning, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电C相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_N_Temp_War: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelWarning, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电N相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_A_Temp_Cri: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电A(L)相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_B_Temp_Cri: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电B相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_C_Temp_Cri: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电C相过温");
                        break;
                    case defSignalEVSE_Alarm_AC_N_Temp_Cri: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, (pEVSE->status.ulSignalAlarm >> i) & 1, "市电N相过温");
                        break;
                    default:
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelCritical, 1, "EVSE未知警告");
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
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "读卡器");
                        break;
                    case defSignalEVSE_Fault_Bluetooth: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "蓝牙");
                        break;
                    case defSignalEVSE_Fault_Wifi: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "WI-FI");
                        break;
                    case defSignalEVSE_Fault_GPRS: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "GPRS");
                        break;
                    case defSignalEVSE_Fault_GSensor: 
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelFault, (pEVSE->status.ulSignalFault >> i) & 1, "加速度传感器");
                        break;
                    default:
                        AddEVSELogObj(jsEVSELogObj, 0, defLogLevelFault, 1, "EVSE未知故障");
                        break;
                    }
                }
            }
        }//if (ulSignalPoolXor != 0)
        ulSignalEVSEFaultOld = pEVSE->status.ulSignalFault;   //别忘了给old赋值, 要不下次进来没法检测差异哦 :)
        
        /////////////////处理断电产生的临时订单文件/////////////////////
        for(i = 0 ; i < ulTotalCON ; i++)
        {
            pCON = CONGetHandle(i);
            if (pCON->OrderTmp.ucCheckOrderTmp == 1)//上电时初始化为1, 进行临时订单检查
            {
                errcode = GetOrderTmp(pCON->OrderTmp.strOrderTmpPath, &(pCON->OrderTmp.order));
                if (errcode == ERR_FILE_NO)
                {
                    //无订单临时文件,太好了
                    pCON->OrderTmp.ucCheckOrderTmp = 0;
                    continue;
                }
                else if (errcode == ERR_NO)
                {
                    //当前枪有订单临时文件
                    printf_safe("枪%d有订单临时文件 SN:%016ld\n", pCON->info.ucCONID, pCON->OrderTmp.order.ullOrderSN);
                    AddOrderCfg(jsEVSEOrderObj, &(pCON->OrderTmp.order), pechProto);
                    RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                }
                else//解析错误
                {
                    RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                }
            }
        }
        
        /////////////////定时存储EVSELog、Order/////////////////////
        uxBitsData = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBStoreLog, pdTRUE, pdTRUE, 0);
        if ((uxBitsData & defEventBitTimerCBStoreLog) == defEventBitTimerCBStoreLog)
        {
            SetCfgObj(pathEVSELog, jsEVSELogObj, 0x5555);
            printf_safe("日志定时存储...OK\n");
        }
        uxBitsData = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBStoreOrder, pdTRUE, pdTRUE, 0);
        if ((uxBitsData & defEventBitTimerCBStoreOrder) == defEventBitTimerCBStoreOrder)
        {
            SetCfgObj(pathOrder, jsEVSEOrderObj, 0x5555);
            printf_safe("订单定时存储...OK\n");
        }
        
#endif //if 1
        
#endif //DEBUG_NO_TASKDATA
        vTaskDelay(100);
    }
}
