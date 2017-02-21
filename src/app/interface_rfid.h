/**
* @file interface_rfid.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#ifndef  __INTERFACE_RFID_H
#define  __INTERFACE_RFID_H

#include "RFIDReader_mt626.h"
#include "errorcode.h"

typedef struct _RFIDDevStatus
{
    uint8_t ucFoundCard;
    uint8_t ucUID[4];

    ErrorCode_t (*GetUID)(void *pvRFIDDev);
}RFIDDevStatus_t;

typedef struct _RFIDDev
{
    RFIDDevStatus_t status;
    void *com;
}RFIDDev_t;

RFIDDev_t *RFIDDevCreate(void);



#endif
