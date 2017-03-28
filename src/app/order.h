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
    STATE_ORDER_IDLE,
    STATE_ORDER_TMP,
    STATE_ORDER_MAKE,
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

    uint8_t ucCardID[defCardIDLength];    //卡号//在taskrfid中赋值
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 2:欠费 0：未注册卡
    double  dBalance;           //余额
    uint8_t ucCONID;
    //创建时
    time_t tStartTime;                    //启动充电时间
    double  dStartPower;
    uint8_t ucServiceFeeType;           //服务费类型
    //充电过程
    double  dTotalPower;                  //总电量
    double  dTotalPowerFee;             //总电费
    double  dServiceFee;                //总服务费
    double  dTotalFee;                      //总费用
    uint8_t ucTotalSegment;             //充电明细段数
    double  dDefSegPower;               //默认段电量
    double  dDefSegFee;                //默认段电费
    UserList_t *pChargeSegment;
    //停止时
    uint8_t ucPayType;                    //支付方式
    uint8_t ucStopType;                   //停止类型

    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;

void OrderInit(OrderData_t *pOrder);
#endif
