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
#include "utils.h"

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
    res = 0;
    errcode = ERR_NO;
    
    if (pechProto->info.ftp.ucDownloadStart == 1)
    {
#if EVSE_USING_RFID
        xTimerStop(xHandleTimerRFID, 100); 
#endif
    }
    while (pEVSE->status.ulTimeUpdated == 0)
    {
        vTaskDelay(100);
    }
#if EVSE_USING_RFID
    xTimerStart(xHandleTimerRFID, 0);
#endif
    while(1)
    {
#ifndef DEBUG_NO_TASKRFID
        

        
        
        
        switch(pRFIDDev->state)
        {
        case STATE_RFID_NOID:
	        xEventGroupClearBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGotIDtoRFID);//bugfix：避免上次刷卡处理过程中再次检测到卡，导致充电完成后再次显示卡信息
            
            /////////////////
            uxBits = xEventGroupWaitBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID, pdTRUE, pdTRUE, 0);
            if ((uxBits & defEventBitTimerCBRFID) == defEventBitTimerCBRFID)
            {
#ifdef TEST_TIME_rfid
                printf_safe("begin %s %d\n", TEST_TIME_rfid, clock());
#endif // TEST_TIME_rfid 
                THROW_ERROR(defDevID_RFID, errcode = pRFIDDev->status.GetCardID(pRFIDDev), ERR_LEVEL_CRITICAL, "Monitor");
#ifdef TEST_TIME_rfid
                printf_safe("end %s %d\n", TEST_TIME_rfid, clock());
#endif // TEST_TIME_rfid 
                if (errcode == ERR_NO)
                {
                    pEVSE->status.ulSignalFault &= ~defSignalEVSE_Fault_RFID;
                }
                else
                {
                    pEVSE->status.ulSignalFault |= defSignalEVSE_Fault_RFID;
                }
            }
        
            /////////////////////
            
            
            
            uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                         defEventBitGotIDtoRFID,
                                         pdTRUE, pdTRUE, 0);
            if((uxBits & defEventBitGotIDtoRFID) == defEventBitGotIDtoRFID)//1. 检测到卡
            {
                ul2uc ul2ucTmp;
                ul2ucTmp.ucVal[0] = pRFIDDev->status.ucCardID[0];
                ul2ucTmp.ucVal[1] = pRFIDDev->status.ucCardID[1];
                ul2ucTmp.ucVal[2] = pRFIDDev->status.ucCardID[2];
                ul2ucTmp.ucVal[3] = pRFIDDev->status.ucCardID[3];
                //ul2ucTmp.ulVal = utils_ntohl(ul2ucTmp.ulVal);
                sprintf(pRFIDDev->order.strCardID, "%016u", ul2ucTmp.ulVal);
                printf_safe("CardID :%s\n", pRFIDDev->order.strCardID);
                pRFIDDev->state = STATE_RFID_GOTID;
#if EVSE_USING_RFID
                xTimerStop(xHandleTimerRFID, 100); 
#endif
            }
            break;
        case STATE_RFID_GOTID:
	        xEventGroupClearBits(xHandleEventHMI,defEventBitHMITimeOutToRFID);//防止其他状态产生timeout没有处理
            for(i = 0; i < ulTotalCON; i++)  //2.判断卡是否刷过
            {
                pCON =  CONGetHandle(i);
                if(pCON->state == STATE_CON_CHARGING)
                {
                    if(strcmp(pCON->order.strCardID, pRFIDDev->order.strCardID) == 0)
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
                        pRFIDDev->state = STATE_RFID_RETURN;
                    }
                }
                else if(pCON->state == STATE_CON_STOPCHARGE)
                {
                    pRFIDDev->state = STATE_RFID_RETURN;
                    //while(1);
                }
                else if(pCON->state == STATE_CON_ERROR)
                {
                    pRFIDDev->state = STATE_RFID_RETURN;
	               //while(1);
                }
                else// if(pCON->state == STATE_CON_IDLE)
                {
                    //未刷过卡
#ifdef DEBUG_RFID
                    printf_safe("connector %d 空闲\n", i);
#endif
					xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID,
									   defEventBitGotIDtoHMI);
                    pRFIDDev->state = STATE_RFID_NEWID;
                }
            }
            break;
        case STATE_RFID_NEWID:
            pRFIDDev->order.ucCONID = 0;/** @fixme (rgw#1#): 这是模拟HMI返回选择ID ,选好枪后进行卡信息显示*/
            pRFIDDev->order.dLimitFee = 0;
            pRFIDDev->order.ulLimitTime = 0;
            
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupClearBits(pCON->status.xHandleEventException, defEventBitExceptionRFIDStop);//fix：防止结束时再次刷卡产生多余的刷卡停止标志
            if (pCON->order.statRemoteProc.rmt_ctrl.stat != REMOTECTRL_IDLE)
            {
                printf_safe("APP正在启动充电!!!!\n");
                vTaskDelay(2000);
                pRFIDDev->state = STATE_RFID_RETURN;
                break;
            }
            if (pCON->order.statOrder != STATE_ORDER_IDLE)
            {
                printf_safe("该接口有未完成订单!!!!\n");
                vTaskDelay(2000);
                pRFIDDev->state = STATE_RFID_RETURN;
                break;
            }
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
            errcode = ERR_REMOTE_NODATA;//初始化errorcode ；
            res = 0;
            while(pRFIDDev->order.ucCardStatus == 0 && (errcode != ERR_NO || res != 1))
            {
                remote_timeout_u100ms++;
                if(remote_timeout_u100ms >= 300)//30s
                {
                    pRFIDDev->state = STATE_RFID_TIMEOUT;
                    break;
                }
                errcode = RemoteIF_RecvCardStart(pechProto, pRFIDDev, &ucVaild, &res);
                vTaskDelay(100);
            }
            
            if (pRFIDDev->state == STATE_RFID_TIMEOUT)//while超时情况的额外判断,以便退出当前case
            {
                break;
            }
            if(ucVaild == 2)//e充网定义 1 可充, 2不可充
            {
                pRFIDDev->state = STATE_RFID_BADID;
                break;
            }

            if(pRFIDDev->order.ucAccountStatus != 0 && pRFIDDev->order.dBalance > 0)
            {
                if (pRFIDDev->order.dBalance <= get_current_totalfee(time(NULL)) * 0.1)
                {
                    pRFIDDev->state = STATE_RFID_OWE;
                }
                else
                {
                    pRFIDDev->state = STATE_RFID_GOODID;
                }
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
                pRFIDDev->state = STATE_RFID_RETURN;
            }
	        //卡信息界面立即点退出
	        uxBits = xEventGroupWaitBits(xHandleEventHMI,
		        defEventBitHMITimeOutToRFID,
		        pdTRUE,
		        pdTRUE,
		        0);
	        if ((uxBits & defEventBitHMITimeOutToRFID) == defEventBitHMITimeOutToRFID)
	        {
		        pRFIDDev->state = STATE_RFID_TIMEOUT;
		        break;
	        }
            break;
        case STATE_RFID_OLDID:
