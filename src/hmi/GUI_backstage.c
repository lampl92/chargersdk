#include "GUI_backstage.h"
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"
#include "utils.h"
#include "touchtimer.h"
#include "evse_globals.h"

int i;//临时用
double GBSBalance;
int quitflag = 0;
GUNState_E GBSgunstate[2];//刷新各个枪的状态，给StateHome使用
GBSState_E gbsstate;
RfidQPkg_t Temprfid_pkg;//没选枪之前保存刷卡的卡号
UserLike_S Tempuserlike;

//破标志
int flagGetMoney = 0;

//枪状态刷新
static void flashGunState()
{
    CON_t *pCON;  
    for (i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        switch (pCON->state)
        {
        case STATE_CON_IDLE:
        case STATE_CON_PLUGED:
        case STATE_CON_PRECONTRACT:
        case STATE_CON_PRECONTRACT_LOSEPLUG:
        case STATE_CON_STARTCHARGE:                 
            GBSgunstate[i] = GunfreeState;
            break;
        case STATE_CON_CHARGING:
            GBSgunstate[i] = GunchargingState;
            break;
        case STATE_CON_STOPCHARGE:
        case STATE_CON_UNLOCK:
        //                    GBSgunstate[i] = GunchargedoneState;
            break;
        case STATE_CON_ERROR:
        case STATE_CON_DEV_ERROR:
            GBSgunstate[i] = Gunerror;
            break;
        case STATE_CON_RETURN: 
            break;
        }
        if (pCON->order.statOrder == STATE_ORDER_HOLD || pCON->order.statOrder == STATE_ORDER_FINISH)
        {
            GBSgunstate[i] = GunchargedoneState;
            continue;
        }
        if (pCON->status.ulSignalAlarm != 0 ||
            pCON->status.ulSignalFault != 0 ||
            pEVSE->status.ulSignalAlarm != 0 ||
            pEVSE->status.ulSignalFault != 0)
        {
            GBSgunstate[i] = Gunerror;
            continue;
        }
    }
}

//根据枪数量的不同采用不同的灯光控制
static void ledcontrl()
{
    if (pEVSE->info.ucTotalCON == 1)
    {
        Led_Show();
    }
    else
    {
        ledShow();
    } 
}

//接受刷卡消息,不阻塞
void recNewOperate()
{
    BaseType_t xResult;
    xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
    if (xResult == pdTRUE)
    {
        if (pEVSE->info.ucTotalCON == 1)
        {
            Tempuserlike.user_like.ucCONID = 0;
            Tempuserlike.user_like.dLimitFee = 0;
            Tempuserlike.user_like.dLimitEnergy = 0;
            Tempuserlike.user_like.ulLimitTime = 0;
            xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
            gbsstate = StateTestChargeCondition;
            return;
        }
        else
        {
            gbsstate = StateGetGunInfo;
            return;
        }
    }
}

//发送选择的枪和模式
void sendChose()
{
    EventBits_t uxBitHMI;
    if (Tempuserlike.UserLikeFlag == 1)
    {
        xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
        Tempuserlike.UserLikeFlag = 0;
        gbsstate = StateTestChargeCondition;
        return;
    }
    if (quitflag == 1)
    {
        Tempuserlike.user_like.HMItimeout = 1;
        xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
        Tempuserlike.user_like.HMItimeout = 0;
        quitflag = 0;
        gbsstate = StateQuit;
        return ;
    }
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StateHome;
        return ;
    }
}

//等超时回主页
void quitBackHome()
{
    EventBits_t uxBitHMI;
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StateHome;
        return ;
    }
}

void GBSTask()
{
    BaseType_t xResult;
    EventBits_t uxBitHMI;
    CON_t *pCON;   
    gbsstate = StateHome;
    while (1)
    {       
        ledcontrl();
        switch (gbsstate)
        {
        case StateHome:
            flashGunState();
            recNewOperate();
        case StateGetGunInfo:
            sendChose();
        case StateQuit:
            quitBackHome();
        case StateTestChargeCondition:
            vTaskDelay(500);
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                pCON = CONGetHandle(Temprfid_pkg.ucCONID);
                GBSBalance = Temprfid_pkg.dBalance;
                flagGetMoney = 1;
            }
            if (flagGetMoney == 1)
            {
                if ((Temprfid_pkg.ucAccountStatus == 1)  && (pCON->status.xPlugState == UNPLUG))
                {            
                    flagGetMoney = 0;
                    gbsstate = StatePleasePlug;
                    break;
                }
                if (pCON->state == STATE_CON_CHARGING)
                {
                    flagGetMoney = 0;
                    gbsstate = StateChargingOk;
                    break;
                }
                if ((Temprfid_pkg.ucAccountStatus == 2) || (Temprfid_pkg.ucAccountStatus == 0))
                {
                    flagGetMoney = 0;
                    gbsstate = StateCardconditionNotOk;
                    break;
                }
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateNetTimeout;
            }
            break;
        case StatePleasePlug:
            vTaskDelay(500);
            pCON = CONGetHandle(Temprfid_pkg.ucCONID);
            if (pCON->status.xPlugState == PLUG)
            {
                gbsstate = StatePlug;
                break;
            }
            if (quitflag == 1)
            {
                Tempuserlike.user_like.HMItimeout = 1;
                xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
                Tempuserlike.user_like.HMItimeout = 0;
                quitflag = 0;
                gbsstate = StateQuit;
                break;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StatePlugTimeout;
            }
            break;
        case StatePlug:
            vTaskDelay(1000);
            pCON = CONGetHandle(Temprfid_pkg.ucCONID);
            if (pCON->state == STATE_CON_CHARGING)
            {
                gbsstate = StateChargingOk;
                break;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StatePlugTimeout;
            }
            break;
        case StateChargingOk:
            vTaskDelay(2000);
            flashGunState();
            gbsstate = StateHome;
            break;
        case StateCardconditionNotOk:
            vTaskDelay(2000);
            flashGunState();
            gbsstate = StateHome;
            break;
        case StateNetTimeout:
            vTaskDelay(3500);
            flashGunState();
            gbsstate = StateHome;
            break;
        case StatePlugTimeout:
            vTaskDelay(3500);
            flashGunState();
            gbsstate = StateHome;
            break;
        }
        vTaskDelay(100);
    }
}




