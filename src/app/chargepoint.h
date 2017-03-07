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
    CP_6V,
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
} ChargePointStateType;

typedef enum
{
    STATE_VOLT_OK,
    STATE_VOLT_UPPER_Dummy,
    STATE_VOLT_UPPER,  //��ѹ
    STATE_VOLT_LOWER_Dummy,
    STATE_VOLT_LOWER,   //Ƿѹ
    STATE_VOLT_OK_Dummy
} VoltState_t;
typedef enum
{
    STATE_CURR_INIT,
    STATE_CURR_DELAY,
    STATE_CURR_OK,
    STATE_CURR_UPPER_Fix,
    STATE_CURR_UPPER_Dummy,
    STATE_CURR_ERROR
} CurrState_t;
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
} ChargePointInfo_t;

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
    EventGroupHandle_t xHandleEventCharge;
    EventGroupHandle_t xHandleEventException;
    TimerHandle_t xHandleTimerVolt;     //��ѹ״̬�жϹ�����ʹ��
    TimerHandle_t xHandleTimerCurr;     //����״̬�жϹ�����ʹ��
    TimerHandle_t xHandleTimerCharge;   //Charge״̬�жϹ�����ʹ��
    uint8_t ucRelayLState;
    uint8_t ucRelayNState;

    pChargePoint_ft GetChargingVoltage;
    pChargePoint_ft GetChargingCurrent;
    pChargePoint_ft GetChargingFrequence;
    VoltState_t xVoltStat;
    CurrState_t xCurrStat;

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
    ErrorCode_t (*SetRelay)(void *pvPoint, uint8_t cmd);

} ChargePointStatus_t;

typedef enum _ChargePointState
{
    STATE_POINT_IDLE,
    STATE_POINT_PLUGED,
    STATE_POINT_PRECONTRACT,
    STATE_POINT_PRECONTRACT_LOSEPLUG,
    STATE_POINT_STARTCHARGE,
    STATE_POINT_CHARGING,
    STATE_POINT_STOPCHARGE,
    STATE_POINT_ERROR
} ChargePointState_t;


typedef struct _ChargePoint
{
    ChargePointInfo_t info;
    ChargePointStatus_t status;
    ChargePointState_t state;
} ChargePoint_t;

ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID );
ChargePoint_t *ChargePointGetHandle(uint8_t ucChargePointID);
#endif
