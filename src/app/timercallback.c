/**
* @file timercallback.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-13
*/
#include "includes.h"

void vTimerCallback(TimerHandle_t xTimer)
{
    uint32_t ulTotalPoint = pListChargePoint->Total;
    ChargePoint_t *pPoint[ulTotalPoint];
    uint32_t uxTimerID;
    int i;
    for(i = 0; i < ulTotalPoint; i++)
    {
        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
    }
    uxTimerID = pvTimerGetTimerID(xTimer);
    if(uxTimerID == defTIMERID_Temp)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->status.dACLTemp = GetACLTemp(pPoint[i]);
            pPoint[i]->status.dACNTemp = GetACNTemp(pPoint[i]);
            pPoint[i]->status.dBTypeConnectorTemp1 = GetBTypeConnectorTemp1(pPoint[i]);
            pPoint[i]->status.dBTypeConnectorTemp2 = GetBTypeConnectorTemp2(pPoint[i]);
//            printf_safe("num = %d, ulIntervalOfGetTemp\n", i);
        }
    }
    if(uxTimerID == defTIMERID_LockState)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->status.BTypeConnectorLockState = GetBTypeConnectorLock(pPoint[i]);
//            printf_safe("num = %d, ulIntervalOfGetLock\n", i);
        }
    }
    if(uxTimerID == defTIMERID_CPCCState)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->status.ulCCState = GetCCState(pPoint[i]);
            pPoint[i]->status.ulCPState = GetCPState(pPoint[i]);
//            printf_safe("num = %d, defTIMERID_CPCCState\n", i);
        }
    }
    if(uxTimerID == defTIMERID_ChargingData)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            pPoint[i]->status.dChargingVoltage = GetChargingVoltage(pPoint[i]);
            pPoint[i]->status.dChargingCurrent = GetChargingCurrent(pPoint[i]);
            pPoint[i]->status.dChargingFrequence = GetChargingFrequence(pPoint[i]);
        }
    }
    if(uxTimerID == defTIMERID_EVSEState)
    {

    }

}
