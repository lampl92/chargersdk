/**
* @file taskcreate.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-17
*/
#ifndef  __TASKCREATE_H
#define  __TASKCREATE_H

#include "includes.h"
#include "timercallback.h"

#define defTIMERID_Temp             0
#define defTIMERID_LockState        1
#define defTIMERID_PlugState        2
#define defTIMERID_ChargingData     3
#define defTIMERID_EVSEState        4
#define defTIMERID_RFID             5
#define defTIMERID_DATAREFRESH      6
#define defTIMERID_Volt             7
#define defTIMERID_RemoteHeartbeat  8
#define defTIMERID_RemoteStatus     9
#define defTIMERID_RemoteRTData     10

extern const char *TASKNAME_CLI;
extern const char *TASKNAME_GUI;
extern const char *TASKNAME_Touch;
extern const char *TASKNAME_OTA;
extern const char *TASKNAME_EVSERemote;
extern const char *TASKNAME_EVSERFID;
extern const char *TASKNAME_EVSECharge;
extern const char *TASKNAME_EVSEMonitor;
extern const char *TASKNAME_EVSEDiag;
extern const char *TASKNAME_EVSEData;

extern EventGroupHandle_t xHandleEventTimerCBNotify;
extern EventGroupHandle_t xHandleEventData;
extern EventGroupHandle_t xHandleEventDiag;
extern EventGroupHandle_t xHandleEventRemote;
extern EventGroupHandle_t xHandleEventHMI;
extern EventGroupHandle_t xHandleEventLwIP;

extern TimerHandle_t xHandleTimerChargingData;
extern TimerHandle_t xHandleTimerRemoteHeartbeat;
extern TimerHandle_t xHandleTimerRemoteStatus;
extern TimerHandle_t xHandleTimerRemoteRTData;

extern QueueHandle_t xHandleQueueOrders;
extern QueueHandle_t xHandleQueueErrorPackage;

void TaskInit(void);
void SysTaskCreate (void);
void AppTaskCreate (void);
void AppObjCreate(void);

#endif
