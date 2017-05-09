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

void vCONTimerCB(TimerHandle_t xTimer)
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
    if(uxTimerID == defTIMERID_Volt)   //50ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBVolt);
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
        xEventGroupSetBits(xHandleEventlwIP, defEventBitTCPClientSendReq);
    }
    if(uxTimerID == defTIMERID_Heartbeat)//5000ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBHeartbeat);
    }
}

void vRFIDTimerCB(TimerHandle_t xTimer) //500ms
{
    xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID);
}

void vVoltTimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;
    uint32_t ulTotalCON = pListCON->Total;
    CON_t *pCON = NULL;
    uint32_t i;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    for(i = 0; i < ulTotalCON; i++)
    {
        if(uxTimerID == i)
        {
            pCON = CONGetHandle(i);
            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionVoltTimer);
        }
    }
}
void vCurrTimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;
    uint32_t ulTotalCON = pListCON->Total;
    CON_t *pCON = NULL;
    uint32_t i;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    for(i = 0; i < ulTotalCON; i++)
    {
        if(uxTimerID == i)
        {
            pCON = CONGetHandle(i);
            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionCurrTimer);
        }
    }
}
void vChargeStateTimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;
    uint32_t ulTotalCON = pListCON->Total;
    CON_t *pCON = NULL;
    uint32_t i;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    for(i = 0; i < ulTotalCON; i++)
    {
        if(uxTimerID == i)
        {
            pCON = CONGetHandle(i);
            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionChargeTimer);
        }
    }
}
