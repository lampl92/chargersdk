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

static int IsOldID(char *pCardID)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    
    ulTotalCON = pListCON->Total;
    
    for (i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        if (strcmp(pCON->order.strCardID, pCardID) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int IsNewID(void)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    
    ulTotalCON = pListCON->Total;
    
    for (i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        if (pCON->order.statOrder == STATE_ORDER_IDLE)
        {
            return i;
        }
    }
    return -1;
}

static void make_rfidpkg(RFIDDev_t *dev, RfidQPkg_t *pkg)
{
    pkg->state = dev->state;
    pkg->ucAccountStatus = dev->order.ucAccountStatus;
    pkg->ucCardStatus = dev->order.ucCardStatus;
    pkg->dBalance = dev->order.dBalance;
    pkg->ucCONID = dev->order.ucCONID;
    strcpy(pkg->strCardID, dev->order.strCardID);
}

void vTaskEVSERFID(void *pvParameters)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int id;
    EventBits_t uxBits;
    ErrorCode_t errcode;
    int res; //remote 返回值;
    uint32_t remote_timeout_u100ms;
    uint8_t ucVaild = 0;//用于判断remote返回是否可以充电
    BaseType_t xResult;
    UserChargeCondition_t user_like;
    RfidQPkg_t rfid_pkg;

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
                ul2ucTmp.ulVal = utils_ntohl(ul2ucTmp.ulVal);
                sprintf(pRFIDDev->order.strCardID, "%016u", ul2ucTmp.ulVal);
                printf_safe("CardID :%s\n", pRFIDDev->order.strCardID);
                pRFIDDev->state = STATE_RFID_GOTID;
#if EVSE_USING_RFID
                xTimerStop(xHandleTimerRFID, 100); 
#endif
            }
            break;
        case STATE_RFID_GOTID:
            // 判断卡是否刷过
            id = IsOldID(pRFIDDev->order.strCardID);
            if (id >= 0)
            {
                pRFIDDev->order.ucCONID = id;
                pRFIDDev->state = STATE_RFID_OLDID;
                break;
            }
            // 卡之前没有刷过, 判断是否有可用枪
            id = IsNewID();
            if (id >= 0)
            {
                printf_safe("有枪可以用\n");
                pRFIDDev->state = STATE_RFID_NEWID;
                break;
            }
            // 无枪可用
            pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_NEWID:
            make_rfidpkg(pRFIDDev, &rfid_pkg);
            xQueueSend(xHandleQueueRfidPkg, &rfid_pkg, 0);
            xResult = xQueueReceive(xHandleQueueUserChargeCondition, &user_like, 60000);
            if (xResult == pdTRUE)
            {
                pRFIDDev->order.ucCONID = user_like.ucCONID;
                pRFIDDev->order.dLimitFee = user_like.dLimitFee;
                pRFIDDev->order.ulLimitTime = user_like.ulLimitTime;
                pRFIDDev->order.dLimitPower = user_like.dLimitPower;
            }
            else
            {
                xEventGroupSetBits(xHandleEventHMI, defEventBitHMI_TimeOut);
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
                if(remote_timeout_u100ms >= 600)//60s
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
                pRFIDDev->state = STATE_RFID_RETURN;
            }
            break;
        case STATE_RFID_OLDID:
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            //如果是正在充电中刷卡, 则停止充电
            //如果已经停止充电, 但是没有拔枪, 则不理会
            if (pCON->order.statOrder == STATE_ORDER_UPDATE) 
            {
                uxBits = xEventGroupGetBits(pCON->status.xHandleEventException);
                if ((uxBits & defEventBitExceptionRFIDStop) != defEventBitExceptionRFIDStop)
                {
                    xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionRFIDStop);
                }
            }
            pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_GOODID:
            make_rfidpkg(pRFIDDev, &rfid_pkg);
            xQueueSend(xHandleQueueRfidPkg, &rfid_pkg, 0);
#ifdef DEBUG_RFID
            printf_safe("用户状态：");
            switch(pRFIDDev->order.ucAccountStatus)
            {
            case 0: printf_safe("未注册卡\n"); break;
            case 1: printf_safe("注册卡\n"); break;
            case 2: printf_safe("欠费卡\n"); break;
            }
            printf_safe("余额：%.2lf\n", pRFIDDev->order.dBalance);
            printf_safe("用户选择充电枪ID：%d\n", pRFIDDev->order.ucCONID);
#endif
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            xEventGroupSync(pCON->status.xHandleEventOrder,
                            defEventBitOrderTmp,
                            defEventBitOrderUpdateOK,
                            60000);
            pRFIDDev->state = STATE_RFID_HOLD;
	        pRFIDDev->status.tHoldStateStartTime = time(NULL);
            break;
        case STATE_RFID_BADID:
            make_rfidpkg(pRFIDDev, &rfid_pkg);
            xQueueSend(xHandleQueueRfidPkg, &rfid_pkg, 0);
            vTaskDelay(5000);
            pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_OWE:
            make_rfidpkg(pRFIDDev, &rfid_pkg);
            xQueueSend(xHandleQueueRfidPkg, &rfid_pkg, 0);
            vTaskDelay(5000);
            pRFIDDev->state = STATE_RFID_RETURN;
            break;
        case STATE_RFID_HOLD:
	        if (time(NULL) - pRFIDDev->status.tHoldStateStartTime > pRFIDDev->status.ulHoldMaxTime_s)
	        {
		        pRFIDDev->state = STATE_RFID_TIMEOUT;
		        break;
	        }
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
            if((uxBits & defEventBitCONStartOK) == defEventBitCONStartOK)
            {
                vTaskDelay(5000);
                pRFIDDev->state = STATE_RFID_RETURN;
            }
            break;
        case STATE_RFID_TIMEOUT:
            xEventGroupSetBits(xHandleEventHMI, defEventBitHMI_TimeOut);
	        pCON = CONGetHandle(pRFIDDev->order.ucCONID);
	        xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
	        OrderInit(&(pCON->order));
	        pRFIDDev->state = STATE_RFID_RETURN;
	        break;
        case STATE_RFID_RETURN:
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
