
#ifndef _EVSE_PRIVTYPE_H
#define _EVSE_PRIVTYPE_H

#include "stm32f4xx.h"
#include "errorcode.h"
#include "evse_config.h"
#include "gdsl_list.h"
#include "evse_define.h"
#include <time.h>

typedef ErrorCode_t(*pEVSEGetCfg_ft)(void *pvEVSE, void *pvCfgObj);
//typedef ErrorCode_t (*pEVSESetCfg_ft)(void *pvEVSE, void *pvCfgParam);
typedef ErrorCode_t(*pEVSE_ft)(void *pvEVSE);
typedef struct _TemplSeg
{
    time_t tStartTime;//用time_t表示时，忽略年月日，在时段中只关注时分秒
    time_t tEndTime;
    uint8_t strStartTime[6];//只在设置时使用 "HH:MM"
    uint8_t strEndTime[6];//只在设置时使用 "HH:MM"
    double dSegFee;
}TemplSeg_t;
typedef struct _EVSEInfo
{
    uint8_t strSN[defEVSESNLength + 1]; //设备唯一序列号
    uint8_t ucSNLength;
    uint8_t strID[defEVSEIDLength + 1]; //平台分配ID
    uint8_t ucIDLength;
    uint8_t ucType;   //1：直流设备 2：交流设备 3：交直流一体设备 4：无线设备 5：其他
    uint8_t ucTotalCON; //一共有多少枪
    double dLng;      //经度，保留后六位
    double dLat;      //纬度，保留后六位
    uint8_t ucServiceFeeType;//0:按单， 1：按度
    double  dServiceFee;                //服务费
    double dDefSegFee;
    gdsl_list_t plTemplSeg;

    pEVSEGetCfg_ft GetEVSECfg;
    ErrorCode_t(*SetEVSECfg)(void *pvEVSE, uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
//    pEVSESetCfg_ft SetSN;
//    pEVSESetCfg_ft SetID;
//    pEVSESetCfg_ft SetType;
//    pEVSESetCfg_ft SetTotalCON;
//    pEVSESetCfg_ft SetLngLat;
//    pEVSESetCfg_ft SetTempl;
}EVSEInfo_t;

typedef struct _EVSEStatus
{
    uint32_t ulScramState;
    uint32_t ulKnockState;
    uint32_t ulPEState;
    uint32_t ulPowerOffState;
    uint32_t ulArresterState;
	
    double dKnockAngle;
    double dAC_A_Temp_IN;
    double dAC_B_Temp_IN;
    double dAC_C_Temp_IN;
    double dAC_N_Temp_IN;
	
    uint32_t ulSignalState;
    uint32_t ulSignalAlarm;
    uint32_t ulSignalFault;
    uint32_t ulSignalPool[EVSE_MAX_SIGNAL_BLOCK];

    pEVSE_ft GetScramState;
    pEVSE_ft GetKnockState;
    pEVSE_ft GetPEState;
    pEVSE_ft GetPowerOffState;
    pEVSE_ft GetArresterState;
    pEVSE_ft GetAC_A_Temp_in;
    pEVSE_ft GetAC_B_Temp_in;
    pEVSE_ft GetAC_C_Temp_in;
    pEVSE_ft GetAC_N_Temp_in;
}EVSEStatus_t;

#endif
