/**
* @file order.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#ifndef  __ORDER_H
#define  __ORDER_H

#include "gdsl_list.h"
#include "evse_config.h"
#include "taskremote.h"
#include "FreeRTOS.h"
#include "timers.h"
#include <time.h>

/*启动类型 StopType*/
#define defOrderStartType_Card          4
#define defOrderStartType_Remote        5

/*停止类型 StopType*/
#define defOrderStopType_Unknown        0
#define defOrderStopType_RFID           1
#define defOrderStopType_Full           2
#define defOrderStopType_Energy          3
#define defOrderStopType_Time           4
#define defOrderStopType_Fee            5
#define defOrderStopType_NeedFee        6
#define defOrderStopType_Remote         7
#define defOrderStopType_Offline        8
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
#define defOrderSerType_Energy          1


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
    STATE_ORDER_WAITSTOP,
    STATE_ORDER_FINISH,
    STATE_ORDER_HOLD,
    STATE_ORDER_RETURN
}OrderState_t;

/*每个时间段需要记录的信息*/
typedef struct _ChargePeriodStatus
{
    time_t tStartTime;
    time_t tEndTime;
    double dStartEnergy;
    double dEnergy;
}ChargePeriodStatus_t;

/** @brief  ucCardID 、ucAccountStatus、 dBalance、 ucCONID、 strOrderSN 是刷卡板要获取的数据, 在order建立时应拷贝到CON的order中
 */
typedef struct _OrderData
{
    OrderState_t    statOrder;  //记录订单状态
    OrderSegState_e statOrderSeg; //记录订单时间段状态
    uint8_t pos;//在时段中的位置，用于与now获得的pos进行对比

    char strCardID[defCardIDLength + 1];//卡号//在taskrfid中赋值
    uint8_t ucAccountStatus;    //帐户状态 1：注册卡 2:欠费 0：未注册卡
    uint8_t ucCardStatus;      //0 普通用户, 1 白名单用户, 2 黑名单用户
    double  dBalance;           //余额 
    uint8_t ucCONID;            //
    //创建时
    char strOrderSN[defOrderSNLength + 1]; //交易流水号

    time_t  tStartTime;         //启动充电时间 
    uint8_t ucStartType;        //4 有卡 5 无卡 
    double  dLimitFee;          //充电截至金额         在远程启动和界面启动时赋值
    uint32_t ulLimitTime;       //充电最大时间         
    double dLimitEnergy;         //充电最大电量
    double  dStartEnergy;        //
    //充电过程
    double  dTotalEnergy;        //总电量
    double  dTotalEnergyFee;     //总电费
    double  dTotalServFee;   //总服务费
    double  dTotalFee;          //总费用
    ChargePeriodStatus_t chargeSegStatus[defOrderSegMax][defOrderPeriodMax];     //[0][0]尖第一时段 [1][0]峰第一时段 [2][1]平第二时段  过程信息
    double dSegTotalEnergy[defOrderSegMax];   //分段总电量
    double dSegTotalEnergyFee[defOrderSegMax];//分段总电费
    double dSegTotalServFee[defOrderSegMax]; //分段总服务费
    uint32_t ulSegTotalTime[defOrderSegMax];    //分段总充电时间

    //停止时
    uint8_t         ucPayType;  //支付方式
    uint8_t         ucPayStatus;//结算状态 0:未结算  1:已结算
    uint8_t         ucStopType; //停止类型
    time_t          tStopTime;  //停止时间 

	statRemote_t statRemoteProc;
}OrderData_t;

typedef struct _OrderTmpData
{
    OrderData_t order;
    uint8_t ucCheckOrderTmp;
    char strOrderTmpPath[32];
    TimerHandle_t xHandleTimerOrderTmp;
}OrderTmpData_t;

typedef struct _UserChargeCondition
{
    uint8_t ucCONID;            //
    double  dLimitFee;          //充电截至金额
    uint32_t ulLimitTime;       //充电最大时间       
    double dLimitEnergy;         //充电最大电量
    int HMItimeout;       //HMI超时
    char strPwd[7];
}UserChargeCondition_t;
void OrderCreate(OrderData_t *pOrder);
void OrderInit(OrderData_t *pOrder);
double get_current_totalfee(time_t now);

#endif
