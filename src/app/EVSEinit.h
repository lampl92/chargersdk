/**
* @file EVSEinit.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#ifndef  __EVSEINIT_H
#define  __EVSEINIT_H

#include "interface.h"


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

void EVSEinit(void);
#endif
