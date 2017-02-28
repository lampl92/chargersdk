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
    STATE_NOUID,             //没有UID
    STATE_GOTUID,           //获取到UID
    STATE_OLDUID,
    STATE_NEWUID,
    STATE_GOODCARD,           //注册帐户
    STATE_BADCARD,           //非注册帐户
    STATE_OWE,                 //欠费
    STATE_PRECONTRACT           //预约状态，还没开始充电
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
    int i;
    EventBits_t uxBits;
    ErrorCode_t errcode;
    ChargePoint_t *pPoint[2];
    uint8_t ucTotal;
    OrderData_t OrderData;
    RFIDState_t RFIDState;

    OrderInitData(&OrderData);
    ucTotal = pEVSE->info.ucTotalPoint;
    for(i = 0; i < ucTotal; i++)
    {
        pPoint[i] =  ChargePointGetHandle(i);
    }
    uxBits = 0;
    errcode = ERR_NO;
    RFIDState = STATE_NOUID;
    while(1)
    {
        switch(RFIDState)
        {
        case STATE_NOUID:
            uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                         defEventBitGotUIDtoRFID,
                                         pdTRUE, pdFALSE, portMAX_DELAY);
            if((uxBits & defEventBitGotUIDtoRFID) == defEventBitGotUIDtoRFID)//1. 检测到卡
            {
                memmove(OrderData.ucUID, pRFIDDev->status.ucUID, defUIDLength);
                RFIDState = STATE_GOTUID;
            }
            break;
        case STATE_GOTUID:
#ifdef DEBUG_RFID
            printf_safe("im rfid task,find card, :)\n");
            printf_safe("UID = ");
            for(i = 0; i < defUIDLength; i++)
            {
                printf_safe("%x", OrderData.ucUID[i]);
            }
            printf_safe("\n");
#endif
            for(i = 0; i < pEVSE->info.ucTotalPoint; i++)  //2.判断卡是否刷过
            {
                if(memcmp(pPoint[i]->status.ucHeldCardUID, OrderData.ucUID, defUIDLength) == 0)
                {
                    //此卡已刷
#ifdef DEBUG_RFID
                    printf_safe("point %d 此卡已刷\n", i);
#endif
                    RFIDState = STATE_OLDUID;
                    break;
                }
                else
                {
                    //未刷过卡
#ifdef DEBUG_RFID
                    printf_safe("point %d 未刷\n", i);
#endif
                    RFIDState = STATE_NEWUID;
                }
            }
            break;
        case STATE_NEWUID:
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
                    RFIDState = STATE_GOODCARD;
                }
                if(OrderData.ucAccountStatus == 0)
                {
                    RFIDState = STATE_BADCARD;
                }
                if(OrderData.dBalance < 0)
                {
                    RFIDState = STATE_OWE;
                }
            }
            break;
        case STATE_OLDUID:
            OrderInitData(&OrderData);//fixme
            RFIDState = STATE_NOUID;
            break;
        case STATE_GOODCARD:
            /** @todo (rgw#1#): 1. 本任务会发送Order队列到HMI，通知HMI显示余额，此时如果为双枪，HMI应提示用户选择枪
                                    HMI填充好选择的枪后，发送回Order队列*/

            OrderData.ucPointID = 0;/** @fixme (rgw#1#): 这是模拟HMI返回选择ID */
            pPoint[OrderData.ucPointID]->state = POINT_PRECONTRACT;
            memmove(pPoint[OrderData.ucPointID]->status.ucHeldCardUID, OrderData.ucUID, defUIDLength);
            xEventGroupSetBits(pPoint[OrderData.ucPointID]->status.xHandleEventStartCondition, defEventBitStdAuthed);
            RFIDState = STATE_PRECONTRACT;
            break;
        case STATE_BADCARD:
            /** @todo (rgw#1#): 通知HMI显示未注册 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

            OrderInitData(&OrderData);
            RFIDState = STATE_NOUID;
            break;
        case STATE_OWE:
            /** @todo (rgw#1#): 通知HMI显示欠费 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

            OrderInitData(&OrderData);
            RFIDState = STATE_NOUID;
            break;
        case STATE_PRECONTRACT:
            uxBits = xEventGroupWaitBits(pPoint[OrderData.ucPointID]->status.xHandleEventStartCondition,
                                         defEventBitStdOK,
                                         pdTRUE, pdFALSE, 1000);
            if((uxBits & defEventBitStdOK) == defEventBitStdOK)
            {

            }
            break;
        default:
            break;
        }
    }
}
