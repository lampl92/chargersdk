/**
* @file timercallback.c
* @brief 定时器回调函数
* @author rgw
* @version v1.0
* @date 2017-02-13
*/
#include "includes.h"
#include "interface.h"

/** 定时器在taskcreate中定义和创建
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
            THROW_ERROR(GetACLTemp(pPoint[i]), ERR_LEVEL_TIPS);
            THROW_ERROR(GetACNTemp(pPoint[i]), ERR_LEVEL_TIPS);
            if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
            {
                THROW_ERROR(GetBTypeConnectorTemp1(pPoint[i]), ERR_LEVEL_TIPS);
                THROW_ERROR(GetBTypeConnectorTemp2(pPoint[i]), ERR_LEVEL_TIPS);
            }
//            printf_safe("num = %d, ulIntervalOfGetTemp\n", i);
        }
    }
    if(uxTimerID == defTIMERID_LockState)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            if(pPoint[i]->info.ucConnectorType == defConnectorTypeB)
            {
                THROW_ERROR(GetBTypeConnectorLock(pPoint[i]), ERR_LEVEL_TIPS);
//            printf_safe("num = %d, ulIntervalOfGetLock\n", i);
            }
        }
    }
    if(uxTimerID == defTIMERID_CPCCState)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            THROW_ERROR(GetCCState(pPoint[i]), ERR_LEVEL_TIPS);
            THROW_ERROR(GetCPState(pPoint[i]), ERR_LEVEL_TIPS);
//            printf_safe("num = %d, defTIMERID_CPCCState\n", i);
        }
    }
    if(uxTimerID == defTIMERID_ChargingData)
    {
        for(i = 0; i < ulTotalPoint; i++)
        {
            THROW_ERROR(GetChargingVoltage(pPoint[i]), ERR_LEVEL_TIPS);
            THROW_ERROR(GetChargingCurrent(pPoint[i]), ERR_LEVEL_TIPS);
            THROW_ERROR(GetChargingFrequence(pPoint[i]), ERR_LEVEL_TIPS);
        }
    }

}
void vEVSETimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    if(uxTimerID == defTIMERID_EVSEState)
    {
        THROW_ERROR(GetScramState(pEVSE), ERR_LEVEL_CRITICAL);
        THROW_ERROR(GetPEState(pEVSE), ERR_LEVEL_TIPS);
        THROW_ERROR(GetKnockState(pEVSE), ERR_LEVEL_TIPS);
        THROW_ERROR(GetArresterState(pEVSE), ERR_LEVEL_TIPS);
        THROW_ERROR(GetPowerOffState(pEVSE), ERR_LEVEL_TIPS);
        //printf_safe("EVSE State,TimerTicks = %d\n",ulHighFrequencyTimerTicks);
    }
}

