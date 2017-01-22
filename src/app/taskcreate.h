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

extern const char *TASKNAME_CLI;
extern const char *TASKNAME_GUI;
extern const char *TASKNAME_Touch;
extern const char *TASKNAME_OTA;
extern const char *TASKNAME_EVSERemoteComm;
extern const char *TASKNAME_EVSECard;
extern const char *TASKNAME_EVSECharge;
extern const char *TASKNAME_EVSEMonitor;
extern const char *TASKNAME_EVSEError;
extern const char *TASKNAME_EVSEData;

void SysTaskCreate (void);
void AppTaskCreate (void);

#endif
