#include "GUI_backstage.h"
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface.h"
#include "utils.h"


GBSState_E gbsstate;
OrderInfo_S GunInfo[defMaxCON];
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
                gbsstate = StatePrepareCharge;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateHome;
            }
            break;
        case StatePrepareCharge:
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                GunInfo[Tempuserlike.user_like.ucCONID].rfid_pkg = Temprfid_pkg;
                if ((Temprfid_pkg.ucAccountStatus == 1) && (Temprfid_pkg.ucCardStatus != 2))
                {                  
                    gbsstate = StateNetTimeout;
                }
                else
                {
                    gbsstate = StateHome;
                }
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateHome;
            }
            break;
        case StateNetTimeout:
            pCON = CONGetHandle(Tempuserlike.user_like.ucCONID);
            if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
            {
                gbsstate = StateHome;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = StateHome;
            }
            break;
        }
    }
}




