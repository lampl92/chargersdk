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

typedef enum _RFIDState
{
    STATE_RFID_NOUID,             //û��UID
    STATE_RFID_GOTUID,           //��ȡ��UID
    STATE_RFID_OLDUID,
    STATE_RFID_NEWUID,
    STATE_RFID_GOODCARD,           //ע���ʻ�
    STATE_RFID_BADCARD,           //��ע���ʻ�
    STATE_RFID_OWE,                 //Ƿ��
    STATE_RFID_PRECONTRACT           //ԤԼ״̬����û��ʼ���
} RFIDState_t;
void OrderInitData(OrderData_t *order)
{
    order->dBalance = 0;
    order->ucAccountStatus = 0;
    memmove(order->ucEVSESN, pEVSE->info.ucSN, defEVSESNLength);
    order->ucPointID = 0;
    memset(order->ucUID, 0, defUIDLength);
}
void vTaskEVSERFID(void *pvParameters)
{
    ChargePoint_t *pPoint = NULL;
    uint32_t ulTotalPoint;
    int i;
    EventBits_t uxBits;
    ErrorCode_t errcode;
    OrderData_t OrderData;
    RFIDState_t RFIDState;


    ulTotalPoint = pListChargePoint->Total;
    uxBits = 0;
    errcode = ERR_NO;
    RFIDState = STATE_RFID_NOUID;
    OrderInitData(&OrderData);

    while(1)
    {
        switch(RFIDState)
        {
        case STATE_RFID_NOUID:
            uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                         defEventBitGotUIDtoRFID,
                                         pdTRUE, pdFALSE, portMAX_DELAY);
            if((uxBits & defEventBitGotUIDtoRFID) == defEventBitGotUIDtoRFID)//1. ��⵽��
            {
                memmove(OrderData.ucUID, pRFIDDev->status.ucUID, defUIDLength);
                RFIDState = STATE_RFID_GOTUID;
            }
            break;
        case STATE_RFID_GOTUID:
#ifdef DEBUG_RFID
            printf_safe("im rfid task,find card, :)\n");
            printf_safe("UID = ");
            for(i = 0; i < defUIDLength; i++)
            {
                printf_safe("%x", OrderData.ucUID[i]);
            }
            printf_safe("\n");
#endif
            for(i = 0; i < ulTotalPoint; i++)  //2.�жϿ��Ƿ�ˢ��
            {
                pPoint =  ChargePointGetHandle(i);
                if(memcmp(pPoint->status.ucHeldCardUID, OrderData.ucUID, defUIDLength) == 0)
                {
                    //�˿���ˢ
#ifdef DEBUG_RFID
                    printf_safe("point %d �˿���ˢ\n", i);
#endif
                    RFIDState = STATE_RFID_OLDUID;
                    break;
                }
                else
                {
                    //δˢ����
#ifdef DEBUG_RFID
                    printf_safe("point %d δˢ\n", i);
#endif
                    RFIDState = STATE_RFID_NEWUID;
                }
            }
            break;
        case STATE_RFID_NEWUID:
            xQueueSend(xHandleQueueOrders, &OrderData, 0);
            uxBits = xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                                     defEventBitIsNewUID,
                                     defEventBitGetAccountStatus,
                                     1000);//���͵�Remote
            if((uxBits & defEventBitGetAccountStatus) == defEventBitGetAccountStatus)
            {
                xQueueReceive(xHandleQueueOrders, &OrderData, 1000);
                if(OrderData.ucAccountStatus != 0 && OrderData.dBalance > 0)
                {
                    RFIDState = STATE_RFID_GOODCARD;
                }
                if(OrderData.ucAccountStatus == 0)
                {
                    RFIDState = STATE_RFID_BADCARD;
                }
                if(OrderData.dBalance < 0)
                {
                    RFIDState = STATE_RFID_OWE;
                }
            }
            break;
        case STATE_RFID_OLDUID:
#ifdef DEBUG_RFID
            printf_safe("�û��ٴ�ˢ��\n");
            printf_safe("�ȴ�HMI����...\n");
#endif
            /** @fixme (rgw#1#): �����û�ѡ��ֹͣ��� */
            xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointAuthed);//�����֤��־��
            OrderInitData(&OrderData);
            RFIDState = STATE_RFID_NOUID;
            break;
        case STATE_RFID_GOODCARD:
            /** @todo (rgw#1#): 1. ������ᷢ��Order���е�HMI��֪ͨHMI��ʾ����ʱ���Ϊ˫ǹ��HMIӦ��ʾ�û�ѡ��ǹ
                                    HMI����ѡ���ǹ�󣬷��ͻ�Order����*/

            OrderData.ucPointID = 0;/** @fixme (rgw#1#): ����ģ��HMI����ѡ��ID */
            pPoint = ChargePointGetHandle(OrderData.ucPointID);
            memmove(pPoint->status.ucHeldCardUID, OrderData.ucUID, defUIDLength);
            xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointAuthed);
            RFIDState = STATE_RFID_PRECONTRACT;
            break;
        case STATE_RFID_BADCARD:
            /** @todo (rgw#1#): ֪ͨHMI��ʾδע�� */
            /** @todo (rgw#1#): �ȴ�HMI�¼�֪ͨ���� */

            OrderInitData(&OrderData);
            RFIDState = STATE_RFID_NOUID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): ֪ͨHMI��ʾǷ�� */
            /** @todo (rgw#1#): �ȴ�HMI�¼�֪ͨ���� */

            OrderInitData(&OrderData);
            RFIDState = STATE_RFID_NOUID;
            break;
        case STATE_RFID_PRECONTRACT:
            pPoint = ChargePointGetHandle(OrderData.ucPointID);
            uxBits = xEventGroupWaitBits(pPoint->status.xHandleEventCharge,
                                         defEventBitPointStartOK,
                                         pdTRUE, pdFALSE, 1000);
            if((uxBits & defEventBitPointStartOK) == defEventBitPointStartOK)
            {
                RFIDState = STATE_RFID_NOUID;
            }
            break;
        default:
            break;
        }/* switch(RFIDState)*/
    }/* while*/
}
