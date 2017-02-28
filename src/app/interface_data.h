/**
* @file interface_data.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __INTERFACE_DATA_H
#define  __INTERFACE_DATA_H

#include "includes.h"

typedef struct _OrderData
{

    uint8_t ucUID[defUIDLength];        //在taskrfid中赋值
    uint8_t ucAccountStatus;    //帐户状态
    double  dBalance;           //余额
    uint8_t ucPointID;
    uint8_t ucEVSESN[defEVSEIDLength];

}OrderData_t;

typedef struct _FileDataType
{

}FileDataType_t;

ErrorCode_t CreateOrderXML(void);

#endif
