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
        if (pCON->state == STATE_CON_CHARGING)
        {
            if (strcmp(pCON->order.strCardID, pCardID) == 0)
            {
                return i;
            }
        }
    }
    return -1;
}

static int IsCONAvailable(void)
{
    CON_t *pCON = NULL;
    uint32_t ulTotalCON;
    int i;
    
    ulTotalCON = pListCON->Total;
    
    for (i = 0; i < ulTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        if (pCON->state == STATE_CON_CHARGING ||
            pCON->state == STATE_CON_STOPCHARGE || 
            pCON->state == STATE_CON_ERROR || 
            pCON->state == STATE_CON_DEV_ERROR)
        {
            continue;
        }
        else if (pCON->order.statRemoteProc.rmt_ctrl.stat != REMOTECTRL_IDLE)
        {
            continue;
        }
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
    pkg->ucNeedPwd = dev->status.ucNeedPwd;
    strcpy(pkg->strCardID, dev->order.strCardID);
}

//是否可充电函数,此函数先发送充电消息到服务器,服务器返回是否可以充电和充电相关数据
//进入超时状态返回0
//可以充电返回1
//不可充电返回2
//需要密码返回3
static int canChargeOrNot()
{
    RfidQPkg_t rfid_pkg;
    UserChargeCondition_t user_like;
    BaseType_t xResult;
    ErrorCode_t errcode;
    uint32_t remote_timeout_u100ms;
    uint8_t ucVaild = 0;//用于判断remote返回是否可以充电
    int res = 0; //remote 返回值;
    make_rfidpkg(pRFIDDev, &rfid_pkg);
    xQueueSend(xHandleQueueRfidPkg, &rfid_pkg, 0);
#if EVSE_USING_GUI
    xResult = xQueueReceive(xHandleQueueUserChargeCondition, &user_like, 60000);
#else
    xResult = pdTRUE;
    user_like.ucCONID = 0;
    user_like.dLimitFee = 0;
    user_like.ulLimitTime = 0;
    user_like.dLimitEnergy = 0;
    user_like.HMItimeout = 0;
    strcpy(user_like.strPwd, "888888");
#endif
    if (xResult == pdTRUE)
    {
        if (user_like.HMItimeout == 1)
        {          
            return 0;
        }
        pRFIDDev->order.ucCONID = user_like.ucCONID;
        pRFIDDev->order.dLimitFee = user_like.dLimitFee;
        pRFIDDev->order.ulLimitTime = user_like.ulLimitTime;
        pRFIDDev->order.dLimitEnergy = user_like.dLimitEnergy;
        strcpy(pRFIDDev->status.strPwd, user_like.strPwd);
    }
    else
    {      
        return 0;
    }
    if (pRFIDDev->status.ucNeedPwd == 0)
    {
        errcode = RemoteIF_SendCardStart(pEVSE, pechProto, pRFIDDev);    
    }
    else if (pRFIDDev->status.ucNeedPwd == 1)
    {
        errcode = RemoteIF_SendCardStartPwd(pEVSE, pechProto, pRFIDDev);  
    }
    ucVaild = 0;
    switch (errcode)
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
    while (pRFIDDev->order.ucCardStatus == 0 && (errcode != ERR_NO || res != 1))
    {
        remote_timeout_u100ms++;
        if (remote_timeout_u100ms >= 600)//60s
        {
            return 0;
        }
        if ((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Logined) == defSignalEVSE_State_Network_Logined)
        {
            errcode = RemoteIF_RecvCardStart(pechProto, pRFIDDev, &ucVaild, &res);
        }
        else //如果没有联网，则直接返回超时。
        {
            return 0;
        }
        vTaskDelay(100);
    }
#if (EVSE_USING_GUI==0)
    printf_safe("用户状态：");
    switch (pRFIDDev->order.ucAccountStatus)
    {
    case 0: printf_safe("该卡不可充电\n"); break;
    case 1: printf_safe("注册卡\n"); break;
    case 2: printf_safe("该卡不可充电\n"); break;
    }
    printf_safe("余额：%.2lf\n", pRFIDDev->order.dBalance);
    printf_safe("用户选择充电枪ID：%d\n", pRFIDDev->order.ucCONID);
#endif
    if (ucVaild == 2)//e充网定义 1 可充, 2不可充
    {
        //pRFIDDev->state = STATE_RFID_BADID;
        //break;@!
        return 2;
    }
    if (ucVaild == 3)//拒绝启动,需要密码校验
    {
        //pRFIDDev->status.ucNeedPwd = 1;
        //break;@!
        return 3;
    }
    return 1;
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
    UserChargeCondition_t user_likey;
    RfidQPkg_t rfid_pkg;
    
    int resCanChargeOrNot;
    int timesPwd = 3;//输入密码次数

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
            // 判断卡是否刷过
            id = IsOldID(pRFIDDev->order.strCardID);
            if (id >= 0)
            {
                pRFIDDev->order.ucCONID = id;
                pRFIDDev->state = STATE_RFID_OLDID;
                break;
            }
            // 卡之前没有刷过, 判断是否有可用枪
            id = IsCONAvailable();
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
            resCanChargeOrNot = canChargeOrNot();
            if (resCanChargeOrNot == 3)//需要密码
            {
                pRFIDDev->status.ucNeedPwd = 1;
                pRFIDDev->state = STATE_RFID_PWD;
                break;
            }
            else if (resCanChargeOrNot == 2)//不可充电
            {
                pRFIDDev->state = STATE_RFID_BADID;
                break;
            }
            else if (resCanChargeOrNot == 1)//可以充电
            {
                if (pRFIDDev->order.dBalance <= get_current_totalfee(time(NULL)) * 0.1)
                {
                    pRFIDDev->state = STATE_RFID_OWE;//账户金额低于当前费率0.1倍算欠费
                }
                else
                {
                    pRFIDDev->state = STATE_RFID_GOODID;
                }
                break;
            }
            else//获取界面输入超时，云平台查询金额超时
            {
                pRFIDDev->state = STATE_RFID_TIMEOUT;
                break;
            }
        case STATE_RFID_PWD:
            resCanChargeOrNot = canChargeOrNot();
            if (resCanChargeOrNot == 3 || resCanChargeOrNot == 2)
            {
                timesPwd--;
                if (timesPwd == 0)
                {
                    pRFIDDev->state = STATE_RFID_PWDFULL;
                    pRFIDDev->status.ucNeedPwd = 0;
                    timesPwd = 3;
                    break; 
                }
                break;
            }
            else if (resCanChargeOrNot == 1)
            {
                pRFIDDev->state = STATE_RFID_GOODID;
                pRFIDDev->status.ucNeedPwd = 0;
                break;
            }
            else
            {
                pRFIDDev->state = STATE_RFID_TIMEOUT;
                break;
            }
        case STATE_RFID_PWDFULL:
            make_rfidpkg(pRFIDDev, &rfid_pkg);
            xQueueSend(xHandleQueueRfidPkg, &rfid_pkg, 0);
            vTaskDelay(5000);
            pRFIDDev->state = STATE_RFID_RETURN;
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
            pCON = CONGetHandle(pRFIDDev->order.ucCONID);
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
            xEventGroupSync(pCON->status.xHandleEventOrder,
                            defEventBitOrderTmp,
                            defEventBitOrderUpdateOK,
                            8000);
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
            xResult = xQueueReceive(xHandleQueueUserChargeCondition, &user_likey, 0);
            if (xResult == TRUE)
            {
                if (user_likey.HMItimeout == 1)
                {
                    pRFIDDev->state = STATE_RFID_TIMEOUT;
                }
            }
	        if (time(NULL) - pRFIDDev->status.tHoldStateStartTime > pRFIDDev->status.ulHoldMaxTime_s)
	        {
		        pRFIDDev->state = STATE_RFID_TIMEOUT;
	        }
            if (pRFIDDev->state == STATE_RFID_TIMEOUT)
            {
    	        pCON = CONGetHandle(pRFIDDev->order.ucCONID);
                xEventGroupClearBits(pCON->status.xHandleEventCharge, defEventBitCONAuthed);
    	        OrderInit(&(pCON->order));
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
	        pRFIDDev->state = STATE_RFID_RETURN;
	        break;
        case STATE_RFID_RETURN:
            pRFIDDev->status.ucNeedPwd = 0;
            OrderInit(&(pRFIDDev->order));
            memset(pRFIDDev->status.ucCardID, 0, defCardIDLength);
            memset(pRFIDDev->status.strPwd, 0, sizeof(pRFIDDev->status.strPwd));
            vTaskDelay(3000);
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
