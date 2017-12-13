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
#include "evse_config.h"
#include "taskremote.h"


/*停止类型 StopType*/
#define defOrderStopType_Unknown        0
#define defOrderStopType_RFID           1
#define defOrderStopType_Full           2
#define defOrderStopType_Power          3
#define defOrderStopType_Time           4
#define defOrderStopType_Fee            5
#define defOrderStopType_NeedFee        6
#define defOrderStopType_Remote         7
#define defOrderStopType_NetLost        8
#define defOrderStopType_Poweroff       9
#define defOrderStopType_Scram          10
#define defOrderStopType_SocketError    11
#define defOrderStopType_UnPlug     12
#define defOrderStopType_OverVolt       13
#define defOrderStopType_UnderVolt      14
#define defOrderStopType_OverCurr       15
#define defOrderStopType_UnderCurr      16
#define defOrderStopType_Knock          17

/*支付方式 PayType*/
#define defOrderPayType_Online          0
#define defOrderPayType_Offline         1

/*服务费类型*/
#define defOrderSerType_Order           0
#define defOrderSerType_Power           1


/*
          +------+------+------+------+------+
          |      |      |      |      |      |
Segment0  |Period|      |      |      |      |
          |      |      |      |      |      |
          +------+------+------+------+------+

          +------+------+------+------+------+
          |      |      |      |      |      |
Segment1  |Period|      |      |      |      |
          |      |      |      |      |      |
          +------+------+------+------+------+
*/

#define defOrderSegMax                  5
#define defOrderPeriodMax               5

/*当前时间所在时间段类型*/
typedef enum
{
    STATE_SEG_SHARP = 0, //枚举位置用作数组标号, 因此不要修改尖峰平谷的位置
    STATE_SEG_PEAK,
    STATE_SEG_SHOULDER,
    STATE_SEG_OFF_PEAK,
    STATE_SEG_DEFAULT,
    STATE_SEG_IDLE
}OrderSegState_e;

/*订单所在状态*/
typedef enum _OrderState
{
    STATE_ORDER_IDLE,
    STATE_ORDER_TMP,
    STATE_ORDER_WAITSTART,
    STATE_ORDER_MAKE,
    STATE_ORDER_UPDATE,
    STATE_ORDER_FINISH
}OrderState_t;

/*每个时间段需要记录的信息*/
typedef struct _ChargePeriodStatus
{
    time_t tStartTime;
    time_t tEndTime;
    double dStartPower;
    double dPower;
}ChargePeriodStatus_t;

typedef struct _statRemote
{
	RemoteCardStatus_t card;
	RemoteOrderStatus_t order;
}statRemote_t;

/** @brief  ucCardID 、ucAccountStatus、 dBalance、 ucCONID、 strOrderSN 是刷卡板要获取的数据, 在order建立时应拷贝到CON的order中
 */
typedef struct _OrderData
{
    OrderState_t    statOrder;  //记录订单状态
    OrderSegState_e statOrderSeg; //记录订单时间段状态
    uint8_t pos;//在时段中的位置，用于与now获得的pos进行对比

    uint8_t ucCardID[defCardIDLength]; //卡号//在taskrfid中赋值            2
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 2:欠费 0：未注册卡
    uint8_t ucCardStatus;      //0 普通用户, 1 白名单用户, 2 黑名单用户
    double  dBalance;           //余额                                        3
    uint8_t ucCONID;            //4
    //创建时
    uint8_t strOrderSN[defOrderSNLength]; //交易流水号       DBIdx 1

    time_t  tStartTime;         //启动充电时间           5
    uint8_t ucStartType;        //4 有卡 5 无卡         6
    double  dLimitFee;          //充电截至金额         在远程启动和界面启动时赋值
    uint32_t ulLimitTime;       //充电最大时间         
    double  dStartPower;        //8
    //充电过程
    double  dTotalPower;        //总电量
    double  dTotalPowerFee;     //总电费
    double  dTotalServFee;   //总服务费
    double  dTotalFee;          //总费用
    ChargePeriodStatus_t chargeSegStatus[defOrderSegMax][defOrderPeriodMax];     //[0][0]尖第一时段 [1][0]峰第一时段 [2][1]平第二时段  过程信息
    double dSegTotalPower[defOrderSegMax];   //分段总电量
    double dSegTotalPowerFee[defOrderSegMax];//分段总电费
    double dSegTotalServFee[defOrderSegMax]; //分段总服务费
    uint32_t ulSegTotalTime[defOrderSegMax];    //分段总充电时间

    //停止时
    uint8_t         ucPayType;  //支付方式
    uint8_t         ucPayStatus;//结算状态 0:未结算  1:已结算
    uint8_t         ucStopType; //停止类型
    time_t          tStopTime;  //停止时间          6

	statRemote_t statRemoteProc;
    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;


void OrderCreate(OrderData_t *pOrder);
void OrderInit(OrderData_t *pOrder);

#endif
