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
#include "interface_rfid.h"

#define defConnectorTypeB   'B'
#define defConnectorTypeC   'C'

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
    double dVolatageUpperLimits;
    double dVolatageLowerLimits;
    double dACTempUpperLimits;
    double dACTempLowerLimits;
    double dConnectorTempUpperLimits;
    double dConnectorTempLowerLimits;
    double dRatedCurrent;
    double dRatedPower;                      //����һλС��

    pChargePoint_ft GetConnectorType;
    pChargePoint_ft GetVolatageUpperLimits;
    pChargePoint_ft GetVolatageLowerLimits;
    pChargePoint_ft GetACTempUpperLimits;
    pChargePoint_ft GetACTempLowerLimits;
    pChargePoint_ft GetConnectorTempUpperLimits;
    pChargePoint_ft GetConnectorTempLowerLimits;
    pChargePoint_ft GetRatedCurrent;
    pChargePoint_ft GetRatedPower;
}ChargePointInfo_t;

typedef struct _ChargePointStatus
{
    uint8_t ucHeldCardUID[defUIDLength];
    ChargePointStateType xCPState;     // ����1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    uint8_t ucLoadPercent;             // ���ذٷֱ�
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
    EventGroupHandle_t xHandleEventStartCondition;
    EventGroupHandle_t xHandleEventException;
    uint8_t ucRelayLState;
    uint8_t ucRelayNState;

    pChargePoint_ft GetChargingVoltage;
    pChargePoint_ft GetChargingCurrent;
    pChargePoint_ft GetChargingFrequence;

    pChargePoint_ft GetCPState;
    ErrorCode_t (*SetCPSwitch)(void *pvPoint, uint8_t cmd);
    ErrorCode_t (*SetLoadPercent)(void *pvPoint, uint8_t ucLoadPercent);
    pChargePoint_ft GetCCState;
    pChargePoint_ft GetPlugState;
    pChargePoint_ft GetBTypeConnectorLock;
    ErrorCode_t (*SetBTypeConnectorLock)(void *pvPoint, uint8_t cmd);
    pChargePoint_ft GetACLTemp;
    pChargePoint_ft GetACNTemp;
    pChargePoint_ft GetBTypeConnectorTemp1;
    pChargePoint_ft GetBTypeConnectorTemp2;
    pChargePoint_ft StartCharge;
    pChargePoint_ft StopCharge;
    pChargePoint_ft GetRelayState;

}ChargePointStatus_t;

typedef enum _ChargePointState
{
    POINT_IDLE,
    POINT_PLUGED,
    POINT_PRECONTRACT,
    POINT_STARTCHARGE,
    POINT_CHARGING,
    POINT_CHARGING_HALFLOAD,
    POINT_ERROR
}ChargePointState_t;


typedef struct _ChargePoint
{
    ChargePointInfo_t info;
    ChargePointStatus_t status;
    ChargePointState_t state;
} ChargePoint_t;

ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID );
ChargePoint_t *ChargePointGetHandle(uint8_t ucChargePointID);
#endif
