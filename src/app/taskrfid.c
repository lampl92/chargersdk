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

typedef enum _RFIDState
{
    STATE_RFID_NOID,             //û��ID
    STATE_RFID_GOTID,           //��ȡ��ID
    STATE_RFID_OLDID,
    STATE_RFID_NEWID,
    STATE_RFID_GOODID,           //ע���ʻ�
    STATE_RFID_BADID,           //��ע���ʻ�
    STATE_RFID_OWE,                 //Ƿ��
    STATE_RFID_PRECONTRACT           //ԤԼ״̬����û��ʼ���
} RFIDState_t;
void vTaskEVSERFID(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    EventBits_t uxBits;
    ErrorCode_t errcode;
    OrderData_t tmpOrderData;
    RFIDState_t RFIDState;


    ulTotalCON = pListCON->Total;
    uxBits = 0;
    errcode = ERR_NO;
    RFIDState = STATE_RFID_NOID;
    tmpOrderData.pChargeSegment = NULL;
    OrderInit(&tmpOrderData);

    while(1)
    {
#ifndef DEBUG_NO_TASKRFID
        switch(RFIDState)
        {
        case STATE_RFID_NOID:
            uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                         defEventBitGotIDtoRFID,
                                         pdTRUE, pdFALSE, portMAX_DELAY);
            if((uxBits & defEventBitGotIDtoRFID) == defEventBitGotIDtoRFID)//1. ��⵽��
            {
                memmove(tmpOrderData.ucCardID, pRFIDDev->status.ucCardID, defCardIDLength);
                RFIDState = STATE_RFID_GOTID;
            }
            break;
        case STATE_RFID_GOTID:
#ifdef DEBUG_RFID
            printf_safe("im rfid task,find card, :)\n");
            printf_safe("ID = ");
            for(i = 0; i < defCardIDLength; i++)
            {
                printf_safe("%02x ", tmpOrderData.ucCardID[i]);
            }
            printf_safe("\n");
#endif
            for(i = 0; i < ulTotalCON; i++)  //2.�жϿ��Ƿ�ˢ��
            {
                pCON =  CONGetHandle(i);
                if(pCON->state == STATE_CON_CHARGING)
                {
                    if(memcmp(pCON->order.ucCardID, tmpOrderData.ucCardID, defCardIDLength) == 0)
                    {
                        //�˿���ˢ
#ifdef DEBUG_RFID
                        printf_safe("connector %d �˿���ˢ\n", i);
#endif
                        tmpOrderData.ucCONID = i;
                        RFIDState = STATE_RFID_OLDID;
                        break;
                    }
                }
                else if(pCON->state == STATE_CON_IDLE)
                {
                    //δˢ����
#ifdef DEBUG_RFID
                    printf_safe("connector %d ����\n", i);
#endif
                    RFIDState = STATE_RFID_NEWID;
                }
            }
            break;
        case STATE_RFID_NEWID:
            xQueueSend(xHandleQueueOrders, &tmpOrderData, 0);
            uxBits = xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                                     defEventBitIsNewID,
                                     defEventBitGetAccountStatus,
                                     1000);//���͵�Remote
            if((uxBits & defEventBitGetAccountStatus) == defEventBitGetAccountStatus)
            {
                xQueueReceive(xHandleQueueOrders, &tmpOrderData, 1000);
                if(tmpOrderData.ucAccountStatus != 0 && tmpOrderData.dBalance > 0)
                {
                    RFIDState = STATE_RFID_GOODID;
                }
                if(tmpOrderData.ucAccountStatus == 0)
                {
                    RFIDState = STATE_RFID_BADID;
                }
                if(tmpOrderData.dBalance < 0)
                {
                    RFIDState = STATE_RFID_OWE;
                }
            }
            break;
        case STATE_RFID_OLDID:
#ifdef DEBUG_RFID
            printf_safe("�û��ٴ�ˢ��\n");
            printf_safe("�ȴ�HMI����...\n");
#endif
            /** @fixme (rgw#1#): �����û�ѡ��ֹͣ��� */
            pCON = CONGetHandle(tmpOrderData.ucCONID);
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//�����֤��־��
            OrderInit(&tmpOrderData);
            RFIDState = STATE_RFID_NOID;
            break;
        case STATE_RFID_GOODID:
            /** @todo (rgw#1#): 1. ������ᷢ��Order���е�HMI��֪ͨHMI��ʾ����ʱ���Ϊ˫ǹ��HMIӦ��ʾ�û�ѡ��ǹ
                                    HMI����ѡ���ǹ�󣬷��ͻ�Order����*/

            tmpOrderData.ucCONID = 0;/** @fixme (rgw#1#): ����ģ��HMI����ѡ��ID */
#ifdef DEBUG_RFID
            printf_safe("�û�״̬��");
            switch(tmpOrderData.ucAccountStatus)
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
            printf_safe("��%.2lf\n", tmpOrderData.dBalance);
            printf_safe("�û�ѡ����ǹID��%d\n", tmpOrderData.ucCONID);
#endif
            pCON = CONGetHandle(tmpOrderData.ucCONID);
            pCON->order.dBalance = tmpOrderData.dBalance;
            pCON->order.ucAccountStatus = tmpOrderData.ucAccountStatus;
            memmove(pCON->order.ucCardID, tmpOrderData.ucCardID, defCardIDLength);

            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            OrderInit(&tmpOrderData);
            RFIDState = STATE_RFID_PRECONTRACT;
            break;
        case STATE_RFID_BADID:
            /** @todo (rgw#1#): ֪ͨHMI��ʾδע�� */
            /** @todo (rgw#1#): �ȴ�HMI�¼�֪ͨ���� */

            OrderInit(&tmpOrderData);
            RFIDState = STATE_RFID_NOID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): ֪ͨHMI��ʾǷ�� */
            /** @todo (rgw#1#): �ȴ�HMI�¼�֪ͨ���� */

            OrderInit(&tmpOrderData);
            RFIDState = STATE_RFID_NOID;
            break;
        case STATE_RFID_PRECONTRACT:
            pCON = CONGetHandle(tmpOrderData.ucCONID);
            uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                         defEventBitCONStartOK,
                                         pdFALSE, pdFALSE, 1000);
            if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                RFIDState = STATE_RFID_NOID;
            }
            /** @todo (rgw#1#): ���Charge״̬������û�δ���ǰ��ֹ������̣��򷵻ص�NOID */
            break;
        default:
            break;
        }/* switch(RFIDState)*/
#endif
#ifdef DEBUG_NO_TASKRFID
        vTaskDelay(1000);
#endif
    }/* while(1)*/
}
