/**
* @file taskremote.c
* @brief 远程通信的操作，接收通信实体的指针
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "includes.h"
#include "interface.h"
#include "taskcreate.h"
#include "taskremote.h"
#include "cfg_parse.h"
#include "cfg_order.h"
#include "stringName.h"

#include "evse_debug.h"

/** @todo (rgw#1#): 如果状态时Charging，那么Remote的状态如果是No或者是err超过5分钟，则判断系统断网，应该停止充电 */

//#define DEBUG_NO_TASKREMOTE
#define defRemoteMaxDelay       60000


void taskremote_reset(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set)
{
    uint32_t ulOptSN;
    uint8_t ucAct;
    int res;

    ulOptSN = 0;
    ucAct = 1;
    res = 0;

    RemoteIF_RecvReset(pProto, &ulOptSN, &res);
    if(res == 1)
    {
        if(flag_set == 0)
        {
            RemoteIF_SendReset(pEVSE, pProto, 2); //1 成功，2失败
            return;
        }
        cfg_set_uint32(pathProtoCfg, &ulOptSN, "%s", jnProtoOptSN);
        cfg_set_uint8(pathProtoCfg, &ucAct, "%s", jnProtoResetAct);

        HAL_NVIC_SystemReset();
        return;
    }

    ////
    if(pProto->info.ucResetAct == 1)
    {
        ucAct = 0;

        cfg_set_uint8(pathProtoCfg, &ucAct, "%s", jnProtoResetAct);
        THROW_ERROR(defDevID_File, pechProto->info.GetProtoCfg(pechProto, NULL), ERR_LEVEL_WARNING, "taskremote GetProtoCfg");
        RemoteIF_SendReset(pEVSE, pProto, 1); //1 成功，2失败
    }
}
void taskremote_set(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    int res;
    uint8_t flag_set;

    res = 0;
    flag_set = 0;

    /******* 设置上报间隔************/
    RemoteIF_RecvSetCyc(pEVSE, pProto, &res);
    if(res == 1)
    {
        xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
                           pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
                           100);//设置timer period ，有timer start 功能
        xTimerChangePeriod(xHandleTimerRemoteStatus,
                           pdMS_TO_TICKS(pechProto->info.ulStatusCyc_ms),
                           100);//设置timer period ，有timer start 功能
    }
    /******* 判断充电过程中不允许设置************/
    if (isEVSEWorking() == 0)
    {
        flag_set = 1;//0不可设置, 1 可设置
    }
    else
    {
        flag_set = 0;
    }

    taskremote_reset(pEVSE, pProto, flag_set);
    RemoteIF_RecvSetPowerFee(pEVSE, pProto, flag_set, &res);
    RemoteIF_RecvSetServFee(pEVSE, pProto, flag_set, &res);
    RemoteIF_RecvSetTimeSeg(pEVSE, pProto, flag_set, &res);
    RemoteIF_RecvSetQR(pEVSE, pProto, flag_set, &res);

    /******* end 充电过程中不允许设置************/

    RemoteIF_RecvSetKey(pEVSE, pProto, &res);

}

