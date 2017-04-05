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
#include "order.h"

typedef enum _RFIDState
{
    STATE_RFID_NOID,             //û��ID
    STATE_RFID_GOTID,           //��ȡ��ID
    STATE_RFID_OLDID,
    STATE_RFID_NEWID,
    STATE_RFID_GOODID,           //ע���ʻ�
    STATE_RFID_BADID,           //��ע���ʻ�
    STATE_RFID_OWE,                 //Ƿ��
    STATE_RFID_HOLD           //ԤԼ״̬����û��ʼ���
} RFIDState_t;

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
    RFIDState_t state;
    OrderData_t order;
}RFIDDev_t;

RFIDDev_t *RFIDDevCreate(void);



#endif
