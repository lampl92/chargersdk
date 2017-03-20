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


typedef struct _ChargeSegment
{
    uint8_t strStartTime[7];
    uint8_t strStopTime[7];
    double dSegPower;
    double dSegFee;
}ChargeSegment_t;

typedef struct _OrderData
{
    uint8_t OrderState;         //1:启动中，2：充电中，3：停止中，4：已结束，5：未知
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 0：未注册卡
    double  dBalance;           //余额

    double  dTotalPower;                  //总电量
    uint8_t ucPaymentType;                //支付方式
    uint8_t ucStopType;                   //停止类型
    uint8_t ucReserved;                   //保留
    uint8_t ucCardID[defCardIDLength];    //卡号//在taskrfid中赋值
    double  dTotalFee;                 //总费用
    uint8_t strOrderSN[7];             //订单流水号
    uint8_t ucServiceFeeType;          //服务费类型
    double  dServiceFee;                //服务费
    uint8_t ucTotalSegment;             //充电明细段数
    double  dDefSegPower;               //默认段电量
    double  dDefSegFee;                //默认段电费
    UserList_t *pChargeSegment;
}OrderData_t;

typedef struct _RfidOrderData
{
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 0：未注册卡
    double  dBalance;           //余额
    uint8_t ucCONID;          //接口ID 0~255
    uint8_t ucCardID[defCardIDLength];    //卡号//在taskrfid中赋值
}RfidOrderData_t;


ErrorCode_t CreateOrderFile(void);

#endif
