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


void vTaskEVSERemote(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBits;
    RemoteState_t remotestat;
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

    ulTotalCON = pListCON->Total;
    uxBits = 0;
    uxBits = 0;
    remotestat = REMOTE_NO;//REMOTE_REGEDITED;//
    eRmtCtrlStat = REMOTECTRL_IDLE;
    eRmtRTDataStat = REMOTERTData_IDLE;
    eRmtOrderStat = REMOTEOrder_IDLE;
    errcode = 0;
    network_res = 0;
    id_rmtctrl = 0;
    ctrl_rmtctrl = 0;
    time_rmtctrl = 0;
    time_order = 0;
    order_send_count = 0;

    while(1)
    {
#ifndef DEBUG_NO_TASKREMOTE
        switch(remotestat)
        {
        case REMOTE_NO:
            /** @todo (rgw#1#): ������������ */
            uxBits = xEventGroupWaitBits(xHandleEventLwIP,
                                         defEventBitPPPup,
                                         pdFALSE, pdTRUE, portMAX_DELAY);
            if((uxBits & defEventBitPPPup) == defEventBitPPPup)
            {
                RemoteRegist(pEVSE, pechProto);
                remotestat = REMOTE_CONNECTED;
            }
            break;
        case REMOTE_CONNECTED:
            /********** ע�� **************/
            RemoteRegistRes(pEVSE, pechProto, &network_res);
            if(network_res == 1)
            {
                xTimerChangePeriod(xHandleTimerRemoteHeartbeat,
                                   pdMS_TO_TICKS(pechProto->info.ulHeartBeatCyc_ms),
                                   100);//����timer period ����timer start ����
                xTimerChangePeriod(xHandleTimerRemoteStatus,
                                   pdMS_TO_TICKS(pechProto->info.ulStatusCyc_ms),
                                   100);//����timer period ����timer start ����
                remotestat = REMOTE_REGEDITED;
            }

            break;
        case REMOTE_REGEDITED:

            /************ ���� ***************/
            uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                         defEventBitTimerCBHeartbeat,
                                         pdTRUE, pdTRUE , 0);
            if((uxBits & defEventBitTimerCBHeartbeat) == defEventBitTimerCBHeartbeat)
            {
                RemoteHeart(pEVSE, pechProto);
            }
            /************ ״̬******************/
            uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify,
                                         defEventBitTimerCBStatus,
                                         pdTRUE, pdTRUE , 0);
            if((uxBits & defEventBitTimerCBStatus) == defEventBitTimerCBStatus)
            {
                for(i = 0; i < ulTotalCON; i++)
                {
                    pCON = CONGetHandle(i);
                    RemoteStatus(pEVSE, pechProto, pCON);
                }
            }

            /*********** Զ����ͣ*******************/
            switch(eRmtCtrlStat)
            {
            case REMOTECTRL_IDLE:
                RemoteRemoteCtrlRes(pEVSE, pechProto, &id_rmtctrl, &ctrl_rmtctrl, &network_res);
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
                        eRmtCtrlStat = REMOTECTRL_FAIL;
                    }
                }
                break;
            case REMOTECTRL_SUCC:
                pCON = CONGetHandle(id_rmtctrl);
                RemoteRemoteCtrl(pEVSE, pechProto, pCON, 1, 0); //0�� ����
                eRmtCtrlStat = REMOTECTRL_IDLE;
                break;
            case REMOTECTRL_FAIL:
                pCON = CONGetHandle(id_rmtctrl);
                uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBits & defEventBitCONPlugOK) != defEventBitCONPlugOK)
                {
                    RemoteRemoteCtrl(pEVSE, pechProto, pCON, 0, 3);//3, ǹδ����
                }
                else
                {
                    RemoteRemoteCtrl(pEVSE, pechProto, pCON, 0, 4);//4�� ��������
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
                            RemoteRTData(pEVSE, pechProto, pCON, 2, 1);//�ֶ�ֹͣ
                            break;
                        case defOrderStopType_Full:
                            RemoteRTData(pEVSE, pechProto, pCON, 2, 3);//����ֹͣ
                            break;
                        case defOrderStopType_Fee:
                            RemoteRTData(pEVSE, pechProto, pCON, 2, 4);//�ﵽ�����
                            break;
                        case defOrderStopType_Scram:
                        case defOrderStopType_NetLost:
                        case defOrderStopType_Poweroff:
                        case defOrderStopType_OverCurr:
                        case defOrderStopType_Knock:
                            RemoteRTData(pEVSE, pechProto, pCON, 2, 5);//�쳣ֹͣ
                            break;
                        default:
                            RemoteRTData(pEVSE, pechProto, pCON, 2, 6);//����ԭ��ֹͣ
                            break;
                        }
                    }
                    //xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_RemoteOK); //����order����������������
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
                    RemoteRTData(pEVSE, pechProto, pCON, 1, 0);
                }
            }

            /******** ���׼�¼ ****************/
            for(i = 0; i < ulTotalCON; i++)
            {
                pCON = CONGetHandle(i);
                switch(eRmtOrderStat)
                {
                case REMOTEOrder_IDLE:
                   uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                             defEventBitOrderMakeFinish,
                                             pdFALSE, pdTRUE, 0);
                    if((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)
                    {
                        order_send_count = 0;
                        eRmtOrderStat = REMOTEOrder_Send;
                    }
                    break;
                case REMOTEOrder_Send:
                    RemoteOrder(pEVSE, pechProto, pCON);
                    order_send_count++;
                    time_order = time(NULL);
                    eRmtOrderStat = REMOTEOrder_WaitRecv;
                    break;
                case REMOTEOrder_WaitRecv: //�ط��ⲿ�֣��Ȳ����Զ��ط���Ȼ���ٲ����ֶ��ط�
                    RemoteOrderRes(pEVSE, pechProto, &network_res);
                    if(network_res == 1)
                    {
                        eRmtOrderStat = REMOTEOrder_IDLE;
                    }
                    else if(network_res == 0)
                    {
                        if(time(NULL) - time_order > 20)
                        {
                            eRmtOrderStat = REMOTEOrder_Send;
                        }
                        if(order_send_count > 3)
                        {
                            eRmtOrderStat = REMOTEOrder_IDLE;
                        }
                    }
                    break;
                }//switch
            }
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
        }

#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
#endif //#ifndef DEBUG_NO_TASKREMOTE
        vTaskDelay(100);
    }
}
