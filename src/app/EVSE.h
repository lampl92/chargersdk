/**
* @file EVSE.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#ifndef  __EVSE_H
#define  __EVSE_H

#include "stm32f4xx.h"
#include "evse_config.h"
#include "errorcode.h"
#include "userlib_list.h"

typedef ErrorCode_t (*pEVSE_ft)(void *pvEVSE);
typedef struct _TemplSeg
{
    time_t tStartTime;//用time_t表示时，忽略年月日，在时段中只关注时分秒
    time_t tEndTime;
    double dSegFee;
}TemplSeg_t;
typedef struct _EVSEInfo
{
    uint8_t ucSN[defEVSESNLength+1]; //设备唯一序列号
    uint8_t ucSNLength;
    uint8_t ucID[defEVSEIDLength+1]; //平台分配ID
    uint8_t ucIDLenght;
    uint8_t ucType;   //1：直流设备 2：交流设备 3：交直流一体设备 4：无线设备 5：其他
    uint8_t ucTotalCON; //一共有多少枪
    double dLng;      //经度，保留后六位
    double dLat;      //纬度，保留后六位
    uint8_t ucServiceFeeType;
    double  dServiceFee;                //服务费
    double dDefSegFee;
    UserList_t *pTemplSeg;

    pEVSE_ft GetEVSECfg;
    pEVSE_ft GetSN;
    pEVSE_ft GetID;
    pEVSE_ft GetType;
    pEVSE_ft GetTotalCON;
    pEVSE_ft GetLngLat;
}EVSEInfo_t;

typedef struct _EVSEStatus
{
    uint32_t ulScramState;
    uint32_t ulKnockState;
    uint32_t ulPEState;
    uint32_t ulPowerOffState;
    uint32_t ulArresterState;

    pEVSE_ft GetScramState;
    pEVSE_ft GetKnockState;
    pEVSE_ft GetPEState;
    pEVSE_ft GetPowerOffState;
    pEVSE_ft GetArresterState;
}EVSEStatus_t;

typedef struct _EVSE
{
    EVSEInfo_t info;
    EVSEStatus_t status;
}EVSE_t;

EVSE_t *EVSECreate(void);
void EVSEinit(void);

#endif
