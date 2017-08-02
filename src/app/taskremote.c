/**
* @file taskremote.c
* @brief Զ��ͨ�ŵĲ���������ͨ��ʵ���ָ��
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "includes.h"
#include "interface.h"
#include "taskcreate.h"
#include "taskremote.h"
#include "cfg_parse.h"
#include "stringName.h"

/** @todo (rgw#1#): ���״̬ʱCharging����ôRemote��״̬�����No������err����5���ӣ����ж�ϵͳ������Ӧ��ֹͣ��� */

//#define DEBUG_NO_TASKREMOTE

typedef enum
{
    REMOTE_NO,
    REMOTE_CONNECTED,
    REMOTE_REGEDITED,
    REMOTE_RECONNECT,
    REMOTE_ERROR
} RemoteState_t;

typedef enum
{
    REMOTEHEART_IDLE,
    REMOTEHEART_RECV
} RemoteHeartState_e;

typedef enum
{
    REMOTECTRL_IDLE,
    REMOTECTRL_WAIT_START,
    REMOTECTRL_SUCC,
    REMOTECTRL_WAIT_STOP,
    REMOTECTRL_STOP,
    REMOTECTRL_FAIL
} RemoteCtrlState_e;

typedef enum
{
    REMOTERTData_IDLE,
    REMOTERTData_START,
    REMOTERTData_STOP
} RemoteRTDataState_e;

typedef enum
{
    REMOTEOrder_IDLE,
    REMOTEOrder_Send,
    REMOTEOrder_WaitRecv
} RemoteOrderState_e;

void taskremote_reset(EVSE_t *pEVSE, echProtocol_t *pProto)
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

        pProto->info.SetProtoCfg(jnProtoOptSN, ParamTypeU32, NULL, 0, &ulOptSN);
        pProto->info.SetProtoCfg(jnProtoResetAct, ParamTypeU8, NULL, 0, &ucAct);

        HAL_NVIC_SystemReset();
        return;
    }

    ////
    if(pProto->info.ucResetAct == 1)
    {
        ucAct = 0;

        pProto->info.SetProtoCfg(jnProtoResetAct, ParamTypeU8, NULL, 0, &ucAct);
        THROW_ERROR(defDevID_File, pechProto->info.GetProtoCfg(pechProto, NULL), ERR_LEVEL_WARNING, "taskremote GetProtoCfg");
        RemoteIF_SendReset(pEVSE, pProto, 1); //1 �ɹ���2ʧ��
    }
}
void taskremote_set(EVSE_t *pEVSE, echProtocol_t *pProto)
{
    int res;
    res = 0;
    RemoteIF_RecvSetPowerFee(pEVSE, pProto, &res);
    RemoteIF_RecvSetServFee(pEVSE, pProto, &res);
    RemoteIF_RecvSetCyc(pEVSE, pProto, &res);
    if(res == 1)
    {
        xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
                           pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
                           100);//����timer period ����timer start ����
        xTimerChangePeriod(xHandleTimerRemoteStatus,
                           pdMS_TO_TICKS(pechProto->info.ulStatusCyc_ms),
                           100);//����timer period ����timer start ����
    }

}


