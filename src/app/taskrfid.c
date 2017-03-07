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
    STATE_RFID_NOUID,             //没有UID
    STATE_RFID_GOTUID,           //获取到UID
    STATE_RFID_OLDUID,
    STATE_RFID_NEWUID,
    STATE_RFID_GOODCARD,           //注册帐户
    STATE_RFID_BADCARD,           //非注册帐户
    STATE_RFID_OWE,                 //欠费
    STATE_RFID_PRECONTRACT           //预约状态，还没开始充电
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
            if((uxBits & defEventBitGotUIDtoRFID) == defEventBitGotUIDtoRFID)//1. 检测到卡
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
            for(i = 0; i < ulTotalPoint; i++)  //2.判断卡是否刷过
            {
                pPoint =  ChargePointGetHandle(i);
                if(memcmp(pPoint->status.ucHeldCardUID, OrderData.ucUID, defUIDLength) == 0)
                {
                    //此卡已刷
#ifdef DEBUG_RFID
                    printf_safe("point %d 此卡已刷\n", i);
#endif
                    RFIDState = STATE_RFID_OLDUID;
                    break;
                }
                else
                {
                    //未刷过卡
#ifdef DEBUG_RFID
                    printf_safe("point %d 未刷\n", i);
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
                                     1000);//发送到Remote
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
            printf_safe("用户再次刷卡\n");
            printf_safe("等待HMI操作...\n");
#endif
            /** @fixme (rgw#1#): 假设用户选择停止充电 */
            xEventGroupClearBits(pPoint->status.xHandleEventCharge, defEventBitPointAuthed);//清除认证标志。
            OrderInitData(&OrderData);
            RFIDState = STATE_RFID_NOUID;
            break;
        case STATE_RFID_GOODCARD:
            /** @todo (rgw#1#): 1. 本任务会发送Order队列到HMI，通知HMI显示余额，此时如果为双枪，HMI应提示用户选择枪
                                    HMI填充好选择的枪后，发送回Order队列*/

            OrderData.ucPointID = 0;/** @fixme (rgw#1#): 这是模拟HMI返回选择ID */
            pPoint = ChargePointGetHandle(OrderData.ucPointID);
            memmove(pPoint->status.ucHeldCardUID, OrderData.ucUID, defUIDLength);
            xEventGroupSetBits(pPoint->status.xHandleEventCharge, defEventBitPointAuthed);
            RFIDState = STATE_RFID_PRECONTRACT;
            break;
        case STATE_RFID_BADCARD:
            /** @todo (rgw#1#): 通知HMI显示未注册 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

            OrderInitData(&OrderData);
            RFIDState = STATE_RFID_NOUID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): 通知HMI显示欠费 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

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
