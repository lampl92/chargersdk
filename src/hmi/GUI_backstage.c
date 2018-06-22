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

static void change_condition(CON_t *pCON ,int i)
{
    if (pCON->status.ulSignalAlarm != 0 ||
    pCON->status.ulSignalFault != 0 ||
    pEVSE->status.ulSignalAlarm != 0 ||
    pEVSE->status.ulSignalFault != 0)
    {
        if ((pCON->status.ulSignalAlarm != 0)&&\
            (pCON->status.ulSignalFault == 0)&&\
            ((pCON->status.ulSignalAlarm | defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault == 0))
        {
            ;
        }
        else if ((pCON->status.ulSignalAlarm != 0)&&\
            (pCON->status.ulSignalFault == 0)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault == 0)&&\
            ((pCON->status.ulSignalAlarm | defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow))
        {
            ;
        }
        else if ((pCON->status.ulSignalFault != 0)&&\
             (pCON->status.ulSignalAlarm == 0)&&\
             (pEVSE->status.ulSignalAlarm == 0)&&\
             (pEVSE->status.ulSignalFault == 0)&&\
    ((pCON->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP))
        {
            ;
        }
        else if (((pEVSE->status.ulSignalFault != 0)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pCON->status.ulSignalFault == 0)&&\
            (pCON->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault | defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID))
        {
            ;
        }
        else if (((pEVSE->status.ulSignalFault != 0)&&\
            (pEVSE->status.ulSignalAlarm == 0)&&\
            (pCON->status.ulSignalFault != 0)&&\
            (pCON->status.ulSignalAlarm == 0)&&\
            (pEVSE->status.ulSignalFault | defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)&&\
            ((pCON->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP))
        {
            ;
        }
        else
        {
            GBSgunstate[i] = Gunerror;
        }
    }
}

//枪状态刷新
static void flashGunState()
{
    CON_t *pCON;  
    for (i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON = CONGetHandle(i);
        if (((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)\
            || ((pCON->status.ulSignalState & defSignalCON_State_Stopping) == defSignalCON_State_Stopping))//在充电中
        {
            GBSgunstate[i] = GunchargingState;         
            change_condition(pCON, i);
        }
        else
        {
            GBSgunstate[i] = GunfreeState;
            change_condition(pCON, i);
        }        
        if (pCON->order.statOrder == STATE_ORDER_HOLD || pCON->order.statOrder == STATE_ORDER_FINISH\
     || pCON->order.statOrder == STATE_ORDER_WAITSTOP || pCON->order.statOrder == STATE_ORDER_WAITUSE\
    || pCON->order.statOrder == STATE_ORDER_STORE)
        {
            if (pCON->status.xPlugState == PLUG)
            {
                GBSgunstate[i] = GunchargedoneState;
            }
            else
            {
                GBSgunstate[i] = GunfreeState;
            }            
            continue;
        }      
    }
}


//接受刷卡消息,不阻塞
static void recNewOperate()
{
    BaseType_t xResult;
    flashGunState();
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

//发送选择的枪和模式或者发送密码
static void sendChoseOrWithPwd(GBSState_E state)
{
    EventBits_t uxBitHMI;
    flashGunState();
    if (Tempuserlike.UserLikeFlag == 1)
    {
        xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);       
        gbsstate = state;
        vTaskDelay(100);
        Tempuserlike.UserLikeFlag = 0;
        return;
    }
    if (quitflag == 1)
    {
        Tempuserlike.user_like.HMItimeout = 1;
        xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
        Tempuserlike.user_like.HMItimeout = 0;
        gbsstate = StateQuit;
        return ;
    }
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StateOperateTimeout;
        return ;
    }
}

//按取消按键后,等超时回主页
static void quitBackHome()
{
    EventBits_t uxBitHMI;
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        flashGunState();
        gbsstate = StateHome;
        vTaskDelay(100);
        quitflag = 0;
        return ;
    }
}

//检测是否超时
static void testTimeout()
{
    EventBits_t uxBitHMI;
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StateNetTimeout;
    }
}

//分析接受的数据
static void analyzeReceive()
{
    EventBits_t uxBitHMI;
    BaseType_t xResult;
    CON_t *pCON;
    xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
    if (xResult == pdTRUE)
    {
        pCON = CONGetHandle(Temprfid_pkg.ucCONID);
        GBSBalance = Temprfid_pkg.dBalance;
        if (gbsstate == StateTestChargeCondition)
        {
            if (Temprfid_pkg.ucNeedPwd == 1)
            {
                gbsstate = StatePwd;
                return;
            }
            if ((Temprfid_pkg.state == STATE_RFID_BADID) || (Temprfid_pkg.state == STATE_RFID_OWE))
            {                
                gbsstate = StateCardconditionNotOk;                
                return;
            }
        }
        if (gbsstate == StateTestPwd)
        {
            if (Temprfid_pkg.state == STATE_RFID_PWD)
            {
                gbsstate = StatePwdErrorAgain;
                return;
            }   
            if (Temprfid_pkg.state == STATE_RFID_PWDFULL)
            {
                gbsstate = StatePwdFull;
                return;
            }
        }
        if ((Temprfid_pkg.state == STATE_RFID_GOODID)  && (pCON->status.xPlugState == UNPLUG))
        {            
            gbsstate = StatePleasePlug;
            return;
        }
        if ((Temprfid_pkg.state == STATE_RFID_GOODID)  && (pCON->status.xPlugState == PLUG))
        {
            gbsstate = StateWaitBecomeCharge;
            return;
        }      
    }
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StateNetTimeout;
    }
}

static void WaitBecomeCharging()
{
    CON_t *pCON; 
    EventBits_t uxBitHMI;
    pCON = CONGetHandle(Temprfid_pkg.ucCONID);
    if (pCON->state == STATE_CON_CHARGING)
    {     
        gbsstate = StateChargingOk;
        return;
    }
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StateCantChargeOfDevice;
    }
}

