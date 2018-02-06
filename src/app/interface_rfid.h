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
    STATE_RFID_NOID,             //没有ID
    STATE_RFID_TIMEOUT,             //超时
    STATE_RFID_GOTID,           //获取到ID
    STATE_RFID_OLDID,
    STATE_RFID_NEWID,
    STATE_RFID_GOODID,           //注册帐户
    STATE_RFID_BADID,           //非注册帐户
    STATE_RFID_OWE,                 //欠费
    STATE_RFID_HOLD,           //预约状态，还没开始充电
    STATE_RFID_RETURN
} RFIDState_t;

typedef struct _RFIDDevStatus
{
    uint8_t ucFoundCard;
    uint8_t ucCardID[defCardIDLength];
	time_t tHoldStateStartTime;
	uint32_t ulHoldMaxTime_s;

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

typedef struct _RfidQPkg
{
    char strCardID[defCardIDLength + 1];//卡号//在taskrfid中赋值
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 2:欠费 0：未注册卡
    uint8_t ucCardStatus;      //0 普通用户, 1 白名单用户, 2 黑名单用户
    double  dBalance;           //余额 
    uint8_t ucCONID;            //
    RFIDState_t state;
}RfidQPkg_t;

RFIDDev_t *RFIDDevCreate(void);



#endif
