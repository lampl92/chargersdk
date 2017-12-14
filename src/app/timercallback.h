/**
* @file timercallback.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-07
*/
#ifndef  __TIMERCALLBACK_H
#define  __TIMERCALLBACK_H

void vCONTimerCB(TimerHandle_t xTimer);
void vEVSETimerCB(TimerHandle_t xTimer);
void vRFIDTimerCB(TimerHandle_t xTimer);
void vVoltTimerCB(TimerHandle_t xTimer);
void vCurrTimerCB(TimerHandle_t xTimer);
void vChargeStateTimerCB(TimerHandle_t xTimer);
void vRemoteRTDataTimerCB(TimerHandle_t xTimer);
void vOrderTmpTimerCB(TimerHandle_t xTimer);

#endif
