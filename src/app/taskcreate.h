/**
* @file taskcreate.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-17
*/
#ifndef  __TASKCREATE_H
#define  __TASKCREATE_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "timercallback.h"
#include "taskmonitor.h"

#define defTIMERID_Temp             0
#define defTIMERID_LockState        1
#define defTIMERID_PlugState        2
#define defTIMERID_ChargingData     3
#define defTIMERID_EVSEState        4
#define defTIMERID_RFID             5
#define defTIMERID_DATAREFRESH      6
#define defTIMERID_RemoteHeartbeat  8
#define defTIMERID_RemoteStatus     9
#define defTIMERID_RemoteRTData     10
#define defTIMERID_StoreLog         11

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

extern TaskHandle_t xHandleTaskInit;
extern TaskHandle_t xHandleTaskCLI;
extern TaskHandle_t xHandleTaskGUI;
extern TaskHandle_t xHandleTaskGuidingLights;
extern TaskHandle_t xHandleTaskGUIBS;
extern TaskHandle_t xHandleTaskTouch;
extern TaskHandle_t xHandleTaskOTA;
extern TaskHandle_t xHandleTaskTCPClient;
extern TaskHandle_t xHandleTaskRemoteCmdProc;

extern TaskHandle_t xHandleTaskEVSERemote;
extern TaskHandle_t xHandleTaskEVSERFID;
extern TaskHandle_t xHandleTaskEVSECharge;
extern TaskHandle_t xHandleTaskEVSEMonitor;
extern TaskHandle_t xHandleTaskEVSEDiag;
extern TaskHandle_t xHandleTaskEVSEData;

extern SemaphoreHandle_t xMutexTimeStruct;
extern SemaphoreHandle_t xMeterMutex;
extern SemaphoreHandle_t xTempMutex;

extern EventGroupHandle_t xHandleEventTimerCBNotify;
extern EventGroupHandle_t xHandleEventData;
extern EventGroupHandle_t xHandleEventDiag;
extern EventGroupHandle_t xHandleEventRemote;
extern EventGroupHandle_t xHandleEventHMI;
extern EventGroupHandle_t xHandleEventTCP;

extern TimerHandle_t xHandleTimerChargingData;
extern TimerHandle_t xHandleTimerRemoteHeartbeat;
extern TimerHandle_t xHandleTimerRemoteStatus;
extern TimerHandle_t xHandleTimerRemoteRTData;
extern TimerHandle_t xHandleTimerRFID;
extern TimerHandle_t xHandleTimerStoreLog;

extern QueueHandle_t xHandleQueueRfidPkg;
extern QueueHandle_t xHandleQueueUserChargeCondition;
extern QueueHandle_t xHandleQueueOrders;
extern QueueHandle_t xHandleQueueErrorPackage;

void TaskInit(void);
void SysTaskCreate (void);
void AppTaskCreate (void);
void AppObjCreate(void);

void taskappSuspend(void);
#endif
