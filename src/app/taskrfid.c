/**
* @file taskrfid.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"

//#define DEBUG_NO_TASKRFID


void vTaskEVSERFID(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBits;
    ErrorCode_t errcode;

    ulTotalCON = pListCON->Total;
    uxBits = 0;
    errcode = ERR_NO;
    while(1)
    {
#ifndef DEBUG_NO_TASKRFID
        switch(pRFIDDev->state)
        {
        case STATE_RFID_NOID:
            uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                         defEventBitGotIDtoRFID,
                                         pdTRUE, pdFALSE, portMAX_DELAY);
            if((uxBits & defEventBitGotIDtoRFID) == defEventBitGotIDtoRFID)//1. ��⵽��
            {
                memmove(pRFIDDev->order.ucCardID, pRFIDDev->status.ucCardID, defCardIDLength);
                pRFIDDev->state = STATE_RFID_GOTID;
                xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID,
                                   defEventBitGotIDtoHMI);
            }
            break;
        case STATE_RFID_GOTID:
#ifdef DEBUG_RFID
            printf_safe("im rfid task,find card, :)\n");
            printf_safe("ID = ");
            for(i = 0; i < defCardIDLength; i++)
            {
                printf_safe("%02x ", pRFIDDev->order.ucCardID[i]);
            }
            printf_safe("\n");
#endif
            for(i = 0; i < ulTotalCON; i++)  //2.�жϿ��Ƿ�ˢ��
            {
                pCON =  CONGetHandle(i);
                if(pCON->state == STATE_CON_CHARGING)
                {
                    if(memcmp(pCON->order.ucCardID, pRFIDDev->order.ucCardID, defCardIDLength) == 0)
                    {
                        //�˿���ˢ
#ifdef DEBUG_RFID
                        printf_safe("connector %d �˿���ˢ\n", i);
#endif
                        pRFIDDev->order.ucCONID = i;
                        pRFIDDev->state = STATE_RFID_OLDID;
                        break;
                    }
                }
                else if(pCON->state == STATE_CON_STOPCHARGE)
                {
                    while(1);
                }
                else if(pCON->state == STATE_CON_ERROR)
                {
                    while(1);
                }
                else// if(pCON->state == STATE_CON_IDLE)
                {
                    //δˢ����
#ifdef DEBUG_RFID
                    printf_safe("connector %d ����\n", i);
#endif
                    pRFIDDev->state = STATE_RFID_NEWID;
                }
            }
            break;
        case STATE_RFID_NEWID:
            uxBits = xEventGroupSync(xHandleEventRemote,
                                     defEventBitRemoteGetAccount,
                                     defEventBitRemoteGotAccount,
                                     5000);//���͵�Remote
            if((uxBits & defEventBitRemoteGotAccount) == defEventBitRemoteGotAccount)
            {
                if(pRFIDDev->order.ucAccountStatus != 0 && pRFIDDev->order.dBalance > 0)
                {
                    pRFIDDev->state = STATE_RFID_GOODID;
                }
                if(pRFIDDev->order.ucAccountStatus == 0)
                {
                    pRFIDDev->state = STATE_RFID_BADID;
                }
                if(pRFIDDev->order.dBalance < 0)
                {
                    pRFIDDev->state = STATE_RFID_OWE;
                }
            }
            break;
        case STATE_RFID_OLDID:
#ifdef DEBUG_RFID
            printf_safe("�û��ٴ�ˢ��\n");
            printf_safe("�ȴ�HMI����...\n");
#endif
            /** @fixme (rgw#1#): �����û�ѡ��ֹͣ��� */
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFIDStop);
//            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//�����֤��־��
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_GOODID:
            /** @todo (rgw#1#): 1. ������ᣬ֪ͨHMI��ʾ����ʱ���Ϊ˫ǹ��HMIӦ��ʾ�û�ѡ��ǹ
                                    HMI����ѡ���ǹ�󣬷��ͻ�Order����*/
            pRFIDDev->order.ucCONID = 0;/** @fixme (rgw#1#): ����ģ��HMI����ѡ��ID ,ѡ��ǹ����п���Ϣ��ʾ*/
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitGoodIDReqDisp,
                            defEventBitGoodIDReqDispOK,
                            portMAX_DELAY);

#ifdef DEBUG_RFID
            printf_safe("�û�״̬��");
            switch(pRFIDDev->order.ucAccountStatus)
            {
            case 0:
                printf_safe("δע�Ῠ\n");
                break;
            case 1:
                printf_safe("ע�Ῠ\n");
                break;
            case 2:
                printf_safe("Ƿ�ѿ�\n");
                break;
            }
            printf_safe("��%.2lf\n", pRFIDDev->order.dBalance);
            printf_safe("�û�ѡ����ǹID��%d\n", pRFIDDev->order.ucCONID);
#endif
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
//            xEventGroupSync(xHandleEventData,
//                            defEventBitOrderTmp,
//                            defEventBitOrderUpdateOK,
//                            portMAX_DELAY);
            xEventGroupSync(pCON->status.xHandleEventOrder,
                            defEventBitOrderTmp,
                            defEventBitOrderUpdateOK,
                            portMAX_DELAY);
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_HOLD;
            break;
        case STATE_RFID_BADID:
            /** @todo (rgw#1#): ֪ͨHMI��ʾδע�� */
            /** @todo (rgw#1#): �ȴ�HMI�¼�֪ͨ���� */
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitBadIDReqDisp,
                            defEventBitBadIDReqDispOK,
                            portMAX_DELAY);
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): ֪ͨHMI��ʾǷ�� */
            /** @todo (rgw#1#): �ȴ�HMI�¼�֪ͨ���� */
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitOweIDReqDisp,
                            defEventBitOwdIDReqDispOK,
                            portMAX_DELAY);
            OrderInit(&pRFIDDev->order);
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_HOLD:
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                         defEventBitCONStartOK,
                                         pdFALSE, pdFALSE, 0);
            if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                vTaskDelay(10000);
                pRFIDDev->state = STATE_RFID_NOID;
            }
            uxBits = xEventGroupWaitBits(xHandleEventHMI,
                                         defEventBitHMITimeOutToRFID,
                                         pdTRUE, pdTRUE,0);
            if((uxBits & defEventBitHMITimeOutToRFID) == defEventBitHMITimeOutToRFID)
            {
                pRFIDDev->state = STATE_RFID_NOID;
            }
            /** @todo (rgw#1#): ���Charge״̬������û�δ���ǰ��ֹ������̣��򷵻ص�NOID */
            break;
        default:
            break;
        }/* switch(pRFIDDev->state)*/
#endif
        vTaskDelay(100);
    }/* while(1)*/
}
