#include "GUI_backstage.h"
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"
#include "utils.h"

double GBSBalance = 9999.9;
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
        switch (gbsstate)
        {
        case StateHome:
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
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateHome;
            }
            break;
        case StateReadyStart:
            vTaskDelay(500);
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                pCON = CONGetHandle(Temprfid_pkg.ucCONID);
                GBSBalance = Temprfid_pkg.dBalance;
                if ((Temprfid_pkg.ucAccountStatus == 1)  && (pCON->status.xPlugState == UNPLUG))
                {                  
                    gbsstate = StatePleasePlug;
                }
                if (pCON->state == STATE_CON_CHARGING)
                {
                    gbsstate = StateChargingOk;
                }
                if ((Temprfid_pkg.ucAccountStatus == 2) || (Temprfid_pkg.ucAccountStatus == 0))
                {
                    gbsstate = StateCardconditionNotOk;
                }
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateNetTimeout;
            }
            break;
        case StatePleasePlug:
            vTaskDelay(1000);
            pCON = CONGetHandle(Temprfid_pkg.ucCONID);
            if (pCON->state == STATE_CON_CHARGING)
            {
                gbsstate = StateChargingOk;
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




