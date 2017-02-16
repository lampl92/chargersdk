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
typedef struct _EVSEInfo
{
    uint8_t ucSN[24]; //�豸Ψһ���к�
    uint8_t ucID[24]; //ƽ̨����ID
    uint8_t ucType;   //1��ֱ���豸 2�������豸 3����ֱ��һ���豸 4�������豸 5������
    double dLng;      //���ȣ���������λ
    double dLat;      //γ�ȣ���������λ
}EVSEInfo_t;
typedef struct _EVSEState
{
    uint32_t ulScramState;
    uint32_t ulKnockState;
    uint32_t ulPEState;
    uint32_t ulPowerOffState;
    uint32_t ulArresterState;
}EVSEState_t;

typedef struct _EVSE
{
    EVSEInfo_t info;
    EVSEState_t state;
}EVSE_t;

ErrorCode_t GetScramState(EVSE_t *pEVSE);
ErrorCode_t GetKnockState(EVSE_t *pEVSE);
ErrorCode_t GetPEState(EVSE_t *pEVSE);
ErrorCode_t GetPowerOffState(EVSE_t *pEVSE);
ErrorCode_t GetArresterState(EVSE_t *pEVSE);
EVSE_t *EVSECreate(void);
void EVSEinit(void);

#endif