static void pleasePlug()
{
    EventBits_t uxBitHMI;
    CON_t *pCON;   
    pCON = CONGetHandle(Temprfid_pkg.ucCONID);
    if (pCON->status.xPlugState == PLUG)
    {
        gbsstate = StateWaitBecomeCharge;
        return;
    }
    if (quitflag == 1)
    {
        Tempuserlike.user_like.HMItimeout = 1;
        xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
        Tempuserlike.user_like.HMItimeout = 0;
        gbsstate = StateQuit;
        return;
    }
    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
    if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
    {
        gbsstate = StatePlugTimeout;
        return;
    }
}

static void showResultBackHome(int time_ms)
{
    vTaskDelay(time_ms);
    flashGunState();
    gbsstate = StateHome;
}

void GBSTask()
{ 
    gbsstate = StateHome;
    while (1)
    {       
        switch (gbsstate)
        {
        case StateHome:            
            recNewOperate();
            break;
        case StateGetGunInfo:
            sendChoseOrWithPwd(StateTestChargeCondition);
            break;
        case StateQuit:
            quitBackHome();
            break;
        case StateTestChargeCondition:
            analyzeReceive();
            break;
        case StateWaitBecomeCharge:
            WaitBecomeCharging();
            break;
        case StatePwd:
            sendChoseOrWithPwd(StateTestPwd);
            break;
        case StateTestPwd:
            analyzeReceive();
            break;
        case StatePwdErrorAgain:
            vTaskDelay(2000);
            gbsstate = StatePwd;
            break;
        case StatePwdFull:
            showResultBackHome(2000);
            break;
        case StatePleasePlug:
            pleasePlug();
            break;
        case StateChargingOk:
            showResultBackHome(2000);
            break;
        case StateOperateTimeout://您的操作已超时
            showResultBackHome(2000);
            break;
        case StateCantChargeOfDevice:
            showResultBackHome(2000);
            break;
        case StateCardconditionNotOk:
            showResultBackHome(2000);
            break;
        case StateNetTimeout:
            showResultBackHome(3500);
            break;
        case StatePlugTimeout:
            showResultBackHome(3000);
            break;
        }
        vTaskDelay(100);
    }
}