#ifdef DEBUG_RFID
            printf_safe("用户再次刷卡\n");
            printf_safe("等待HMI操作...\n");
#endif
            /** @fixme (rgw#1#): 假设用户选择停止充电 */
                pCON = CONGetHandle(pRFIDDev->order.ucCONID);
                xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFIDStop);
                pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_GOODID:
            /** @todo (rgw#1#): 1. 本任务会，通知HMI显示余额，此时如果为双枪，HMI应提示用户选择枪
                                    HMI填充好选择的枪后，发送回Order队列*/
#if EVSE_USING_GUI
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitGoodIDReqDisp,
                            defEventBitGoodIDReqDispOK,
                            10000);
#endif
			//卡信息界面立即点退出
	        uxBits = xEventGroupWaitBits(xHandleEventHMI,
		        defEventBitHMITimeOutToRFID,
		        pdTRUE,
		        pdTRUE,
		        0);
	        if ((uxBits & defEventBitHMITimeOutToRFID) == defEventBitHMITimeOutToRFID)
	        {
		        pRFIDDev->state = STATE_RFID_TIMEOUT;
		        break;
	        }

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
	        //此时如果没有进行充电，应在HMI倒计时完成后退出到IDLE状态
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            xEventGroupSync(pCON->status.xHandleEventOrder,
                            defEventBitOrderTmp,
                            defEventBitOrderUpdateOK,
                            portMAX_DELAY);
            pRFIDDev->state = STATE_RFID_HOLD;
	        pRFIDDev->status.tHoldStateStartTime = time(NULL);
            break;
        case STATE_RFID_BADID:
            /** @todo (rgw#1#): 通知HMI显示未注册 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */
#if EVSE_USING_GUI
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitBadIDReqDisp,
                            defEventBitBadIDReqDispOK,
                            portMAX_DELAY);
#endif
            OrderInit(&(pRFIDDev->order));
            pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_OWE:
            /** @todo (rgw#1#): 通知HMI显示欠费 */
            /** @todo (rgw#1#): 等待HMI事件通知结束 */
#if EVSE_USING_GUI
            xEventGroupSync(pRFIDDev->xHandleEventGroupRFID,
                            defEventBitOweIDReqDisp,
                            defEventBitOwdIDReqDispOK,
                            portMAX_DELAY);
#endif
            OrderInit(&pRFIDDev->order);
            pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_HOLD:
	        if (time(NULL) - pRFIDDev->status.tHoldStateStartTime > pRFIDDev->status.ulHoldMaxTime_s)
	        {
		        pRFIDDev->state = STATE_RFID_TIMEOUT;
		        break;
	        }
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
                                         defEventBitCONStartOK,
                                         pdFALSE, pdFALSE, 0);
            if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                vTaskDelay(10000);
                pRFIDDev->state = STATE_RFID_RETURN;
            }
            uxBits = xEventGroupWaitBits(xHandleEventHMI,
                                         defEventBitHMITimeOutToRFID,
                                         pdTRUE, pdTRUE,0);
            if((uxBits & defEventBitHMITimeOutToRFID) == defEventBitHMITimeOutToRFID)
            {
                pRFIDDev->state = STATE_RFID_TIMEOUT;
            }
            /** @todo (rgw#1#): 监控Charge状态，如果用户未充电前终止充电流程，则返回到NOID */
            break;
        case STATE_RFID_TIMEOUT:
	        pCON = CONGetHandle(pRFIDDev->order.ucCONID);
	        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
	        OrderInit(&(pCON->order));
	        pRFIDDev->state = STATE_RFID_RETURN;
	        break;
        case STATE_RFID_RETURN:
            xEventGroupClearBits(pRFIDDev->xHandleEventGroupRFID,
                                defEventBitGotIDtoHMI);
            OrderInit(&(pRFIDDev->order));
            memset(pRFIDDev->status.ucCardID, 0, defCardIDLength);
#if EVSE_USING_RFID
            xTimerStart(xHandleTimerRFID, 100); 
#endif
            pRFIDDev->state = STATE_RFID_NOID;
            break;
        default:
            break;
        }/* switch(pRFIDDev->state)*/
#endif
        vTaskDelay(100);
    }/* while(1)*/
}
