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
#include "errorcode.h"

#define defConnectorTypeB   0x0B
#define defConnectorTypeC   0x0C

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
    //��ǹ���
    PLUG,
    UNPLUG,
    //ǹ��
    LOCK,
    UNLOCK
}ChargePointStateType;

typedef ErrorCode_t (*pChargePoint_ft)(void *pvPoint);

typedef struct _ChargePointInfo
{
    uint8_t ucChargePointID;           // ǹ��
    uint8_t ucConnectorType;           //0x0B 0x0C
    uint32_t ulVolatageUpperLimits;
    uint32_t ulVolatageLowerLimits;
    uint32_t ulRatedCurrent;
    double dRatedPower;                      //����һλС��

    pChargePoint_ft GetConnectorType;
    pChargePoint_ft GetVolatageUpperLimits;
    pChargePoint_ft GetVolatageLowerLimits;
    pChargePoint_ft GetRatedCurrent;
    pChargePoint_ft GetRatedPower;
}ChargePointInfo_t;

typedef struct _ChargePointStatus
{
    ChargePointStateType xCPState;     // ����1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    ChargePointStateType xCCState;     // ����4 CC state --PE
    ChargePointStateType xPlugState;
    double dACLTemp;
    double dACNTemp;
    double dBTypeConnectorTemp1;
    double dBTypeConnectorTemp2;
    ChargePointStateType xBTypeConnectorLockState; //lock unlock
    double dChargingVoltage;
    double dChargingCurrent;
    double dChargingFrequence;
    EventGroupHandle_t xHandleEventGroupStartCharge;
    EventGroupHandle_t xHandleEventGroupStopCharge;

    pChargePoint_ft GetChargingVoltage;
    pChargePoint_ft GetChargingCurrent;
    pChargePoint_ft GetChargingFrequence;

    pChargePoint_ft GetCPState;
    pChargePoint_ft GetCCState;
    pChargePoint_ft GetPlugState;
    pChargePoint_ft GetBTypeConnectorLock;
    pChargePoint_ft SetBTypeConnectorLock;
    pChargePoint_ft GetACLTemp;
    pChargePoint_ft GetACNTemp;
    pChargePoint_ft GetBTypeConnectorTemp1;
    pChargePoint_ft GetBTypeConnectorTemp2;
    pChargePoint_ft StartCharge;
    pChargePoint_ft StopCharge;

}ChargePointStatus_t;

typedef enum _ChargePointState
{
    POINT_IDLE,
    POINT_PRECONTRACT,
    POINT_CHARGING,
    POINT_ERROR
}ChargePointState_t;


typedef struct _ChargePoint
{
    ChargePointInfo_t info;
    ChargePointStatus_t status;
    ChargePointState_t state;
} ChargePoint_t;


ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID );
#endif