/**
* @file timercallback.c
* @brief 定时器回调函数
* @author rgw
* @version v1.0
* @date 2017-02-13
*/
#include "includes.h"


/** 定时器在taskcreate中创建
 */

void vChargePointTimerCB(TimerHandle_t xTimer)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    uint32_t uxTimerID;
    int i;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);
    if(uxTimerID == defTIMERID_Temp)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->state.dACLTemp = GetACLTemp(pPoint[i]);
            pPoint[i]->state.dACNTemp = GetACNTemp(pPoint[i]);
            if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
            {
                pPoint[i]->state.dBTypeConnectorTemp1 = GetBTypeConnectorTemp1(pPoint[i]);
                pPoint[i]->state.dBTypeConnectorTemp2 = GetBTypeConnectorTemp2(pPoint[i]);
            }

//            printf_safe("num = %d, ulIntervalOfGetTemp\n", i);
        }
    }
    if(uxTimerID == defTIMERID_LockState)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->state.BTypeConnectorLockState = GetBTypeConnectorLock(pPoint[i]);
//            printf_safe("num = %d, ulIntervalOfGetLock\n", i);
        }
    }
    if(uxTimerID == defTIMERID_CPCCState)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->state.ulCCState = GetCCState(pPoint[i]);
            pPoint[i]->state.ulCPState = GetCPState(pPoint[i]);
//            printf_safe("num = %d, defTIMERID_CPCCState\n", i);
        }
    }
    if(uxTimerID == defTIMERID_ChargingData)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->state.dChargingVoltage = GetChargingVoltage(pPoint[i]);
            pPoint[i]->state.dChargingCurrent = GetChargingCurrent(pPoint[i]);
            pPoint[i]->state.dChargingFrequence = GetChargingFrequence(pPoint[i]);
        }
    }

}
void vEVSETimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    if(uxTimerID == defTIMERID_EVSEState)
    {
        pEVSE->state.ulScramState = GetScramState();
        pEVSE->state.ulPEState = GetPEState();
        pEVSE->state.ulKnockState = GetKnockState();
        pEVSE->state.ulArresterState = GetArresterState();
        pEVSE->state.ulPowerOffState = GetPowerOffState();
    }
}

