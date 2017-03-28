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
            if((uxBits & defEventBitGotIDtoRFID) == defEventBitGotIDtoRFID)//1. 检测到卡
            {
                memmove(pRFIDDev->order.ucCardID, pRFIDDev->status.ucCardID, defCardIDLength);
                pRFIDDev->state = STATE_RFID_GOTID;
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
            for(i = 0; i < ulTotalCON; i++)  //2.判断卡是否刷过
            {
                pCON =  CONGetHandle(i);
                if(pCON->state == STATE_CON_CHARGING)
                {
                    if(memcmp(pCON->order.ucCardID, pRFIDDev->order.ucCardID, defCardIDLength) == 0)
                    {
                        //此卡已刷
#ifdef DEBUG_RFID
                        printf_safe("connector %d 此卡已刷\n", i);
#endif
                        pRFIDDev->order.ucCONID = i;
                        pRFIDDev->state = STATE_RFID_OLDID;
                        break;
                    }
                }
                else if(pCON->state == STATE_CON_IDLE)
                {
                    //未刷过卡
#ifdef DEBUG_RFID
                    printf_safe("connector %d 空闲\n", i);
#endif
                    pRFIDDev->state = STATE_RFID_NEWID;
                }
            }
            break;
        case STATE_RFID_NEWID:
            uxBits = xEventGroupSync(xHandleEventRemote,
                                     defEventBitRemoteGetAccount,
                                     defEventBitRemoteGotAccount,
                                     5000);//发送到Remote
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
            printf_safe("用户再次刷卡\n");
            printf_safe("等待HMI操作...\n");
#endif
            /** @fixme (rgw#1#): 假设用户选择停止充电 */
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//清除认证标志。
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_GOODID:
            /** @todo (rgw#1#): 1. 本任务会，通知HMI显示余额，此时如果为双枪，HMI应提示用户选择枪
                                    HMI填充好选择的枪后，发送回Order队列*/

            pRFIDDev->order.ucCONID = 0;/** @fixme (rgw#1#): 这是模拟HMI返回选择ID */
#ifdef DEBUG_RFID
            printf_safe("用户状态：");
            switch(pRFIDDev->order.ucAccountStatus)
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
            printf_safe("余额：%.2lf\n", pRFIDDev->order.dBalance);
            printf_safe("用户选择充电枪ID：%d\n", pRFIDDev->order.ucCONID);
#endif
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            xEventGroupSync(xHandleEventData,
                            defEventBitOrderTmp,
                            defEventBitOrderUpdateOK,
                            portMAX_DELAY);
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_PRECONTRACT;
            break;
        case STATE_RFID_BADID:
            /** @todo (rgw#1#): 通知HMI显示未注册 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): 通知HMI显示欠费 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */

            OrderInit(&pRFIDDev->order);
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_PRECONTRACT:
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                         defEventBitCONStartOK,
                                         pdFALSE, pdFALSE, 1000);
            if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                pRFIDDev->state = STATE_RFID_NOID;
            }
            /** @todo (rgw#1#): 监控Charge状态，如果用户未充电前终止充电流程，则返回到NOID */
            break;
        default:
            break;
        }/* switch(pRFIDDev->state)*/
#endif
#ifdef DEBUG_NO_TASKRFID
        vTaskDelay(1000);
#endif
    }/* while(1)*/
}
