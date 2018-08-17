/**
* @file taskremote.c
* @brief 远程通信的操作
*        重连：丢失3个心跳
*        停止充电：丢失12个心跳
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
#include "file_op.h"
#include "yaffsfs.h"
#include "tasknetwork.h"

#include "evse_debug.h"

/** @todo (rgw#1#): 如果状态时Charging，那么Remote的状态如果是No或者是err超过5分钟，则判断系统断网，应该停止充电 */

//#define DEBUG_NO_TASKREMOTE

RemoteState_t remotestat;

extern TaskHandle_t xHandleTaskRemoteCmdProc;

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
    CON_t *pCON;
    int i;
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
    if (isEVSEStandby() == 1)
    {
        flag_set = 1;//0不可设置, 1 可设置
    }
    else
    {
        flag_set = 0;
    }

    taskremote_reset(pEVSE, pProto, flag_set);
    RemoteIF_RecvSetEnergyFee(pEVSE, pProto, flag_set, &res);
    RemoteIF_RecvSetServFee(pEVSE, pProto, flag_set, &res);
    RemoteIF_RecvSetTimeSeg(pEVSE, pProto, flag_set, &res);
    RemoteIF_RecvSetQR(pEVSE, pProto, flag_set, &res);

    /******* end 充电过程中不允许设置************/

    RemoteIF_RecvSetKey(pEVSE, pProto, &res);
    for (i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        RemoteIF_RecvSetPower(pEVSE, pProto, pCON->info.ucCONID, &res);
    }
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
//0：无升级操作   1：升级成功  2：升级失败
int CheckSysUpFlags(void)
{
    int tmp = 2;
    int succ = 2;
    char ch[2] = { 0 }; 
    uint8_t res;
    
    res = get_tmp_file(pathUpgradeTmp, ch);
    if (res != 1)
    {
        succ = 0;
    }
    else
    {
        tmp = atoi(ch);
        switch (tmp)
        {
        case 2:
            succ = 1;
            break;
        case 3:
            succ = 2;
            break;
        default:
            succ = 2;
            break;
        }
        yaffs_unlink(pathUpgradeTmp);
    }

    return succ;
}
static int taskremote_ota(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    ErrorCode_t errcode, errcode_sdt;
    int network_res = 0;
    int succ;
    uint8_t flag_set;
    
    if (isEVSEStandby() == 1)
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
        errcode_sdt = cfg_set_uint8(pathFTPCfg, &pProto->info.ftp.ucDownloadStart, "%s", jnFtpDownloadStart);
        if (errcode_sdt == ERR_NO)
        {
            //HAL_NVIC_SystemReset();
            netChangeState(net_dev, NET_STATE_DISCONNECT);
            remotestat = REMOTE_NO;
            vTaskDelay(1000);
            return 1;//See you later :)
        }
    }
    
    errcode = RemoteIF_RecvReqOTA_DW(pProto, &network_res);
    if (errcode == ERR_NO && network_res == 1)
    {
        RemoteIF_SendReqOTA_DW(pEVSE, pProto, NULL);
    }
    
    /*升级结果*/
    succ = CheckSysUpFlags();
    if (succ != 0)//succ == 1 升级成功
    {
        RemoteIF_SendOTA_Result(pEVSE, pProto, NULL, succ);
        xSysconf.xUpFlag.chargesdk_bin = 0;
    }
    errcode = RemoteIF_RecvOTA_Result(pProto, &network_res);
    if (errcode == ERR_NO && network_res == 1)
    {
        //happy time;
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
    Heartbeat_t *pHeart;
    ErrorCode_t errcode;
    int network_res;
    uint32_t login_try_cnt;
    time_t last_heart_stamp;

    ulTotalCON = pListCON->Total;
    uxBits = 0;
    remotestat = REMOTE_NO;//REMOTE_REGEDITED;//
    errcode = ERR_NO;
    network_res = 0;
    login_try_cnt = 0;
    last_heart_stamp = 0;

    while(1)
    {
#ifndef DEBUG_NO_TASKREMOTE
        switch(remotestat)
        {
        case REMOTE_NO:
            pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Logined;
            if((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Online) == defSignalEVSE_State_Network_Online)
            {
                RemoteIF_SendLogin(pEVSE, pechProto);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_CONNECTED:
            vTaskResume(xHandleTaskRemoteCmdProc); //重启协议处理任务
            /********** 登录 **************/
            RemoteIF_RecvLogin(pEVSE, pechProto, &network_res);
            if(network_res == 1)
            {
                login_try_cnt = 0;
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
                last_heart_stamp = time(NULL);//防止重连时心跳检测测还是上次丢失的时间
                remotestat = REMOTE_LOGINED;
#if DEBUG
                smtpClientTest();
#endif
                break;
            }
            else
            {
                login_try_cnt++;
                if(login_try_cnt > 200)
                {
                    printf_safe("\n\nlogin try cnt = %d!!!!!!!!!!\n\n", login_try_cnt);
                    login_try_cnt = 0;
                    remotestat = REMOTE_ERROR;
                }
            }
            break;
        case REMOTE_LOGINED:
            pEVSE->status.ulSignalState |= defSignalEVSE_State_Network_Logined;
            if ((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Online) != defSignalEVSE_State_Network_Online)
            {
                remotestat = REMOTE_OFFLINE;
                break;
            }
            /*********上传未处理的订单**************/
#if 1
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                if (pCON->OrderTmp.ucCheckOrderTmp == 1)
                {
                    if (pCON->state != STATE_CON_CHARGING && 
                        pCON->order.statRemoteProc.order.stat == REMOTEOrder_IDLE)//只有在处理订单idle时，才进行临时订单检查
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
                                printf_safe("stat %d, 枪%d有订单临时文件\n", pCON->order.statRemoteProc.order.stat,pCON->info.ucCONID);
                                pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_Send;
                            }
                            else//解析错误
                            {
                                RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                            }
                            break;
                        case REMOTEOrder_Send:
                            printf_safe("CON%d,临时订单发送\n", pCON->info.ucCONID);
                            RemoteIF_SendOrder(pEVSE, pechProto, &(pCON->OrderTmp.order));
                            pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_WaitRecv;
                            break;
                        case REMOTEOrder_WaitRecv:
                            errcode = RemoteIF_RecvOrder(pEVSE, pechProto, &(pCON->OrderTmp.order), &network_res);
                            if (network_res == 1)
                            {
                                pCON->OrderTmp.order.ucPayStatus = 1;
                                AddOrderCfg(pathOrder, &(pCON->OrderTmp.order), pechProto);
                                RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                                pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                            }
                            else if (network_res == 0)
                            {
                                if (errcode == ERR_REMOTE_ORDERSN || errcode == ERR_REMOTE_PARAM)//服务器不接受该订单号
                                {
                                    RemoveOrderTmp(pCON->OrderTmp.strOrderTmpPath);
                                    pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                                    break;
                                }
                                if(errcode == ERR_REMOTE_TIMEOUT)
                                {
                                    pCON->OrderTmp.order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                                    remotestat = REMOTE_ERROR;
                                    break;
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
                if ((time(NULL) - last_heart_stamp) * 1000 / (pechProto->info.ulHeartBeatCyc_ms + 5000) >= 3)//心跳丢失3个进行重连
                {
                    printf_safe("\n\nHeart LOST!!!!\n\n");
                    remotestat = REMOTE_ERROR;
                    break;
                }
            }
            else
            {
                pEVSE->status.ulTimeUpdated = 1; //上电时只使用一次，用来看是否校准时间
                printf_protolog("\n\nRecv Heart  !!!!!!!!!!\n\n");
                last_heart_stamp = time(NULL);
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
                    RemoteIF_RecvRemoteCtrl(pEVSE, pechProto, pCON,
                                            &(pCON->order.statRemoteProc.rmt_ctrl.id), 
                                            &(pCON->order.statRemoteProc.rmt_ctrl.ctrl_onoff), 
                                            &network_res);
                    if (network_res == 1)
                    {
                        pCON->order.statRemoteProc.rmt_ctrl.timestamp = time(NULL);
                        if (pCON->order.statRemoteProc.rmt_ctrl.ctrl_onoff == 1)
                        {
                            if ((pCON->status.ulSignalState & defSignalCON_State_Standby) == defSignalCON_State_Standby &&
                                pRFIDDev->state == STATE_RFID_NOID)
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
                        if (time(NULL) - pCON->order.statRemoteProc.rmt_ctrl.timestamp > 20)
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
                        if (time(NULL) - pCON->order.statRemoteProc.rmt_ctrl.timestamp > 20)
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
                    errcode = RemoteIF_RecvCardStartRes(pechProto, pCON, &network_res);
                    if(network_res == 1)
                    {
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_WAIT_STOP;
                    }
                    if (errcode == ERR_REMOTE_TIMEOUT)
                    {
                        printf_safe("平台回复刷卡启动充电超时\n");
                        xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionOfflineStop);
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_IDLE;
                        remotestat = REMOTE_ERROR;
                    }
                    break;
                case CARDCTRL_WAIT_STOP_RECV:
                    errcode = RemoteIF_RecvCardStopRes(pechProto, pCON, &network_res);
                    if(network_res == 1)
                    {
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_IDLE;
                    }
                    if (errcode == ERR_REMOTE_TIMEOUT)
                    {
                        printf_safe("平台回复刷卡停止充电超时\n");
                        pCON->order.statRemoteProc.card.stat = CARDCTRL_IDLE;
                        remotestat = REMOTE_ERROR;
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
                        case defOrderStopType_Offline:
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
                    uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                        defEventBitCONStartOK,
                        pdFALSE,
                        pdTRUE,
                        0);
                    if ((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
                    {
                        pCON->order.statRemoteProc.order.stat = REMOTEOrder_WaitOrder;
                    }
                    break;
                case REMOTEOrder_WaitOrder:
                    uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder, 
                        defEventBitOrderMakeFinishToRemote, 
                        pdTRUE,
                        pdTRUE,
                        0);
                    if ((uxBits & defEventBitOrderMakeFinishToRemote) == defEventBitOrderMakeFinishToRemote)
                    {
                        pCON->order.statRemoteProc.order.stat = REMOTEOrder_Send;
                    }
                    break;
                case REMOTEOrder_Send:
                    printf_safe("CON%d,正常订单发送\n", pCON->info.ucCONID);
                    RemoteIF_SendOrder(pEVSE, pechProto, &(pCON->order));
                    pCON->order.statRemoteProc.order.stat = REMOTEOrder_WaitRecv;
                    break;
                case REMOTEOrder_WaitRecv:
                    errcode = RemoteIF_RecvOrder(pEVSE, pechProto, &(pCON->order), &network_res);
                    if(network_res == 1 && errcode == ERR_NO)
                    {
                        network_res = 0;
                        xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderOK);
                        pCON->order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                    }
                    else if(network_res == 0)
                    {
                        if(errcode == ERR_REMOTE_TIMEOUT)
                        {
                            printf_safe("平台Order回复超时\n");
                            xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderTimeOut);
                            pCON->order.statRemoteProc.order.stat = REMOTEOrder_IDLE;
                            remotestat = REMOTE_ERROR;
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

            /******** 紧急停止 ****************/
            for (i = 0; i < pEVSE->info.ucTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch (pCON->order.statRemoteProc.rmt_emer_stop.stat)
                {
                case REMOTE_EMER_STOP_IDLE:
                        RemoteIF_RecvEmergencyStop(pechProto, pCON->info.ucCONID, &network_res);
                        if (network_res == 1)
                        {
                            if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
                            {
                                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRmtEmergencyStop);
                            }
                            pCON->order.statRemoteProc.rmt_emer_stop.timestamp = time(NULL);
                            pCON->order.statRemoteProc.rmt_emer_stop.stat = REMOTE_EMER_STOP_WAIT_STOP;
                        }
                    break;
                case REMOTE_EMER_STOP_WAIT_STOP:
                    if ((pCON->status.ulSignalState & defSignalCON_State_Working) != defSignalCON_State_Working)
                    {
                        RemoteIF_SendEmergencyStop(pEVSE, pechProto, pCON, 1);//成功
                        pCON->order.statRemoteProc.rmt_emer_stop.stat = REMOTE_EMER_STOP_RETURN;
                        break;
                    }
                    else
                    {
                        if (time(NULL) - pCON->order.statRemoteProc.rmt_emer_stop.timestamp > 20)
                        {
                            xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRmtEmergencyStop);
                            RemoteIF_SendEmergencyStop(pEVSE, pechProto, pCON, 0); //超时失败
                            pCON->order.statRemoteProc.rmt_emer_stop.stat = REMOTE_EMER_STOP_RETURN;
                            break;
                        }
                    }
                    break;
                case REMOTE_EMER_STOP_RETURN:
                    pCON->order.statRemoteProc.rmt_emer_stop.stat = REMOTE_EMER_STOP_IDLE;
                    break;
                }
            }
            /******** 预约 ****************/
            /*  0：未知
                1：无预约
                2：已预约
                3：预约失败
            */
            for (i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                errcode = RemoteIF_RecvSetAppoint(pEVSE, pechProto, pCON->info.ucCONID, &network_res);
                if (errcode == ERR_NO)
                {
                    if (network_res == 1)//2：预约成功
                    {
                        RemoteIF_SendAppoint(ECH_CMDID_SET_APPOINT, pEVSE, pechProto, pCON, 2);
                    }
                    else//3：预约失败
                    {
                        RemoteIF_SendAppoint(ECH_CMDID_SET_APPOINT, pEVSE, pechProto, pCON, 3);
                    }
                }
                errcode = RemoteIF_RecvReqAppoint(pEVSE, pechProto, pCON->info.ucCONID, &network_res);
                if (errcode == ERR_NO )
                {
                    if (network_res == 1)//查询成功
                    {
                        RemoteIF_SendAppoint(ECH_CMDID_REQ_APPOINT, pEVSE, pechProto, pCON, pCON->appoint.status);
                    }
                    else//查询失败
                    {
                        RemoteIF_SendAppoint(ECH_CMDID_REQ_APPOINT, pEVSE, pechProto, pCON, 0);
                    }
                }
            }
            
            break;//REMOTE_REGEDITED
        case REMOTE_RECONNECT:
            pEVSE->status.ulSignalState &= ~defSignalEVSE_State_Network_Logined;
            {   //如果是APP启动，丢失12个心跳停止充电
                if((time(NULL) - last_heart_stamp) * 1000 / pechProto->info.ulHeartBeatCyc_ms >= 12) 
                {
                    for (i = 0; i < ulTotalCON; i++)
                    {
                        pCON = CONGetHandle(i);
                        if (pCON->order.ucStartType == defOrderStartType_Remote)
                        {
                            if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
                            {
                                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionOfflineStop);
                                remotestat = REMOTE_NO;
                                break;
                            }
                        }
                    }
                }
            }
            {   //从REMOTE_ERR状态过来后，先等defEventBitRemoteError事件处理完，再判断Online
                uxBits = xEventGroupGetBits(xHandleEventRemote);
                if ((uxBits & defEventBitRemoteError) == defEventBitRemoteError)
                {
                    vTaskDelay(1000);
                    break;
                }
            }
            if((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Online) == defSignalEVSE_State_Network_Online)
            {
                RemoteIF_SendLogin(pEVSE, pechProto);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_ERROR:
            vTaskSuspend(xHandleTaskRemoteCmdProc);//停止协议处理任务
            xTimerStop(xHandleTimerRemoteHeartbeat, 100);
            xTimerStop(xHandleTimerRemoteStatus, 100);
            remotestat = REMOTE_RECONNECT;
            xEventGroupSetBits(xHandleEventRemote, defEventBitRemoteError);
            printf_safe("remote state error ,remote state -> reconnect!!\n");
            break;
        case REMOTE_OFFLINE:
            vTaskSuspend(xHandleTaskRemoteCmdProc); //停止协议处理任务
            xTimerStop(xHandleTimerRemoteHeartbeat, 100);
            xTimerStop(xHandleTimerRemoteStatus, 100);
            remotestat = REMOTE_RECONNECT;
            printf_safe("net offline ,remote state -> remote reconnect!!\n");
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
