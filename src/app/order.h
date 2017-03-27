/**
* @file order.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#ifndef  __ORDER_H
#define  __ORDER_H

#include <time.h>
#include "userlib_list.h"

typedef enum _OrderState
{
    STATE_ORDER_TEMP,
    STATE_ORDER_MAKE,
    STATE_ORDER_START,
    STATE_ORDER_UPDATE,
    STATE_ORDER_FINISH
}OrderState_t;


typedef struct _ChargeSegment
{
    time_t tStartTime;
    time_t tEndTime;
    double dSegStartPower;
    double dSegPower;
    double dSegFee;
}ChargeSegment_t;

typedef struct _OrderData
{
    OrderState_t state;
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 2:欠费 0：未注册卡
    double  dBalance;           //余额

    uint8_t ucCONID;
    double  dTotalPower;                  //总电量
    uint8_t ucPayType;                    //支付方式
    uint8_t ucStopType;                   //停止类型
    uint8_t ucReserved;                   //保留
    uint8_t ucCardID[defCardIDLength];    //卡号//在taskrfid中赋值
    double  dTotalFee;                      //总费用
    uint8_t strStartTime[7];             //订单流水号
    uint8_t ucServiceFeeType;           //服务费类型
    double  dServiceFee;                //服务费
    uint8_t ucTotalSegment;             //充电明细段数
    double  dDefSegPower;               //默认段电量
    double  dDefSegFee;                //默认段电费
    UserList_t *pChargeSegment;
    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;

void OrderInit(OrderData_t *pOrder);
#endif
