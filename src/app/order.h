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
#include "gdsl_list.h"

typedef enum _OrderState
{
    STATE_ORDER_IDLE,
    STATE_ORDER_TMP,
    STATE_ORDER_WAITSTART,
    STATE_ORDER_MAKE,
    STATE_ORDER_UPDATE,
    STATE_ORDER_FINISH
}OrderState_t;
typedef enum _OrderSegState
{
    STATE_ORDERSEG_DEF,          //订单没有进入SEG,在默认段中
    STATE_ORDERSEG_START,//订单第一次进入SEG
    STATE_ORDERSEG_IN,

    STATE_ORDERSEG_END
}OrderSegState_t;
typedef enum _ChargeSegState
{
    STATE_SEG_START,//当前SEG第一次进入
    STATE_SEG_IN,
    STATE_SEG_END
}ChargeSegState_t;

typedef struct _ChargeSegment
{
    time_t tStartTime;
    time_t tEndTime;
    double dSegStartPower;
    double dSegPower;
    double dSegFee;
    ChargeSegState_t state;
    uint8_t ucTemplPos;//记录当前段在Templ中的位置，用于判断当前时间是否在当前段中
}ChargeSegment_t;

typedef struct _OrderData
{
    OrderState_t statOrder;//记录订单状态
    OrderSegState_t statOrderSeg;//记录订单时间段状态

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
    double  dTotalServiceFee;                //总服务费
    double  dTotalFee;                      //总费用
    uint8_t ucTotalSegment;             //充电明细段数
    double  dDefSegStartPower;          //默认段起始变量。此变量为中间变量，功能为记录每一次进入默认段时的起始电量
    double  dDefSegPower;               //默认段电量
    double  dDefSegFee;                //默认段电费
    gdsl_list_t plChargeSegment;
    //停止时
    uint8_t ucPayType;                    //支付方式
    uint8_t ucStopType;                   //停止类型

    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;

void OrderCreate(OrderData_t *pOrder);
void OrderInit(OrderData_t *pOrder);

#endif
