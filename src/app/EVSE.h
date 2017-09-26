/**
* @file EVSE.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#ifndef  __EVSE_H
#define  __EVSE_H

#include "EVSE_privType.h"

typedef struct _EVSE
{
    EVSEInfo_t info;
    EVSEStatus_t status;
}EVSE_t;

EVSE_t *EVSECreate(void);
void EVSEinit(void);

#endif
