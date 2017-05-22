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
    uint16_t cmd_id;
    uint32_t len;
    uint8_t *pbuff;
    uint8_t status; //Send��1 �ѷ��ͣ�0 δ����   Recv��1 �Ѵ��� 0 δ����
    uint8_t trycount;
    uint8_t trycountmax;
}echCmdElem_t;


#endif
