/**
* @file chargepoint.h
* @brief ������С��絥Ԫ��Ҫ�����������
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __CHARGEPOINT_H
#define  __CHARGEPOINT_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "event_groups.h"

typedef enum __ChargePointStateType
{
    NO_ERR = 0,
    //CP���
    CP_ERR,
    CP_12V,
    CP_9V,
    CP_9V_PWM,
    CP_6V_PWM,
    //CC���
    CC_PE,
    CC_NO,
    //ǹ��
    LOCK,
    UNLOCK
}ChargePointStateType;

typedef struct _ChargePoint
{
    uint8_t  ucChargePointID;           // ǹ��
    ChargePointStateType uiCPState;     // ����1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    ChargePointStateType uiCCState;     // ����4 CC state --PE
    //EventGroupHandle_t xHandleEventGroupConn
    EventGroupHandle_t xHandleEventGroupStartCharge;
    EventGroupHandle_t xHandleEventGroupStopCharge;
    ChargePointStateType (*GetCPState)(uint8_t  ucChargePointID);
    ChargePointStateType (*GetCCState)(uint8_t  ucChargePointID);
    uint32_t (*GetACLTemp)(uint8_t  ucChargePointID);
    uint32_t (*GetACNTemp)(uint8_t  ucChargePointID);
    uint32_t (*GetBTypeChargePointTemp1)(uint8_t  ucChargePointID);
    uint32_t (*GetBTypeChargePointTemp2)(uint8_t  ucChargePointID);
    ChargePointStateType (*LockBTypeChargePoint)(uint8_t  ucChargePointID);
    ChargePointStateType (*GetBTypeChargePointState)(uint8_t  ucChargePointID); //lock unlock
    ChargePointStateType (*StartCharge)(uint8_t  ucChargePointID);
    ChargePointStateType (*StopCharge)(uint8_t  ucChargePointID);
} ChargePoint_t;

#endif
