/**
* @file D:\Documents\Projects\chargersdk\src\app\libEcharge\ech_protocol_proc.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-18
*/
#ifndef  __ECH_PROTOCOL_PROC_H
#define  __ECH_PROTOCOL_PROC_H

#include "ech_protocol.h"

typedef struct
{
    time_t timestamp;
    uint32_t timeout;
    echCMDType_t cmd;
    uint32_t len;
    uint32_t *pbuff;
    uint8_t status;
    uint8_t trycount;
}echCmdElem_t;


#endif
