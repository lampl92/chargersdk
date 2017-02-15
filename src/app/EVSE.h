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
