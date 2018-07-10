/**
 * @file    D:\DPC\workspace\Projects\chargersdk\src\app\EVSE_privType.h.
 *
 * @brief   Declares the evse priv type class
 */

#ifndef _EVSE_PRIVTYPE_H
#define _EVSE_PRIVTYPE_H

#include "stm32f4xx.h"
#include "errorcode.h"
#include "evse_config.h"
#include "gdsl_list.h"
#include "evse_define.h"
#include <time.h>

/**
 * @typedef ErrorCode_t(*pEVSEGetCfg_ft)(void *pvEVSE, void *pvCfgObj)
 *
 * @brief   Defines an alias representing the *pv configuration object
 */

typedef ErrorCode_t(*pEVSEGetCfg_ft)(void *pvEVSE, void *pvCfgObj);
//typedef ErrorCode_t (*pEVSESetCfg_ft)(void *pvEVSE, void *pvCfgParam);

/**
 * @typedef ErrorCode_t(*pEVSE_ft)(void *pvEVSE)
 *
 * @brief   Defines an alias representing the *pv evse
 */

typedef ErrorCode_t(*pEVSE_ft)(void *pvEVSE);

/**
 * @struct  _TemplSeg
 *
 * @brief   A template segment.
 */

typedef struct _TemplSeg
{
    time_t tStartTime;          ///< 用time_t表示时，忽略年月日，在时段中只关注时分秒
    time_t tEndTime;            ///< The end time
    uint8_t strStartTime[6];    ///< 只在设置时使用 "HH:MM"
    uint8_t strEndTime[6];      ///< 只在设置时使用 "HH:MM"
    double dSegFee;             ///< The segment fee
}TemplSeg_t;

/**
 * @struct  _EVSEInfo
 *
 * @brief   Information about the evse.
 */

typedef struct _EVSEInfo
{
    uint8_t strSN[defEVSESNLength + 1]; ///< 设备唯一序列号
    uint8_t ucSNLength;                 ///< Length of the serial number
    uint8_t strID[defEVSEIDLength + 1]; ///< 平台分配ID
    uint8_t ucIDLength;                 ///< Length of the identifier
    uint8_t ucType;                     ///< 1：直流设备 2：交流设备 3：交直流一体设备 4：无线设备 5：其他
    uint8_t ucTotalCON;                 ///< 一共有多少枪
    double dLng;                        ///< 经度，保留后六位
    double dLat;                        ///< 纬度，保留后六位
    uint8_t ucServiceFeeType;           ///< 0:按单， 1：按度
    double  dServiceFee;                ///< 服务费
    double dDefSegFee;                  ///< The definition segment fee
    gdsl_list_t plTemplSeg;             ///< The pl template segment

    pEVSEGetCfg_ft GetEVSECfg;          ///< The get evse configuration

    /**
     * @fn  ErrorCode_t(*SetEVSECfg)(void *pvEVSE, uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
     *
     * @brief   Constructor
     *
     * @param [in,out]  pvEVSE          If non-null, the pv evse.
     * @param [in,out]  jnItemString    If non-null, the jn item string.
     * @param [in,out]  pvCfgParam      If non-null, the pv configuration parameter.
     * @param           type            The type.
     */

    ErrorCode_t(*SetEVSECfg)(void *pvEVSE, uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
}EVSEInfo_t;

/**
 * @struct  _EVSEStatus
 *
 * @brief   An evse status.
 */

typedef struct _EVSEStatus
{
    uint32_t ulScramState;                          ///< State of the scram
    uint32_t ulKnockState;                          ///< State of the knock
    uint32_t ulPEState;                             ///< State of the PE
    uint32_t ulPowerOffState;                       ///< State of the power off
    uint32_t ulArresterState;                       ///< State of the arrester
	
    double dKnockAngle;                             ///< The knock angle
    double dAC_A_Temp_IN;                           ///< AC a temporary input
    double dAC_B_Temp_IN;                           ///< AC b temporary input
    double dAC_C_Temp_IN;                           ///< AC temporary input
    double dAC_N_Temp_IN;                           ///< AC n temporary input
	
    uint32_t ulSignalState;                         ///< State of the signal
    uint32_t ulSignalAlarm;                         ///< The signal alarm
    uint32_t ulSignalFault;                         ///< The signal fault
    uint32_t ulSignalPool[EVSE_MAX_SIGNAL_BLOCK];   ///< The signal pool[ evse maximum signal block]
    uint32_t ulTimeUpdated;
    
    pEVSE_ft GetScramState;                         ///< State of the get scram
    pEVSE_ft GetKnockState;                         ///< State of the get knock
    pEVSE_ft GetPEState;                            ///< State of the get pe
    pEVSE_ft GetPowerOffState;                      ///< State of the get power off
    pEVSE_ft GetArresterState;                      ///< State of the get arrester
    pEVSE_ft GetAC_A_Temp_in;                       ///< The get AC a temporary input
    pEVSE_ft GetAC_B_Temp_in;                       ///< The get AC b temporary input
    pEVSE_ft GetAC_C_Temp_in;                       ///< The get AC temporary input
    pEVSE_ft GetAC_N_Temp_in;                       ///< The get AC n temporary input
}EVSEStatus_t;

#endif
