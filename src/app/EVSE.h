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
#include "errorcode.h"

typedef ErrorCode_t (*pEVSE_ft)(void *pvEVSE);

typedef struct _EVSEInfo
{
    uint8_t ucSN[24]; //�豸Ψһ���к�
    uint8_t ucSNLength;
    uint8_t ucID[24]; //ƽ̨����ID
    uint8_t ucIDLenght;
    uint8_t ucType;   //1��ֱ���豸 2�������豸 3����ֱ��һ���豸 4�������豸 5������
    uint8_t ucTotalPoint; //һ���ж���ǹ
    double dLng;      //���ȣ���������λ
    double dLat;      //γ�ȣ���������λ

    pEVSE_ft GetSN;
    pEVSE_ft GetID;
    pEVSE_ft GetType;
    pEVSE_ft GetTotalPoint;
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
