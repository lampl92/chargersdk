/**
* @file taskdata.c
* @brief �����ļ���д����������
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
    THROW_ERROR(defDevID_File, CreateOrderFile(), ERR_LEVEL_WARNING, "<taskdata> Create Order.txt");//����order.txt
    while(1)
    {
#ifndef DEBUG_NO_TASKDATA
        /* �������� */
        //1. �ȴ�ˢ������¼�
        for(i = 0; i < ulTotalCON; i++)
        {
            pCON = CONGetHandle(i);
            uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                             defEventBitOrderTmp,   //RFID�з������¼�
                                             pdTRUE, pdFALSE, 0);
            if((uxBitsData & defEventBitOrderTmp) == defEventBitOrderTmp)
            {
                pCON->order.ucStartType = 4; //�п�
                pCON = CONGetHandle(pRFIDDev->order.ucCONID);
                pCON->order.statOrder = STATE_ORDER_TMP;
            }
        }
        /** @todo (rgw#1#): !!! ����û����ɨ�������жϡ�Ŀǰ��ɨ����ܵ�������紦��pCON->order.statOrder = STATE_ORDER_WAITSTART; */

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
                //2. �ȴ�StartCharge�¼�
                uxBitsCharge = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                                   defEventBitCONStartOK,
                                                   pdFALSE, pdFALSE, 0);
                if((uxBitsCharge & defEventBitCONStartOK) == defEventBitCONStartOK)
                {
                    pCON->order.statOrder = STATE_ORDER_MAKE;
                }
                break;
            case STATE_ORDER_MAKE:
                //3. ��ʼ���ʱ����׼��
                makeOrder(pCON);
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeOK);//Ŀǰ��û�еط���
                pCON->order.statOrder = STATE_ORDER_UPDATE;
                break;
            case STATE_ORDER_UPDATE:
                //4. ���³������
                /** ��ȡ�뿪Update����������Finish״̬ */
                uxBitsCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
                if((uxBitsCharge & defEventBitCONStartOK) != defEventBitCONStartOK)
                {
                    pCON->order.statOrder = STATE_ORDER_FINISH;
                }
                else
                {
                    makeOrder(pCON);
                }

                if(pCON->order.dLimitFee != 0) //0 ʱ��ʾ�Զ���������0��ֹͣ���
                {
                    if(pCON->order.dTotalFee >= pCON->order.dLimitFee) // �ﵽ�����
                    {
                        xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionLimitFee);
                    }
                }
                break;
            case STATE_ORDER_FINISH:
                //5. �������
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
                if((uxBitsData & defEventBitOrderStopTypeLimitFee) == defEventBitOrderStopTypeLimitFee)    //�ﵽ���������
                {
                    pCON->order.ucStopType = defOrderStopType_Fee;
                }
                if((uxBitsData & defEventBitOrderStopTypeRemoteStop) == defEventBitOrderStopTypeRemoteStop)    //Զ��ֹͣ
                {
                    pCON->order.ucStopType = defOrderStopType_Remote;
                }
                if((uxBitsData & defEventBitOrderStopTypeRFIDStop) == defEventBitOrderStopTypeRFIDStop)    //ˢ��ֹͣ
                {
                    pCON->order.ucStopType = defOrderStopType_RFID;
                }
                if((uxBitsData & defEventBitOrderStopTypeFull) == defEventBitOrderStopTypeFull)    //�Զ�����
                {
                    pCON->order.ucStopType = defOrderStopType_Full;
                }
                xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);

                /** @todo (rgw#1#): �洢���� */

                uxBitsData = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                                 defEventBitOrderUseless,
                                                 pdTRUE, pdTRUE, 0);
                if((uxBitsData & defEventBitOrderUseless) == defEventBitOrderUseless)
                {
                    xEventGroupClearBits(pCON->status.xHandleEventOrder, defEventBitOrderMakeFinish);
                    /* @todo (rgw#1): ������洢����*/
                    xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderFinishToRemote);
                    xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONOrderFinish);
                    OrderInit(&(pCON->order));//״̬��ΪIDLE
                }
                break;
            }
        }


        /* ��ȡ�ļ����� */
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
        /* end of ��ȡ�ļ����� */

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
