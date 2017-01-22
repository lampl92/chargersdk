/**
* @file taskdata.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#ifndef  __TASKDATA_H
#define  __TASKDATA_H

typedef struct _ChargeSegment
{
    uint8_t ucSegElectricity;
    uint8_t ucFee;
}ChargeSegment_t;

typedef struct _Order
{
    uint8_t ucEquipmentID[8];//设备ID
    uint8_t ucConnectorID;//接口ID
    uint8_t ucElectricityTotal[4];//总电量
    uint8_t ucPayment;  //充电类型
    uint8_t ucStopType; //停止类型
    uint8_t ucReserved; //保留
    uint8_t ucCardID[8];//卡号
    uint8_t ucFeeTotal;//总费用
    uint8_t ucOrderSN[7];//订单流水号
    uint8_t ucServiceFeeType;//服务费类型
    uint8_t ucServiceFee[2];//服务费
    uint8_t ucSegmentTotal;//充电明细段数
    uint8_t ucDefaultSegElectricity;//默认段电量
    uint8_t ucDefaultSegFee;//默认段电费
}Order_t
#endif
