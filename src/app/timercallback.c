/**
* @file timercallback.c
* @brief 定时器回调函数
* @author rgw
* @version v1.0
* @date 2017-02-13
*/
#include "includes.h"
#include "interface.h"

/**
 * @fn  void vCONTimerCB(TimerHandle_t xTimer)
 *
 * @brief   定时器在taskcreate中定义和创建
 *
 * @param   xTimer  The timer.
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
    if(uxTimerID == defTIMERID_ChargingData) //50ms
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBChargingData);
    }
}

/**
 * @fn  void vEVSETimerCB(TimerHandle_t xTimer)
 *
 * @brief   Evse timer callback
 *
 * @param   xTimer  The timer.
 */

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
    if(uxTimerID == defTIMERID_RemoteHeartbeat)//15000
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBHeartbeat);
        xEventGroupSetBits(xHandleEventTCP,defEventBitTCPClientFlushBuff);
    }
    if(uxTimerID == defTIMERID_RemoteStatus)//120000
    {
        xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBStatus);
    }
}

/**
 * @fn  void vRFIDTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Rfid timer callback
 *
 * @param   xTimer  The timer.
 */

void vRFIDTimerCB(TimerHandle_t xTimer) //500ms
{
    xEventGroupSetBits(xHandleEventTimerCBNotify, defEventBitTimerCBRFID);
}

/**
 * @fn  void vVoltTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Volt timer callback
 *
 * @param   xTimer  The timer.
 */

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

/**
 * @fn  void vCurrTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Curr timer callback
 *
 * @param   xTimer  The timer.
 */

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

/**
 * @fn  void vFreqTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Frequency timer callback
 *
 * @param   xTimer  The timer.
 */

void vFreqTimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;
    uint32_t ulTotalCON = pListCON->Total;
    CON_t *pCON = NULL;
    uint32_t i;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    for (i = 0; i < ulTotalCON; i++)
    {
        if (uxTimerID == i)
        {
            pCON = CONGetHandle(i);
            xEventGroupSetBits(pCON->status.xHandleEventException, defEventBitExceptionFreqTimer);
        }
    }
}

/**
 * @fn  void vChargeStateTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Charge state timer callback
 *
 * @param   xTimer  The timer.
 */

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

/**
 * @fn  void vRemoteRTDataTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Remote right data timer callback
 *
 * @param   xTimer  The timer.
 */

void vRemoteRTDataTimerCB(TimerHandle_t xTimer)
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
            xEventGroupSetBits(pCON->status.xHandleEventCharge, defEventBitChargeRTDataTimer);
        }
    }
}

/**
 * @fn  void vOrderTmpTimerCB(TimerHandle_t xTimer)
 *
 * @brief   Order temporary timer callback
 *
 * @param   xTimer  The timer.
 */

void vOrderTmpTimerCB(TimerHandle_t xTimer)
{
    uint32_t uxTimerID;
    uint32_t ulTotalCON = pListCON->Total;
    CON_t *pCON = NULL;
    uint32_t i;

    uxTimerID = (uint32_t)pvTimerGetTimerID(xTimer);

    for (i = 0; i < ulTotalCON; i++)
    {
        if (uxTimerID == i)
        {
            pCON = CONGetHandle(i);
            xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrderTmpTimer);
        }
    }
}
