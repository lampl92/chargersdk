/**
* @file connector.h
* @brief 定义充电接口需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __CONNECTOR_H
#define  __CONNECTOR_H

#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "errorcode.h"
#include "interface_rfid.h"
#include "order.h"
#include "taskremote.h"

typedef enum _CONStatusType
{
    NO_ERR = 0,
    //CP检测
    CP_ERR,
    CP_12V,
    CP_12V_PWM,
    CP_9V,
    CP_9V_PWM,
    CP_6V,
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
} CONStatusType_t;

typedef enum
{
    STATE_VOLT_OK,
    STATE_VOLT_UPPER_Dummy,
    STATE_VOLT_UPPER,  //过压
    STATE_VOLT_LOWER_Dummy,
    STATE_VOLT_LOWER,   //欠压
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
typedef ErrorCode_t (*pCONGetCfg_ft)(void *pvCON, void *pvCfgObj);
typedef ErrorCode_t (*pCONSetCfg_ft)(void *pvCON, void *pvCfgParam);
typedef ErrorCode_t (*pCon_ft)(void *pvCon);

typedef struct _CONInfo
{
    uint8_t ucCONID;                // 枪号
    uint8_t ucCONType;
    uint8_t ucSocketType;
    double dVolatageUpperLimits;
    double dVolatageLowerLimits;
    double dACTempUpperLimits;
    double dACTempLowerLimits;
    double dSocketTempUpperLimits;
    double dSocketTempLowerLimits;
    double dRatedCurrent;
    double dRatedPower;                      //保留一位小数
    uint8_t strQRCode[defQRCodeLength];

    pCONGetCfg_ft GetCONCfg;
    ErrorCode_t (*SetCONCfg)(void *pvCON, uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
//    pCONSetCfg_ft SetCONType;
//    pCONSetCfg_ft SetSocketType;
//    pCONSetCfg_ft SetVolatageUpperLimits;
//    pCONSetCfg_ft SetVolatageLowerLimits;
//    pCONSetCfg_ft SetACTempUpperLimits;
//    pCONSetCfg_ft SetACTempLowerLimits;
//    pCONSetCfg_ft SetSocketTempUpperLimits;
//    pCONSetCfg_ft SetSocketTempLowerLimits;
//    pCONSetCfg_ft SetRatedCurrent;
//    pCONSetCfg_ft SetRatedPower;
} CONInfo_t;

typedef struct _statRemote
{
    RemoteCardStatus_t card;
    RemoteOrderStatus_t order;
}statRemote_t;
typedef struct _CONStatus
{
    //uint8_t ucHeldCardID[defCardIDLength];
    CONStatusType_t xCPState;     // 检测点1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    uint8_t ucLoadPercent;        // 负载百分比
    CONStatusType_t xCCState;     // 检测点4 CC state --PE
    CONStatusType_t xPlugState;
    double dCPVolt;
    double dACLTemp;
    double dACNTemp;
    double dBTypeSocketTemp1;
    double dBTypeSocketTemp2;
    CONStatusType_t xBTypeSocketLockState; //lock unlock
    double dChargingVoltage;
    double dChargingCurrent;
    double dChargingFrequence;
    double dChargingPower;
    EventGroupHandle_t xHandleEventCharge;
    EventGroupHandle_t xHandleEventOrder;
    EventGroupHandle_t xHandleEventException;
    TimerHandle_t xHandleTimerVolt;     //电压状态判断过程中使用
    TimerHandle_t xHandleTimerCurr;     //电流状态判断过程中使用
    TimerHandle_t xHandleTimerCharge;   //Charge状态判断过程中使用
    TimerHandle_t xHandleTimerRTData;
    uint8_t ucRelayLState;
    uint8_t ucRelayNState;
    uint32_t ulSignalState;
    uint32_t ulSignalAlarm;
    uint32_t ulSignalFault;
    uint32_t ulSignalState_Old;
    uint32_t ulSignalAlarm_Old;
    uint32_t ulSignalFault_Old;

    pCon_ft GetChargingVoltage;
    pCon_ft GetChargingCurrent;
    pCon_ft GetChargingFrequence;
    pCon_ft GetChargingPower;
    VoltState_t xVoltStat;
    CurrState_t xCurrStat;

    pCon_ft GetCPState;
    ErrorCode_t (*SetCPSwitch)(void *pvCON, uint8_t cmd);
    ErrorCode_t (*SetLoadPercent)(void *pvCON, uint8_t ucLoadPercent);
    pCon_ft GetCCState;
    pCon_ft GetPlugState;
    pCon_ft GetBTypeSocketLock;
    ErrorCode_t (*SetBTypeSocketLock)(void *pvCON, uint8_t cmd);
    pCon_ft GetACLTemp;
    pCon_ft GetACNTemp;
    pCon_ft GetBTypeSocketTemp1;
    pCon_ft GetBTypeSocketTemp2;
    pCon_ft StartCharge;
    pCon_ft StopCharge;
    pCon_ft GetRelayState;
    ErrorCode_t (*SetRelay)(void *pvCON, uint8_t cmd);

    statRemote_t statRemoteProc;

} CONStatus_t;

typedef enum _CONState
{
    STATE_CON_IDLE,
    STATE_CON_PLUGED,
    STATE_CON_PRECONTRACT,
    STATE_CON_PRECONTRACT_LOSEPLUG,
    STATE_CON_STARTCHARGE,
    STATE_CON_CHARGING,
    STATE_CON_SuspendedEV,
    STATE_CON_SuspendedEVSE,
    STATE_CON_FINISHING,
    STATE_CON_STOPCHARGE,
    STATE_CON_RETURN,
    STATE_CON_ERROR
} CONState_t;


typedef struct _CON
{
    CONInfo_t info;
    CONStatus_t status;
    CONState_t state;
    OrderData_t order;
} CON_t;

CON_t *CONCreate(uint8_t ucCONID);
CON_t *CONGetHandle(uint8_t ucCONID);
CONState_t CONGetState(uint8_t ucCONID);
#endif