void taskremote_req(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    int res;
    RemoteIF_RecvReq(pEVSE, pProto, &res);

}
// 1 更新  0 不更新
static uint32_t ulSignalState_old[defMaxCON];
static int taskremote_status_update(CON_t *pCON)
{
    uint32_t status_old;
    status_old = ulSignalState_old[pCON->info.ucCONID];
    ulSignalState_old[pCON->info.ucCONID] = pCON->status.ulSignalState & (defSignalCON_State_Standby | \
                                                            defSignalCON_State_Working | \
                                                            defSignalCON_State_Stopping | \
                                                            defSignalCON_State_Fault | \
                                                            defSignalCON_State_Plug);
    if (ulSignalState_old[pCON->info.ucCONID]  != status_old)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int CheckSysUpFlags(void)
{
    int succ = 2;

    switch (xSysconf.xUpFlag.chargesdk_bin)
    {
    case 0://无需升级
        succ = 0;
        break;
    case 1://待升级
        succ = 0;
        break;
    case 2://升级成功
        succ = 1;
        break;
    case 3://升级失败
        succ = 2;
        break;
    default:
        succ = 0;
        break;
    }
    return succ;
}
static int taskremote_ota(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode, errcode_sdt;
    int network_res = 0;
    int succ;
    uint8_t flag_set;
    
    if (isEVSEWorking() == 0)
    {
        flag_set = 1;//0不可设置, 1 可设置
    }
    else
    {
        flag_set = 0;
    }
    
    /*1. 平台下发升级命令*/
    errcode = RemoteIF_RecvSetOTA(pProto, &network_res);
    if (errcode == ERR_NO && network_res == 1 && flag_set == 1)
    {
        RemoteIF_SendSetOTA(pEVSE, pProto, NULL, 1);
        /*2. 充电桩上报进入升级状态*/
        RemoteIF_SendOTA_Start(pEVSE, pProto, NULL);
    }
    else if (network_res == 1)//errcode != ERR_NO, flag_set != 1
    {
        RemoteIF_SendSetOTA(pEVSE, pProto, NULL, 0);
    }
    /*3. 平台回复充电桩进入升级状态, 之后重启进入*/
    errcode = RemoteIF_RecvOTA_Start(pProto, &network_res);
    if (errcode == ERR_NO && network_res == 1)
    {
        pProto->info.ftp.ucDownloadStart = 1;
        errcode_sdt = pProto->info.ftp.SetFtpCfg(jnFtpDownloadStart, (void *)&(pProto->info.ftp.ucDownloadStart), ParamTypeU8);
        if (errcode_sdt == ERR_NO)
        {
            HAL_NVIC_SystemReset();
            return 1;//Goodbye :)
        }
    }
    
    errcode = RemoteIF_RecvReqOTA_DW(pProto, &network_res);
    if (errcode == ERR_NO && network_res == 1)
    {
        RemoteIF_SendReqOTA_DW(pEVSE, pProto, NULL);
    }
    
    /*升级结果*/
    succ = CheckSysUpFlags();
    if (succ != 0)
    {
        RemoteIF_SendOTA_Result(pEVSE, pProto, NULL, succ);
        xSysconf.xUpFlag.chargesdk_bin = 0;
    }
    if (succ == 1)//succ == 1 升级成功
    {
        //xSysconf.SetSysCfg(jnSysVersion, pProto->info.ftp.strNewVersion, ParamTypeString); // 不再文件中设置版本, 程序中自带版本
    }
    errcode = RemoteIF_RecvOTA_Result(pProto, &network_res);
    if (errcode == ERR_NO && network_res == 1)
    {
        //happy time;
        xSysconf.SetSysCfg(jnSysChargersdk_bin, (void *)&(xSysconf.xUpFlag.chargesdk_bin), ParamTypeU8);
        xSysconf.GetSysCfg(&xSysconf, NULL);
    }
    return 1;
}

void vTaskEVSERemote(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBits;
    RemoteState_t remotestat;
    Heartbeat_t *pHeart;
    ErrorCode_t errcode;
    int network_res;
    uint32_t reg_try_cnt;
    uint32_t heart_lost;

    ulTotalCON = pListCON->Total;
    uxBits = 0;
    remotestat = REMOTE_NO;//REMOTE_REGEDITED;//
    errcode = ERR_NO;
    network_res = 0;
    reg_try_cnt = 0;
    heart_lost = 0;

    while(1)
    {
#ifndef DEBUG_NO_TASKREMOTE
        switch(remotestat)
        {
        case REMOTE_NO:
            pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Registed;
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPConnectOK,
                                         pdFALSE, pdTRUE, portMAX_DELAY);
            if((uxBits & defEventBitTCPConnectOK) == defEventBitTCPConnectOK)
            {
                RemoteIF_SendLogin(pEVSE, pechProto);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_CONNECTED:
            /********** 注册 **************/
            RemoteIF_RecvLogin(pEVSE, pechProto, &network_res);
            if(network_res == 1)
            {
                reg_try_cnt = 0;
                xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
                                   pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
                                   100);//设置timer period ，有timer start 功能
                xTimerChangePeriod(xHandleTimerRemoteStatus,
                                   pdMS_TO_TICKS(pechProto->info.ulStatusCyc_ms),
                                   100);//设置timer period ，有timer start 功能
                for (i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    pCON->OrderTmp.ucCheckOrderTmp = 1;
                }
                remotestat = REMOTE_REGEDITED;
            }
            else
            {
                reg_try_cnt++;
                if(reg_try_cnt > 200)
                {
                    printf_safe("\n\nlogin try cnt = %d!!!!!!!!!!\n\n", reg_try_cnt);
                    reg_try_cnt = 0;
                    remotestat = REMOTE_ERROR;
                }
            }
            break;
        case REMOTE_REGEDITED:
            pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Registed;
            
            /*********上传未处理的订单**************/
#if 1
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                if (pCON->OrderTmp.ucCheckOrderTmp == 1)
                {
                    if (pCON->state != STATE_CON_CHARGING)
                    {
                        switch (pCON->OrderTmp.order.statRemoteProc.order.stat)
                        {
                        case REMOTEOrder_IDLE:
                            errcode = GetOrderTmp(pCON->OrderTmp.strOrderTmpPath, &(pCON->OrderTmp.order));
                            if (errcode == ERR_FILE_NO)
                            {
                                //无订单临时文件,太好了
                                printf_safe("无订单临时文件,太好了\n");
                                pCON->OrderTmp.ucCheckOrderTmp = 0;
                                continue;
                            }
                            else if (errcode == ERR_NO)
                            {
                                //当前枪有订单临时文件
                                printf_safe("枪%d有订单临时文件\n", pCON->info.ucCONID);
                                pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_Send;
                            }
                            else//解析错误
                            {
                                RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                            }
                            break;
                        case REMOTEOrder_Send:
                            RemoteIF_SendOrder(pEVSE, pechProto, &(pCON->OrderTmp.order));
                            pCON->OrderTmp.order.statRemoteProc.order.timestamp = time(NULL);
                            pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_WaitRecv;
                            break;
                        case REMOTEOrder_WaitRecv:
                            RemoteIF_RecvOrder(pEVSE, pechProto, &(pCON->OrderTmp.order), &network_res);
                            if (network_res == 1)
                            {
                                pCON->OrderTmp.order.ucPayStatus = 1;
                                AddOrderCfg(pathOrder, &(pCON->OrderTmp.order), pechProto);
                                RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                                pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                            }
                            else if (network_res == 0)
                            {
                                if (time(NULL) - pCON->OrderTmp.order.statRemoteProc.order.timestamp > 60)
                                {
                                    pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                                }
                            }
                            break;
                        }//switch stat
                    }
                    else//如果不是充电中下次循环过来就不检查了
                    {
                        pCON->OrderTmp.ucCheckOrderTmp = 0;
                    }
                }
            }//for id
#endif
            /************ 心跳 ***************/
            uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                         defEventBitTimerCBHeartbeat,
                                         pdTRUE, pdTRUE , 0);
            if((uxBits & defEventBitTimerCBHeartbeat) == defEventBitTimerCBHeartbeat)
            {
                RemoteIF_SendHeart(pEVSE, pechProto);
            }

            RemoteIF_RecvHeart(pEVSE, pechProto, &network_res);
            if(network_res != 1)
            {
                heart_lost++;
                if(heart_lost > 750)//750
                {
                    printf_safe("\n\nHeart LOST!!!!\n\n");
                    heart_lost = 0;
                    remotestat = REMOTE_ERROR;
                    break;
                }
            }
            else
            {
                printf_protolog("\n\nRecv Heart  !!!!!!!!!!\n\n");
                heart_lost = 0;
            }

            /************ 状态******************/
            uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                         defEventBitTimerCBStatus,
                                         pdTRUE, pdTRUE , 0);
            if((uxBits & defEventBitTimerCBStatus) == defEventBitTimerCBStatus)
            {
                for(i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    RemoteIF_SendStatus(pEVSE, pechProto, pCON);
                }
            }
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                if (taskremote_status_update(pCON) == 1)
                {
                    RemoteIF_SendStatus(pEVSE, pechProto, pCON);
                }
            }

            /*********** 远程启停*******************/
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                
                switch (pCON->order.statRemoteProc.rmt_ctrl.stat)
                {
                case REMOTECTRL_IDLE:
                    RemoteIF_RecvRemoteCtrl(pEVSE, pechProto, 
                                            &(pCON->order.statRemoteProc.rmt_ctrl.id), 
                                            &(pCON->order.statRemoteProc.rmt_ctrl.ctrl_onoff), 
                                            &network_res);
                    if (network_res == 1)
                    {
                        pCON->order.statRemoteProc.rmt_ctrl.timestamp = time(NULL);
                        if (pCON->order.statRemoteProc.rmt_ctrl.ctrl_onoff == 1)
                        {
                            if ((pCON->status.ulSignalState & defSignalCON_State_Standby) == defSignalCON_State_Standby)
                            {
                                pCON->order.statOrder = STATE_ORDER_WAITSTART;//状态处理见taskdata.c文件
                                pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_WAIT_START;
                                break;
                            }
                            else
                            {
                                pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_FAIL;
                                break;
                            }
                        }
                        else if (pCON->order.statRemoteProc.rmt_ctrl.ctrl_onoff == 2)
                        {
                            pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_WAIT_STOP;
                            break;
                        }
                    }
                    break;
                case REMOTECTRL_WAIT_START:
                    uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                    defEventBitCONStartOK,
                                                    pdFALSE,
                                                    pdTRUE,
                                                    0);
                    if ((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
                    {
                        pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_SUCC;
                    }
                    else
                    {
                        if (time(NULL) - pCON->order.statRemoteProc.rmt_ctrl.timestamp > 60)
                        {
                            pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_FAIL;
                        }
                    }
                    break;
                case REMOTECTRL_WAIT_STOP:
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if ((uxBits & defEventBitCONStartOK) != defEventBitCONStartOK)
                    {
                        pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_SUCC;
                    }
                    else
                    {
                        if (time(NULL) - pCON->order.statRemoteProc.rmt_ctrl.timestamp > 60)
                        {
                            pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_FAIL;
                        }
                    }
                    break;
                case REMOTECTRL_SUCC:
                    RemoteIF_SendRemoteCtrl(pEVSE, pechProto, pCON, 1, 0); //0， 正常
                    pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_IDLE;
                    break;
                case REMOTECTRL_FAIL:
                    xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//bugfix：扫码后启动充电失败未清除认证标志，导致下一辆可充电车直接充电
                    if ((pCON->status.ulSignalState & defSignalCON_State_Plug) != defSignalCON_State_Plug)
                    {
                        RemoteIF_SendRemoteCtrl(pEVSE, pechProto, pCON, 0, 3);//3, 枪未连接
                    }
                    else
                    {
                        RemoteIF_SendRemoteCtrl(pEVSE, pechProto, pCON, 0, 4);//4， 其他错误
                    }
                    pCON->order.statRemoteProc.rmt_ctrl.stat = REMOTECTRL_IDLE;
                    break;
                default:
                    break;
                }
            }
            /***************刷卡启停********************/
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch(pCON->order.statRemoteProc.card.stat)
                {
                case CARDCTRL_IDLE: //初始化时进入的状态
                    break;
                case CARDCTRL_WAIT_START:  //taskdata.c中转换的这个状态
                    uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                 defEventBitCONStartOK,
                                                 pdFALSE, pdTRUE, 0);
                    if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
                    {
                        RemoteIF_SendCardStartRes(pEVSE, pechProto, pCON, 1); //1， 成功
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_WAIT_START_RECV;
                    }
                    else
                    {
                        if(time(NULL) - pCON->order.statRemoteProc.card.timestamp > 60)
                        {
                            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//bugfix：启动充电失败未清除认证标志，导致下一辆可充电车直接充电
                            RemoteIF_SendCardStartRes(pEVSE, pechProto, pCON, 0);//0,
                            pCON->order.statRemoteProc.card.stat = CARDCTRL_WAIT_START_RECV;
                        }
                    }
                    break;
                case CARDCTRL_WAIT_STOP:
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if((uxBits & defEventBitCONStartOK) != defEventBitCONStartOK)
                    {
                        //停止充电
                        RemoteIF_SendCardStopRes(pEVSE, pechProto, pCON);
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_WAIT_STOP_RECV;
                    }
                    else
                    {
                        //...
                    }
                    break;

                case CARDCTRL_WAIT_START_RECV:
                    RemoteIF_RecvCardStartRes(pechProto, &network_res);
                    if(network_res == 1)
                    {
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_WAIT_STOP;
                    }
                    break;
                case CARDCTRL_WAIT_STOP_RECV:
                    RemoteIF_RecvCardStopRes(pechProto, &network_res);
                    if(network_res == 1)
                    {
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_IDLE;
                    }
                    break;
                }
            }
            /***************扫码实时数据 *******************/
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch(pCON->order.statRemoteProc.rtdata.stat)
                {
                case REMOTERTData_IDLE:
                {
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
                    {
                        xTimerChangePeriod(pCON->status.xHandleTimerRTData,
                                           pdMS_TO_TICKS(pechProto->info.ulRTDataCyc_ms),
                                           100);//设置timer period ，有timer start 功能
                        pCON->order.statRemoteProc.rtdata.stat = REMOTERTData_START;
                    }
                    break;
                }
                case REMOTERTData_START: //在这期间，由Timer发送Timeout事件对RTData进行刷新
                {
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if((uxBits & defEventBitCONStartOK) != defEventBitCONStartOK)
                    {
                        xTimerStop(pCON->status.xHandleTimerRTData, 100);
                        pCON->order.statRemoteProc.rtdata.stat = REMOTERTData_STOP;
                    }
                    //连接中断情况下，由于状态不在REGEDITED下，所以不会一直发送实时数据
                    break;
                }
                case REMOTERTData_STOP:
                {
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventOrder);
                    if((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)
                    {
                        switch(pCON->order.ucStopType)
                        {
                        case defOrderStopType_RFID:
                        case defOrderStopType_Remote:
                            pCON->order.statRemoteProc.rtdata.stop_reason = 1;
                            break;
                        case defOrderStopType_Full:
                            pCON->order.statRemoteProc.rtdata.stop_reason = 3;
                            break;
                        case defOrderStopType_Fee:
                            pCON->order.statRemoteProc.rtdata.stop_reason = 4;
                            break;
                        case defOrderStopType_Scram:
                        case defOrderStopType_NetLost:
                        case defOrderStopType_Poweroff:
                        case defOrderStopType_OverCurr:
                        case defOrderStopType_Knock:
                            pCON->order.statRemoteProc.rtdata.stop_reason = 5;
                            break;
                        default:
                            pCON->order.statRemoteProc.rtdata.stop_reason = 6;
                            break;
                        }

                        if(pCON->order.ucStartType == 4)//有卡
                        {
                            RemoteIF_SendCardRTData(pEVSE, pechProto, pCON, 2, pCON->order.statRemoteProc.rtdata.stop_reason);
                        }
                        else if(pCON->order.ucStartType == 5)//无卡
                        {
                            RemoteIF_SendRTData(pEVSE, pechProto, pCON, 2, pCON->order.statRemoteProc.rtdata.stop_reason);
                        }
                    }
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteRTDataOK); //告诉taskdata，RTData用完了订单数据
                    pCON->order.statRemoteProc.rtdata.stat = REMOTERTData_IDLE;
                    break;
                }
                }
            }
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                             defEventBitChargeRTDataTimer,
                                             pdTRUE, pdTRUE, 0);
                if((uxBits & defEventBitChargeRTDataTimer) == defEventBitChargeRTDataTimer)
                {
                    if(pCON->order.ucStartType == 4)//有卡
                    {
                        RemoteIF_SendCardRTData(pEVSE, pechProto, pCON, 1, 0);
                    }
                    else if(pCON->order.ucStartType == 5)//无卡
                    {
                        RemoteIF_SendRTData(pEVSE, pechProto, pCON, 1, 0);
                    }
                }
            }

            /******** 交易记录(订单) ****************/
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch(pCON->order.statRemoteProc.order.stat)
                {
                case REMOTEOrder_IDLE:
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventOrder);
                    if((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)
                    {
                        pCON->order.statRemoteProc.order.stat = REMOTEOrder_Send;
                    }
                    break;
                case REMOTEOrder_Send:
                    RemoteIF_SendOrder(pEVSE, pechProto, &(pCON->order));
                    pCON->order.statRemoteProc.order.timestamp = time(NULL);
                    pCON->order.statRemoteProc.order.stat = REMOTEOrder_WaitRecv;
                    break;
                case REMOTEOrder_WaitRecv:
                    RemoteIF_RecvOrder(pEVSE, pechProto, &(pCON->order), &network_res);
                    if(network_res == 1)
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderOK);
                        pCON->order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                    }
                    else if(network_res == 0)
                    {
                        if (time(NULL) - pCON->order.statRemoteProc.order.timestamp > 60)
                        {
                            //超时就不要发送使用完成OK了，下面这条语句注释 ↓
                            //xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderOK);
                            pCON->order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                        }
                    }
                    break;
                }//switch
            }
            /******** 平台下发设置 ****************/
            taskremote_set(pEVSE, pechProto);
            taskremote_req(pEVSE, pechProto);

            /******** 检测并上报故障 ****************/
            RemoteIF_SendUpFault(pEVSE, pechProto);
            RemoteIF_SendUpWarning(pEVSE, pechProto);

            /***********获取帐户信息**********************/ //由taskrfid调用获取账户信息接口 2017年8月10日
            
            /******** OTA ****************/
            taskremote_ota(pEVSE, pechProto);

            break;//REMOTE_REGEDITED
        case REMOTE_RECONNECT:
            xTimerStop(xHandleTimerRemoteHeartbeat, 100);
            xTimerStop(xHandleTimerRemoteStatus, 100);
            pModem->state = DS_MODEM_ERR;
            vTaskDelay(1000);
            remotestat = REMOTE_NO;
            printf_safe("State Reconnect ,Call TCP close!!\n");
            break;
        case REMOTE_ERROR:
            xTimerStop(xHandleTimerRemoteHeartbeat, 100);
            xTimerStop(xHandleTimerRemoteStatus, 100);
            remotestat = REMOTE_NO;
            xEventGroupSetBits(xHandleEventRemote, defEventBitRemoteError);
            printf_safe("remote state error ,Call TCP close!!\n");
            break;
        default:
            break;
        }

#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
#endif //#ifndef DEBUG_NO_TASKREMOTE
        vTaskDelay(100);
    }
}
