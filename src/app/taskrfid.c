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
    STATE_RFID_NOID,             //没有ID
    STATE_RFID_GOTID,           //获取到ID
    STATE_RFID_OLDID,
    STATE_RFID_NEWID,
    STATE_RFID_GOODID,           //注册帐户
    STATE_RFID_BADID,           //非注册帐户
    STATE_RFID_OWE,                 //欠费
    STATE_RFID_PRECONTRACT           //预约状态，还没开始充电
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
            if((uxBits & defEventBitGotIDtoRFID) == defEventBitGotIDtoRFID)//1. 检测到卡
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
            for(i = 0; i < ulTotalCON; i++)  //2.判断卡是否刷过
            {
                pCON =  CONGetHandle(i);
                if(pCON->state == STATE_CON_CHARGING)
                {
                    if(memcmp(pCON->order.ucCardID, tmpOrderData.ucCardID, defCardIDLength) == 0)
                    {
                        //此卡已刷
#ifdef DEBUG_RFID
                        printf_safe("connector %d 此卡已刷\n", i);
#endif
                        tmpOrderData.ucCONID = i;
                        RFIDState = STATE_RFID_OLDID;
                        break;
                    }
                }
                else if(pCON->state == STATE_CON_IDLE)
                {
                    //未刷过卡
#ifdef DEBUG_RFID
                    printf_safe("connector %d 空闲\n", i);
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
                                     1000);//发送到Remote
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
            printf_safe("用户再次刷卡\n");
            printf_safe("等待HMI操作...\n");
#endif
            /** @fixme (rgw#1#): 假设用户选择停止充电 */
            pCON = CONGetHandle(tmpOrderData.ucCONID);
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//清除认证标志。
            OrderInit(&tmpOrderData);
            RFIDState = STATE_RFID_NOID;
            break;
        case STATE_RFID_GOODID:
            /** @todo (rgw#1#): 1. 本任务会发送Order队列到HMI，通知HMI显示余额，此时如果为双枪，HMI应提示用户选择枪
                                    HMI填充好选择的枪后，发送回Order队列*/

            tmpOrderData.ucCONID = 0;/** @fixme (rgw#1#): 这是模拟HMI返回选择ID */
#ifdef DEBUG_RFID
            printf_safe("用户状态：");
            switch(tmpOrderData.ucAccountStatus)
            {
            case 0:
                printf_safe("未注册卡\n");
                break;
            case 1:
                printf_safe("注册卡\n");
                break;
            case 2:
                printf_safe("欠费卡\n");
                break;
            }
            printf_safe("余额：%.2lf\n", tmpOrderData.dBalance);
            printf_safe("用户选择充电枪ID：%d\n", tmpOrderData.ucCONID);
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
            /** @todo (rgw#1#): 通知HMI显示未注册 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

            OrderInit(&tmpOrderData);
            RFIDState = STATE_RFID_NOID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): 通知HMI显示欠费 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

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
            /** @todo (rgw#1#): 监控Charge状态，如果用户未充电前终止充电流程，则返回到NOID */
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
