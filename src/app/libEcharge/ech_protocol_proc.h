/**
* @file D:\Documents\Projects\chargersdk\src\app\libEcharge\ech_protocol_proc.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-18
*/
#ifndef  __ECH_PROTOCOL_PROC_H
#define  __ECH_PROTOCOL_PROC_H

#include "libEcharge/ech_protocol.h"

typedef struct
{
    time_t timestamp;
    uint32_t timeout_s;
    echCMDType_t cmd;
    uint16_t cmd_id;
    uint32_t len;
    uint8_t *pbuff;
    uint8_t status; //Send：1 已发送，0 未发送   Recv：1 已处理 0 未处理
    uint8_t trycount;
    uint8_t trycountmax;
}echProtoElem_t;


#endif
