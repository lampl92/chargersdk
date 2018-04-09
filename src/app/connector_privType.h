/**
 * @file    D:\DPC\workspace\Projects\chargersdk\src\app\connector_privType.h.
 *
 * @brief   Declares the connector priv type class
 */

#ifndef _CONNECTOR_PRIVTYPE_H
#define _CONNECTOR_PRIVTYPE_H

#include "stm32f4xx.h"
#include "errorcode.h"
#include "evse_config.h"
#include "evse_define.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

/**
 * @enum    _CONStatusType
 *
 * @brief   Values that represent con status types
 */

typedef enum _CONStatusType
{
    NO_ERR     = 0,
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

/**
 * @enum    VoltState_t
 *
 * @brief   Values that represent volt state ts
 */

typedef enum
{
    STATE_VOLT_OK,
    STATE_VOLT_UPPER_Dummy,
    STATE_VOLT_UPPER,  //过压
    STATE_VOLT_LOWER_Dummy,
    STATE_VOLT_LOWER,   //欠压
    STATE_VOLT_OK_Dummy
} VoltState_t;

/**
 * @enum    CurrState_t
 *
 * @brief   Values that represent curr state ts
 */

typedef enum
{
    STATE_CURR_INIT,
    STATE_CURR_DELAY,
    STATE_CURR_OK,
    STATE_CURR_UPPER_Fix,
    STATE_CURR_UPPER_Dummy,
    STATE_CURR_ERROR
} CurrState_t;

/**
 * @enum    FreqState_t
 *
 * @brief   Values that represent Frequency state ts
 */

typedef enum
{
    STATE_FREQ_OK,
    STATE_FREQ_UPPER_Dummy,
    STATE_FREQ_UPPER,
    STATE_FREQ_LOWER_Dummy,
    STATE_FREQ_LOWER,
    STATE_FREQ_OK_Dummy
} FreqState_t;

/**
 * @typedef ErrorCode_t(*pCONGetCfg_ft)(void *pvCON, void *pvCfgObj)
 *
 * @brief   Defines an alias representing the *pv configuration object
 */

typedef ErrorCode_t(*pCONGetCfg_ft)(void *pvCON, void *pvCfgObj);

/**
 * @typedef ErrorCode_t(*pCONSetCfg_ft)(void *pvCON, void *pvCfgParam)
 *
 * @brief   Defines an alias representing the *pv configuration parameter
 */

typedef ErrorCode_t(*pCONSetCfg_ft)(void *pvCON, void *pvCfgParam);

/**
 * @typedef ErrorCode_t(*pCon_ft)(void *pvCon)
 *
 * @brief   Defines an alias representing the *pv con
 */

typedef ErrorCode_t(*pCon_ft)(void *pvCon);

/**
 * @struct  _CONInfo
 *
 * @brief   Information about the con.
 */

typedef struct _CONInfo
{
    uint8_t ucCONID;    ///< 枪号
    uint8_t ucCONType;  ///< Type of the con
    uint8_t ucSocketType;   ///< Type of the socket
    double dVolatageUpperLimits;    ///< The volatage upper limits
    double dVolatageLowerLimits;    ///< The volatage lower limits
    double dACTempUpperLimits;  ///< a c temporary upper limits
    double dACTempLowerLimits;  ///< a c temporary lower limits
    double dSocketTempUpperLimits;  ///< The socket temporary upper limits
    double dSocketTempLowerLimits;  ///< The socket temporary lower limits
    double dRatedCurrent;   ///< The rated current
    double dRatedPower; ///< 保留一位小数
    uint8_t strQRCode[defQRCodeLength]; ///< The qr code[def qr code length]

    pCONGetCfg_ft GetCONCfg;    ///< The get con configuration

    /**
     * @fn  ErrorCode_t(*SetCONCfg)(void *pvCON, char *jnItemString, void *pvCfgParam, uint8_t type);
     *
     * @brief   Constructor
     *
     * @param [in,out]  pvCON           If non-null, the pv con.
     * @param [in,out]  jnItemString    If non-null, the jn item string.
     * @param [in,out]  pvCfgParam      If non-null, the pv configuration parameter.
     * @param           type            The type.
     */

    ErrorCode_t(*SetCONCfg)(void *pvCON, char *jnItemString, void *pvCfgParam, uint8_t type);
} CONInfo_t;

/**
 * @struct  _CONStatus
 *
 * @brief   A con status.
 */

typedef struct _CONStatus
{
    //uint8_t ucHeldCardID[defCardIDLength];
    CONStatusType_t xCPState;   ///< 检测点1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    uint8_t ucLoadPercent;  ///< 负载百分比
    CONStatusType_t xCCState;   ///< 检测点4 CC state --PE
    CONStatusType_t xPlugState; ///< State of the plug
    double dCPVolt; ///< The cp volt
    double dACLTemp;    ///< a cl temporary
    double dACNTemp;    ///< a cn temporary
    double dBTypeSocketTemp1;   ///< The first b type socket temporary
    double dBTypeSocketTemp2;   ///< The second b type socket temporary
    CONStatusType_t xBTypeSocketLockState;  ///< lock unlock
    double dChargingVoltage;    ///< The charging voltage
    double dChargingCurrent;    ///< The charging current
    double dChargingFrequence;  ///< The charging frequence
    double dChargingPower;  ///< The charging power
    EventGroupHandle_t xHandleEventCharge;  ///< The handle event charge
    EventGroupHandle_t xHandleEventOrder;   ///< The handle event order
    EventGroupHandle_t xHandleEventException;   ///< The handle event exception
    EventGroupHandle_t xHandleEventTimerCBNotify;   ///< The handle event timer callback notify
    TimerHandle_t xHandleTimerVolt; ///< 电压状态判断过程中使用
    TimerHandle_t xHandleTimerCurr; ///< 电流状态判断过程中使用
    TimerHandle_t xHandleTimerFreq; ///< 频率状态判断过程中使用
    TimerHandle_t xHandleTimerCharge;   ///< Charge状态判断过程中使用
    TimerHandle_t xHandleTimerRTData;   ///< Information describing the handle timer right
    uint8_t ucRelayLState;  ///< State of the relay l
    uint8_t ucRelayNState;  ///< State of the relay n
    uint32_t ulSignalState; ///< State of the ul signal
    uint32_t ulSignalAlarm; ///< The ul signal alarm
    uint32_t ulSignalFault; ///< The ul signal fault
    uint32_t ulSignalPool[CON_MAX_SIGNAL_BLOCK];    ///< The ul signal pool[ con maximum signal block]

    pCon_ft GetChargingVoltage; ///< The get charging voltage
    pCon_ft GetChargingCurrent; ///< The get charging current
    pCon_ft GetChargingFrequence;   ///< The get charging frequence
    pCon_ft GetChargingPower;   ///< The get charging power
    VoltState_t xVoltStat;  ///< The volt stat
    CurrState_t xCurrStat;  ///< The curr stat
    FreqState_t xFreqStat;  ///< The frequency stat

    pCon_ft GetCPState; ///< State of the get cp

    /**
     * @fn  ErrorCode_t(*SetCPSwitch)(void *pvCON, uint8_t cmd);
     *
     * @brief   Constructor
     *
     * @param [in,out]  pvCON   If non-null, the pv con.
     * @param           cmd     The command.
     */

    ErrorCode_t(*SetCPSwitch)(void *pvCON, uint8_t cmd);

    /**
     * @fn  ErrorCode_t(*SetLoadPercent)(void *pvCON, uint8_t ucLoadPercent);
     *
     * @brief   Constructor
     *
     * @param [in,out]  pvCON           If non-null, the pv con.
     * @param           ucLoadPercent   The load percent.
     */

    ErrorCode_t(*SetLoadPercent)(void *pvCON, uint8_t ucLoadPercent);
    pCon_ft GetCCState; ///< State of the get Cc
    pCon_ft GetPlugState;   ///< State of the get plug
    pCon_ft GetBTypeSocketLock; ///< The get b type socket lock

    /**
     * @fn  ErrorCode_t(*SetBTypeSocketLock)(void *pvCON, uint8_t cmd);
     *
     * @brief   Constructor
     *
     * @param [in,out]  pvCON   If non-null, the pv con.
     * @param           cmd     The command.
     */

    ErrorCode_t(*SetBTypeSocketLock)(void *pvCON, uint8_t cmd);
    pCon_ft GetACLTemp; ///< The get a cl temporary
    pCon_ft GetACNTemp; ///< The get a cn temporary
    pCon_ft GetBTypeSocketTemp1;    ///< The first get b type socket temporary
    pCon_ft GetBTypeSocketTemp2;    ///< The second get b type socket temporary
    pCon_ft StartCharge;    ///< The start charge
    pCon_ft StopCharge; ///< The stop charge
    pCon_ft GetRelayState;  ///< State of the get relay

    /**
     * @fn  ErrorCode_t(*SetRelay)(void *pvCON, uint8_t cmd);
     *
     * @brief   Constructor
     *
     * @param [in,out]  pvCON   If non-null, the pv con.
     * @param           cmd     The command.
     */

    ErrorCode_t(*SetRelay)(void *pvCON, uint8_t cmd);

} CONStatus_t;

/**
 * @enum    _CONState
 *
 * @brief   Values that represent con states
 */

typedef enum _CONState
{
    STATE_CON_IDLE,
    STATE_CON_PLUGED,
    STATE_CON_PRECONTRACT,
    STATE_CON_PRECONTRACT_LOSEPLUG,
    STATE_CON_STARTCHARGE,
    STATE_CON_CHARGING,
    STATE_CON_STOPCHARGE,
    STATE_CON_UNLOCK,
    STATE_CON_RETURN,
    STATE_CON_ERROR,
    STATE_CON_DEV_ERROR
} CONState_t;

#endif
