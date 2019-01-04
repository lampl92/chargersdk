/**
* @file connector.h
* @brief 定义充电接口需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __CONNECTOR_H
#define  __CONNECTOR_H

#include "order.h"
#include "connector_privType.h"
#include "line.h"

typedef struct _CON
{
    CONInfo_t info;
    CONStatus_t status;
    CONState_t state;
    CONAppoint_t appoint;
    Line_t line[4];
    OrderData_t order;
    OrderTmpData_t OrderTmp;
    CONTmp_t tmp;
} CON_t;

CON_t *CONCreate(uint8_t ucCONID);
CON_t *CONGetHandle(uint8_t ucCONID);
CONState_t CONGetState(uint8_t ucCONID);
#endif
