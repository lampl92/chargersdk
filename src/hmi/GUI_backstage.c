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
    gbsstate = HomeState;
    
    while (1)
    {
        switch (gbsstate)
        {
        case HomeState:
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                gbsstate = GetGunInfoState;
            }
            break;
        case GetGunInfoState:
            if (Tempuserlike.UserLikeFlag == 1)
            {
                xQueueSend(xHandleQueueUserChargeCondition, &(Tempuserlike.user_like), 0);
                Tempuserlike.UserLikeFlag = 0;
                gbsstate = GetBalanceState;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = HomeState;
            }
            break;
        case GetBalanceState:
            xResult = xQueueReceive(xHandleQueueRfidPkg, &Temprfid_pkg, 0);
            if (xResult == pdTRUE)
            {
                GunInfo[Tempuserlike.user_like.ucCONID].rfid_pkg = Temprfid_pkg;
                if ((Temprfid_pkg.ucAccountStatus == 1) && (Temprfid_pkg.ucCardStatus != 2))
                {                  
                    gbsstate = KnowBalanceState;
                }
                else
                {
                    gbsstate = HomeState;
                }
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = HomeState;
            }
            break;
        case KnowBalanceState:
            pCON = CONGetHandle(Tempuserlike.user_like.ucCONID);
            if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
            {
                gbsstate = HomeState;
            }
            uxBitHMI = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_TimeOut, pdTRUE, pdTRUE, 0);
            if ((uxBitHMI & defEventBitHMI_TimeOut) == defEventBitHMI_TimeOut)
            {
                gbsstate = HomeState;
            }
            break;
        }
    }
}




