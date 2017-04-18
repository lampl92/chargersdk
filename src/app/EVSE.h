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
#include "gdsl_list.h"

typedef ErrorCode_t (*pEVSEGetCfg_ft)(void *pvEVSE, void *pvCfgObj);
typedef ErrorCode_t (*pEVSESetCfg_ft)(void *pvEVSE, void *pvCfgParam);
typedef ErrorCode_t (*pEVSE_ft)(void *pvEVSE);
typedef struct _TemplSeg
{
    time_t tStartTime;//��time_t��ʾʱ�����������գ���ʱ����ֻ��עʱ����
    time_t tEndTime;
    double dSegFee;
}TemplSeg_t;
typedef struct _EVSEInfo
{
    uint8_t ucSN[defEVSESNLength+1]; //�豸Ψһ���к�
    uint8_t ucSNLength;
    uint8_t ucID[defEVSEIDLength+1]; //ƽ̨����ID
    uint8_t ucIDLenght;
    uint8_t ucType;   //1��ֱ���豸 2�������豸 3����ֱ��һ���豸 4�������豸 5������
    uint8_t ucTotalCON; //һ���ж���ǹ
    double dLng;      //���ȣ���������λ
    double dLat;      //γ�ȣ���������λ
    uint8_t ucServiceFeeType;//0:������ 1������
    double  dServiceFee;                //�����
    double dDefSegFee;
    gdsl_list_t plTemplSeg;

    pEVSEGetCfg_ft GetEVSECfg;

    pEVSESetCfg_ft SetSN;
    pEVSESetCfg_ft SetID;
    pEVSESetCfg_ft SetType;
    pEVSESetCfg_ft SetTotalCON;
    pEVSESetCfg_ft SetLngLat;
    pEVSESetCfg_ft SetTempl;
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