void vTaskEVSERemote(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBits;
    RemoteState_t remotestat;
    RemoteHeartState_e eRmtHeartStat;
    RemoteCtrlState_e eRmtCtrlStat;
    RemoteRTDataState_e eRmtRTDataStat;
    RemoteOrderState_e eRmtOrderStat;
    Heartbeat_t *pHeart;
    ErrorCode_t errcode;
    int network_res;
    uint8_t id_rmtctrl;
    uint8_t ctrl_rmtctrl;
    time_t time_rmtctrl;
    time_t time_order;
    uint8_t order_send_count;
    uint32_t reg_try_cnt;
    uint32_t heart_lost;

    ulTotalCON = pListCON->Total;
    uxBits = 0;
    uxBits = 0;
    remotestat = REMOTE_NO;//REMOTE_REGEDITED;//
    eRmtHeartStat = REMOTEHEART_IDLE;  /** @todo (rgw#1#): �⼸��״̬��Ҫת�Ƶ�CON�ṹ���У������ǹʱ״̬����� */
    eRmtCtrlStat = REMOTECTRL_IDLE;
    eRmtRTDataStat = REMOTERTData_IDLE;
    eRmtOrderStat = REMOTEOrder_IDLE;
    errcode = 0;
    network_res = 0;
    id_rmtctrl = 0;
    ctrl_rmtctrl = 0;/** @todo (rgw#1#): ����ǹ�йصı���ת�Ƶ�CON�ṹ���� */
    time_rmtctrl = 0;
    time_order = 0;
    order_send_count = 0;
    reg_try_cnt = 0;
    heart_lost = 0;

    while(1)
    {
#ifndef DEBUG_NO_TASKREMOTE
        switch(remotestat)
        {
        case REMOTE_NO:
            /** @todo (rgw#1#): ������������ */
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPConnectOK,
                                         pdFALSE, pdTRUE, portMAX_DELAY);
            if((uxBits & defEventBitTCPConnectOK) == defEventBitTCPConnectOK)
            {
                RemoteIF_SendRegist(pEVSE, pechProto);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_CONNECTED:
            /********** ע�� **************/
            RemoteIF_RecvRegist(pEVSE, pechProto, &network_res);
            if(network_res == 1)
            {
                reg_try_cnt = 0;
                xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
                                   pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
                                   100);//����timer period ����timer start ����
                xTimerChangePeriod(xHandleTimerRemoteStatus,
                                   pdMS_TO_TICKS(pechProto->info.ulStatusCyc_ms),
                                   100);//����timer period ����timer start ����
                remotestat = REMOTE_REGEDITED;
            }
            else
            {
                reg_try_cnt++;
                if(reg_try_cnt > 200)
                {
                    printf_safe("\n\nregedit try cnt = %d!!!!!!!!!!\n\n", reg_try_cnt);
                    reg_try_cnt = 0;
                    remotestat = REMOTE_NO;
                }
                uxBits = xEventGroupGetBits(xHandleEventTCP);
                if((uxBits & defEventBitTCPConnectFail) == defEventBitTCPConnectFail)
                {
                    reg_try_cnt = 0;
                    remotestat = REMOTE_NO;
                }
            }
            break;
        case REMOTE_REGEDITED:
            uxBits = xEventGroupWaitBits(xHandleEventTCP,
                                         defEventBitTCPConnectFail,
                                         pdTRUE, pdTRUE, 0);
            if((uxBits & defEventBitTCPConnectFail) == defEventBitTCPConnectFail)
            {
                remotestat = REMOTE_RECONNECT;
                printf_safe("State Regedit TCPConnectFail, Call Reconnect!!!\n");
                break;
            }

            /************ ���� ***************/
            uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                         defEventBitTimerCBHeartbeat,
                                         pdTRUE, pdTRUE , 0);
            if((uxBits & defEventBitTimerCBHeartbeat) == defEventBitTimerCBHeartbeat)
            {
                RemoteIF_SendHeart(pEVSE, pechProto);
//                xTimerStop(xHandleTimerRemoteHeartbeat, 100);
//                eRmtHeartStat = REMOTEHEART_RECV;
            }

//            switch(eRmtHeartStat)
//            {
//            case REMOTEHEART_IDLE:
//                break;
//            case REMOTEHEART_RECV:
            RemoteIF_RecvHeart(pEVSE, pechProto, &network_res);
            if(network_res != 1)
            {
                heart_lost++;
//                    printf_safe("heart_lost = %d\n",heart_lost);
                if(heart_lost > 750)
                {
                    heart_lost = 0;
                    eRmtHeartStat = REMOTEHEART_IDLE;
                    remotestat = REMOTE_RECONNECT;
                    break;
                }
            }
            else
            {
//                    xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
//                                   pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
//                                   100);//�����Ļ�������ʱ��������Ƶ�ʲ�������
                printf_safe("\n\nRecv Heart  !!!!!!!!!!\n\n");
//                    eRmtHeartStat = REMOTEHEART_IDLE;
                heart_lost = 0;
            }
//                break;
//            }

            /************ ����******************/

            taskremote_reset(pEVSE, pechProto);


            /************ ״̬******************/
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

            /*********** Զ����ͣ*******************/
            switch(eRmtCtrlStat)
            {
            case REMOTECTRL_IDLE:
                RemoteIF_RecvRemoteCtrl(pEVSE, pechProto, &id_rmtctrl, &ctrl_rmtctrl, &network_res);
                if(network_res == 1) //ע�������ID��һֱ���ڣ�������״̬��Ҳ����ʹ��
                {
                    pCON = CONGetHandle(id_rmtctrl);
                    time_rmtctrl = time(NULL);
                    if(ctrl_rmtctrl == 1)
                    {
                        pCON->order.statOrder = STATE_ORDER_WAITSTART;//״̬�����taskdata.c�ļ�
                        eRmtCtrlStat = REMOTECTRL_WAIT_START;
                    }
                    else if(ctrl_rmtctrl == 2)
                    {
                        eRmtCtrlStat = REMOTECTRL_WAIT_STOP;
                    }
                }
                break;
            case REMOTECTRL_WAIT_START:
                pCON = CONGetHandle(id_rmtctrl);
                uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                             defEventBitCONStartOK,
                                             pdFALSE, pdTRUE, 0);
                if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
                {
                    eRmtCtrlStat = REMOTECTRL_SUCC;
                }
                else
                {
                    if(time(NULL) - time_rmtctrl > 30)
                    {
                        eRmtCtrlStat = REMOTECTRL_FAIL;
                    }
                }
                break;
            case REMOTECTRL_WAIT_STOP:
                pCON = CONGetHandle(id_rmtctrl);
                uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBits & defEventBitCONStartOK) != defEventBitCONStartOK)
                {
                    eRmtCtrlStat = REMOTECTRL_SUCC;
                }
                else
                {
                    if(time(NULL) - time_rmtctrl > 30)
                    {
                        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//bugfix��ɨ����������ʧ��δ�����֤��־��������һ���ɳ�糵ֱ�ӳ��
                        eRmtCtrlStat = REMOTECTRL_FAIL;
                    }
                }
                break;
            case REMOTECTRL_SUCC:
                pCON = CONGetHandle(id_rmtctrl);
                RemoteIF_SendRemoteCtrl(pEVSE, pechProto, pCON, 1, 0); //0�� ����
                eRmtCtrlStat = REMOTECTRL_IDLE;
                break;
            case REMOTECTRL_FAIL:
                pCON = CONGetHandle(id_rmtctrl);
                uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBits & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    RemoteIF_SendRemoteCtrl(pEVSE, pechProto, pCON, 0, 3);//3, ǹδ����
                }
                else
                {
                    RemoteIF_SendRemoteCtrl(pEVSE, pechProto, pCON, 0, 4);//4�� ��������
                }
                eRmtCtrlStat = REMOTECTRL_IDLE;
                break;
            default:
                break;
            }
            /***************ʵʱ���� *******************/
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch(eRmtRTDataStat)
                {
                case REMOTERTData_IDLE:
                {
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
                    {
                        xTimerChangePeriod(pCON->status.xHandleTimerRTData,
                                           pdMS_TO_TICKS(pechProto->info.ulRTDataCyc_ms),
                                           100);//����timer period ����timer start ����
                        eRmtRTDataStat = REMOTERTData_START;
                    }
                    break;
                }
                case REMOTERTData_START: //�����ڼ䣬��Timer����Timeout�¼���RTData����ˢ��
                {
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                    if((uxBits & defEventBitCONStartOK) != defEventBitCONStartOK)
                    {
                        xTimerStop(pCON->status.xHandleTimerRTData, 100);
                        eRmtRTDataStat = REMOTERTData_STOP;
                    }
                    //�����ж�����£�����״̬����REGEDITED�£����Բ���һֱ����ʵʱ����
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
                            RemoteIF_SendRTData(pEVSE, pechProto, pCON, 2, 1);//�ֶ�ֹͣ
                            break;
                        case defOrderStopType_Full:
                            RemoteIF_SendRTData(pEVSE, pechProto, pCON, 2, 3);//����ֹͣ
                            break;
                        case defOrderStopType_Fee:
                            RemoteIF_SendRTData(pEVSE, pechProto, pCON, 2, 4);//�ﵽ�����
                            break;
                        case defOrderStopType_Scram:
                        case defOrderStopType_NetLost:
                        case defOrderStopType_Poweroff:
                        case defOrderStopType_OverCurr:
                        case defOrderStopType_Knock:
                            RemoteIF_SendRTData(pEVSE, pechProto, pCON, 2, 5);//�쳣ֹͣ
                            break;
                        default:
                            RemoteIF_SendRTData(pEVSE, pechProto, pCON, 2, 6);//����ԭ��ֹͣ
                            break;
                        }
                    }
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteRTDataOK); //����taskdata��RTData�����˶�������
                    eRmtRTDataStat = REMOTERTData_IDLE;
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
                    RemoteIF_SendRTData(pEVSE, pechProto, pCON, 1, 0);
                }
            }

            /******** ���׼�¼ ****************/
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch(eRmtOrderStat)
                {
                case REMOTEOrder_IDLE:
                    uxBits = xEventGroupGetBits(pCON->status.xHandleEventOrder);
                    if((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)
                    {
                        order_send_count = 0;
                        eRmtOrderStat = REMOTEOrder_Send;
                    }
                    break;
                case REMOTEOrder_Send:
                    RemoteIF_SendOrder(pEVSE, pechProto, pCON);
                    order_send_count++;
                    time_order = time(NULL);
                    eRmtOrderStat = REMOTEOrder_WaitRecv;
                    break;
                case REMOTEOrder_WaitRecv: //�ط��ⲿ�֣��Ȳ����Զ��ط���Ȼ���ٲ����ֶ��ط�
                    RemoteIF_RecvOrder(pEVSE, pechProto, &network_res);
                    if(network_res == 1)
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderOK);
                        uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                     defEventBitOrderFinishToRemote,
                                                     pdTRUE, pdTRUE, portMAX_DELAY);
                        if((uxBits & defEventBitOrderFinishToRemote) == defEventBitOrderFinishToRemote)
                        {
                            eRmtOrderStat = REMOTEOrder_IDLE;
                        }
                    }
                    else if(network_res == 0)
                    {
                        if(time(NULL) - time_order > 20)
                        {
                            eRmtOrderStat = REMOTEOrder_Send;
                        }
                        if(order_send_count > 3)
                        {
                            xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOrderOK);
                            uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                         defEventBitOrderFinishToRemote,
                                                         pdTRUE, pdTRUE, portMAX_DELAY);
                            if((uxBits & defEventBitOrderFinishToRemote) == defEventBitOrderFinishToRemote)
                            {
                                eRmtOrderStat = REMOTEOrder_IDLE;
                            }
                        }
                    }
                    break;
                }//switch
            }
            /******** ƽ̨�·����� ****************/
            taskremote_set(pEVSE, pechProto);


            /* ��ȡ�ʻ���Ϣ*/
            uxBits = xEventGroupWaitBits(xHandleEventRemote,
                                         defEventBitRemoteGetAccount,
                                         pdTRUE, pdFALSE, 0);
            if((uxBits & defEventBitRemoteGetAccount) == defEventBitRemoteGetAccount)
            {
                THROW_ERROR(defDevID_Cloud,
                            errcode = RemoteGetBalance(pRFIDDev->order.ucCardID,
                                                       defCardIDLength,
                                                       &(pRFIDDev->order.ucAccountStatus),
                                                       &(pRFIDDev->order.dBalance)),
                            ERR_LEVEL_CRITICAL, "Remote GetBalance");
                if(errcode == ERR_NO)
                {
                    xEventGroupSetBits(xHandleEventRemote, defEventBitRemoteGotAccount);
                }
            }
            break;//REMOTE_REGEDITED
        case REMOTE_RECONNECT:
            xTimerStop(xHandleTimerRemoteHeartbeat, 100);
            xTimerStop(xHandleTimerRemoteStatus, 100);
            pModem->state = DS_MODEM_ERR;
            vTaskDelay(1000);
            remotestat = REMOTE_NO;
            printf_safe("State Reconnect ,Call TCP close!!\n");
            break;
        }

#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
#endif //#ifndef DEBUG_NO_TASKREMOTE
        vTaskDelay(100);
    }
}
