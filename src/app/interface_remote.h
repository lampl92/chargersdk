/**
* @file interface_remote.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-16
*/
#ifndef  __INTERFACE_REMOTE_H
#define  __INTERFACE_REMOTE_H

#include <time.h>

ErrorCode_t RemoteGetTime(struct tm *pTimeBlock);
ErrorCode_t RemoteGetBalance(uint8_t *pucUID, uint8_t ucUIDLength, uint8_t *pucAccountStatus, double *pdBalance);

#endif
