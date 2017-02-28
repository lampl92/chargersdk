/**
* @file timercallback.c
* @brief ��ʱ���ص�����
* @author rgw
* @version v1.0
* @date 2017-02-13
*/
#include "includes.h"
#include "interface.h"

/** ��ʱ����taskcreate�ж���ʹ���
 */

void vChargePointTimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    if(uxTimerID == defTIMERID_Temp)    //5000 ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBTemp);
    }
    if(uxTimerID == defTIMERID_LockState)   //1000ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBLockState);
    }
    if(uxTimerID == defTIMERID_PlugState)   //50ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBPlugState);
    }
    if(uxTimerID == defTIMERID_ChargingData) //50ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData);
    }
}
void vEVSETimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    if(uxTimerID == defTIMERID_EVSEState) //50ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBEVSEState);
    }
    if(uxTimerID == defTIMERID_DATAREFRESH)//1000ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBDataRefresh);
    }
}

void vRFIDTimerCB(TimerHandle_t xTimer) //500ms
{
    xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID);
}
