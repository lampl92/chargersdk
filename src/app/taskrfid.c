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
    int res; //remote 返回值;
    uint32_t remote_timeout_u100ms;
    uint8_t ucVaild = 0;//用于判断remote返回是否可以充电

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
                    else
                    {
                        OrderInit(&(pRFIDDev->order));
                        pRFIDDev->state = STATE_RFID_NOID;
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
                    //未刷过卡
#ifdef DEBUG_RFID
                    printf_safe("connector %d 空闲\n", i);
#endif
                    pRFIDDev->state = STATE_RFID_NEWID;
                }
            }
            break;
        case STATE_RFID_NEWID:
            pRFIDDev->order.ucCONID = 0;/** @fixme (rgw#1#): 这是模拟HMI返回选择ID ,选好枪后进行卡信息显示*/
//            uxBits = xEventGroupSync(xHandleEventRemote,
//                                     defEventBitRemoteGetAccount,
//                                     defEventBitRemoteGotAccount,
//                                     5000);//发送到Remote
            errcode = RemoteIF_SendCardStart(pEVSE, pechProto, pRFIDDev);
            ucVaild = 0;
            switch(errcode)
            {
            case ERR_WHITE_LIST:
                pRFIDDev->order.ucCardStatus = 1;
                ucVaild = 1;
                break;
            case ERR_BLACK_LIST:
                pRFIDDev->order.ucCardStatus = 2;
                ucVaild = 2;
                break;
            case ERR_NO:
                pRFIDDev->order.ucCardStatus = 0;
                remote_timeout_u100ms = 0;
                break;
            default:
                pRFIDDev->order.ucCardStatus = 3;
                break;
            }

            while(pRFIDDev->order.ucCardStatus == 0 && (errcode != ERR_NO || res != 1))
            {
                remote_timeout_u100ms++;
                if(remote_timeout_u100ms >= 100)//10s
                {
                    pRFIDDev->state = STATE_RFID_TIMEOUT;
                    break;
                }
                errcode = RemoteIF_RecvCardStart(pechProto, pRFIDDev, &ucVaild, &res);
                vTaskDelay(100);
            }
            
            if (pRFIDDev->state == STATE_RFID_TIMEOUT)//while超时情况的额外判断,以便退出当前case
            {
                pRFIDDev->state = STATE_RFID_NOID;
                break;
            }
            if(ucVaild == 2)//e充网定义 1 可充, 2不可充
            {
                pRFIDDev->state = STATE_RFID_NOID;
                break;
            }

//            if((uxBits & defEventBitRemoteGotAccount) == defEventBitRemoteGotAccount)
//            {
                if(pRFIDDev->order.ucAccountStatus != 0 && pRFIDDev->order.dBalance > 0)
                {
                    pRFIDDev->state = STATE_RFID_GOODID;
                }
                else if(pRFIDDev->order.ucAccountStatus == 0)
                {
                    pRFIDDev->state = STATE_RFID_BADID;
                }
                else if(pRFIDDev->order.dBalance < 0)
                {
                    pRFIDDev->state = STATE_RFID_OWE;
                }
                else
                {
                    pRFIDDev->state = STATE_RFID_NOID;
                }
//            }
            break;
        case STATE_RFID_OLDID:
#ifdef DEBUG_RFID
            printf_safe("用户再次刷卡\n");
            printf_safe("等待HMI操作...\n");
#endif
            /** @fixme (rgw#1#): 假设用户选择停止充电 */
//            xEventGroupSetBits(xHandleEventHMI,defEventBitHMI_RFIDOLD);
//
//            uxBits = xEventGroupWaitBits(xHandleEventHMI,
//                            defEventBitHMI_ChargeReqClickOK,
//                            pdTRUE, pdTRUE, 0);
//            if((uxBits & defEventBitHMI_ChargeReqClickOK) == defEventBitHMI_ChargeReqClickOK)
//            {
                //等到停止充电事件的发生
                pCON = CONGetHandle(pRFIDDev->order.ucCONID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFIDStop);
                //xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);//清除认证标志。
                OrderInit(&(pRFIDDev->order));
//                pRFIDDev->state = STATE_RFID_NOID;
                uxBits = xEventGroupWaitBits(xHandleEventHMI,
                                            defEventBitHMITimeOutToRFID,
                                            pdTRUE, pdTRUE, 0);
                if((uxBits & defEventBitHMITimeOutToRFID) == defEventBitHMITimeOutToRFID)
                {
                    xEventGroupClearBits(pRFIDDev->xHandleEventGroupRFID,
                                   defEventBitGotIDtoHMI);
                    /// TODO (zshare#1#): 增加清除位,第二次刷卡充电会直接停止 , 原因此处等待延时会一直置位 上述两个会不会有问题???????
                    xEventGroupClearBits(pCON->status.xHandleEventException,defEventBitExceptionRFIDStop);
                    pRFIDDev->state = STATE_RFID_NOID;
                }
//            }
//            else
//            {
//                //刷卡未等待停止充电事件发生
//                uxBits = xEventGroupWaitBits(xHandleEventHMI,
//                                defEventBitHMI_ChargeReqLockLcdOK,
//                                pdTRUE, pdTRUE, 0);
//                if((uxBits & defEventBitHMI_ChargeReqLockLcdOK) == defEventBitHMI_ChargeReqLockLcdOK)
//                {
//                    //等到锁屏事件
//                    pRFIDDev->state = STATE_RFID_NOID;
//                }
//            }

            break;
        case STATE_RFID_GOODID:
            /** @todo (rgw#1#): 1. 本任务会，通知HMI显示余额，此时如果为双枪，HMI应提示用户选择枪
                                    HMI填充好选择的枪后，发送回Order队列*/
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitGoodIDReqDisp,
                            defEventBitGoodIDReqDispOK,
                            portMAX_DELAY);

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
            xEventGroupSync(pCON->status.xHandleEventOrder,
                            defEventBitOrderTmp,
                            defEventBitOrderUpdateOK,
                            portMAX_DELAY);
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_HOLD;
            break;
        case STATE_RFID_BADID:
            /** @todo (rgw#1#): 通知HMI显示未注册 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitBadIDReqDisp,
                            defEventBitBadIDReqDispOK,
                            portMAX_DELAY);
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): 通知HMI显示欠费 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */
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
            /** @todo (rgw#1#): 监控Charge状态，如果用户未充电前终止充电流程，则返回到NOID */
            break;
        default:
            break;
        }/* switch(pRFIDDev->state)*/
#endif
        vTaskDelay(100);
    }/* while(1)*/
}
