/**
* @file chargepoint.h
* @brief 定义最小充电单元需要的数据与操作
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
    //CP检测
    CP_ERR,
    CP_12V,
    CP_9V,
    CP_9V_PWM,
    CP_6V_PWM,
    //CC检测
    CC_PE,
    CC_NO,
    //插枪检测
    PLUG,
    UNPLUG,
    //枪锁
    LOCK,
    UNLOCK
}ChargePointStateType;

typedef struct _ChargePointInfo
{
    uint8_t ucChargePointID;           // 枪号
    uint8_t ucConnectorType;           //0x0B 0x0C
    uint32_t ulVolatageUpperLimits;
    uint32_t ulVolatageLowerLimits;
    uint32_t ulCurrent;
    uint32_t ulPower;
}ChargePointInfo_t;

typedef struct _ChargePointState
{
    ChargePointStateType xCPState;     // 检测点1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    ChargePointStateType xCCState;     // 检测点4 CC state --PE
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
}ChargePointState_t;

typedef struct _ChargePoint
{
    ChargePointInfo_t info;
    ChargePointState_t state;

} ChargePoint_t;

ErrorCode_t GetChargingVoltage(ChargePoint_t *pPoint);
ErrorCode_t GetChargingCurrent(ChargePoint_t *pPoint);
ErrorCode_t GetChargingFrequence(ChargePoint_t *pPoint);

ErrorCode_t GetCPState(ChargePoint_t *pPoint);
ErrorCode_t GetCCState(ChargePoint_t *pPoint);
ErrorCode_t GetPlugState(ChargePoint_t *pPoint);
ErrorCode_t GetBTypeConnectorLock(ChargePoint_t *pPoint); //lock unlock
ErrorCode_t SetBTypeConnectorLock(ChargePoint_t *pPoint);
ErrorCode_t GetACLTemp(ChargePoint_t *pPoint);
ErrorCode_t GetACNTemp(ChargePoint_t *pPoint);
ErrorCode_t GetBTypeConnectorTemp1(ChargePoint_t *pPoint);
ErrorCode_t GetBTypeConnectorTemp2(ChargePoint_t *pPoint);
ErrorCode_t StartCharge(ChargePoint_t *pPoint);
ErrorCode_t StopCharge(ChargePoint_t *pPoint);

ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID );
#endif
