/**
* @file interface_rfid.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#ifndef  __INTERFACE_RFID_H
#define  __INTERFACE_RFID_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "RFIDReader_mt626.h"
#include "errorcode.h"
#include "evse_config.h"
typedef struct _RFIDDevStatus
{
    uint8_t ucFoundCard;
    uint8_t ucCardID[defCardIDLength];

    ErrorCode_t (*GetCardID)(void *pvRFIDDev);
}RFIDDevStatus_t;

typedef struct _RFIDDev
{
    RFIDDevStatus_t status;
    void *com;
    SemaphoreHandle_t xHandleMutexRFID;
    EventGroupHandle_t xHandleEventGroupRFID;
}RFIDDev_t;

RFIDDev_t *RFIDDevCreate(void);



#endif
