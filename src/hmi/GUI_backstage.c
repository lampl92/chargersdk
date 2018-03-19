#include "GUI_backstage.h"
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"
#include "utils.h"
#include "touchtimer.h"

int i;//临时用
double GBSBalance;
int quitflag = 0;
GUNState_E GBSgunstate[2];//刷新各个枪的状态，给StateHome使用
GBSState_E gbsstate;
RfidQPkg_t Temprfid_pkg;//没选枪之前保存刷卡的卡号
UserLike_S Tempuserlike;

void GBSTask()
{
    BaseType_t xResult;
    EventBits_t uxBitHMI;
    CON_t *pCON;   
    gbsstate = StateHome;
    
    while (1)
    {
        ledShow();
        switch (gbsstate)
        {
        case StateHome:
            for (i = 0; i < 2; i++)
            {
                pCON = CONGetHandle(i);
                switch (pCON->state)
                {
                case STATE_CON_IDLE:
                case STATE_CON_PLUGED:
                case STATE_CON_PRECONTRACT:
                case STATE_CON_PRECONTRACT_LOSEPLUG:
                case STATE_CON_STARTCHARGE:
                case STATE_CON_RETURN:
                case STATE_CON_UNLOCK:
                    GBSgunstate[i] = GunfreeState;
                    break;
                case STATE_CON_CHARGING:
                    GBSgunstate[i] = GunchargingState;
                    break;
                case STATE_CON_STOPCHARGE:
                    GBSgunstate[i] = GunchargedoneState;
                    break;
                case STATE_CON_ERROR:
                case STATE_CON_DEV_ERROR:
                    GBSgunstate[i] = Gunerror;
                    break;
                }
                if (pCON->status.ulSignalAlarm != 0 ||
                    pCON->status.ulSignalFault != 0 ||
                    pEVSE->status.ulSignalAlarm != 0 ||
                    pEVSE->status.ulSignalFault != 0)
                {
                    GBSgunstate[i] = Gunerror;
                }
            }
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                gbsstate = StateGetGunInfo;
            }
            break;
        case StateGetGunInfo:
            if (Tempuserlike.UserLikeFlag == 1)
            {
                xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
                Tempuserlike.UserLikeFlag = 0;
                gbsstate = StateReadyStart;
                break;
            }
            if (quitflag == 1)
            {
                Tempuserlike.user_like.HMItimeout = 1;
                xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
                quitflag = 0;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateHome;
            }
            break;
        case StateReadyStart:
            vTaskDelay(1000);
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                pCON = CONGetHandle(Temprfid_pkg.ucCONID);
                GBSBalance = Temprfid_pkg.dBalance;
                if ((Temprfid_pkg.ucAccountStatus == 1)  && (pCON->status.xPlugState == UNPLUG))
                {                  
                    gbsstate = StatePleasePlug;
                    break;
                }
                if (pCON->state == STATE_CON_CHARGING)
                {
                    gbsstate = StateChargingOk;
                    break;
                }
                if ((Temprfid_pkg.ucAccountStatus == 2) || (Temprfid_pkg.ucAccountStatus == 0))
                {
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
            vTaskDelay(1000);
            gbsstate = StateHome;
            break;
        case StateCardconditionNotOk:
            vTaskDelay(1000);
            gbsstate = StateHome;
            break;
        case StateNetTimeout:
            vTaskDelay(1000);
            gbsstate = StateHome;
            break;
        case StatePlugTimeout:
            vTaskDelay(1000);
            gbsstate = StateHome;
            break;
        }
        vTaskDelay(100);
    }
}




