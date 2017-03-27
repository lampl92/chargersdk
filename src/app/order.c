/**
* @file order.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#include "includes.h"
#include "interface.h"

static void OrderDelete(OrderData_t *pOrder)
{
    pOrder->pChargeSegment->Delete(pOrder);
    free(pOrder);
}

OrderData_t *OrderCreate(void)
{
    OrderData_t *pOrder = NULL;
    pOrder = (OrderData_t *)malloc(sizeof(OrderData_t));
    pOrder->Delete = OrderDelete;
    pOrder->pChargeSegment = UserListCreate();
    return pOrder;
}

void OrderInit(OrderData_t *pOrder)
{
    pOrder->state = STATE_ORDER_FINISH;
    pOrder->ucAccountStatus = 0;    //帐户状态 1：注册卡 0：未注册卡
    pOrder->dBalance = 0;           //余额

    pOrder->dTotalPower = 0;                 //总电量
    pOrder->ucPayType = 0;               //支付方式 0.云平台支付 1.钱包卡支付
    pOrder->ucStopType = 0;                  //停止类型
    pOrder->ucReserved = 0;                  //保留
    memset(pOrder->ucCardID, 0, defCardIDLength);//卡号//在taskrfid中赋值
    pOrder->dTotalFee = 0;                //总费用
    memset(pOrder->strStartTime, 0, 7);       //订单流水号
    pOrder->ucServiceFeeType = 0;         //服务费类型
    pOrder->dServiceFee = 0;               //服务费
    pOrder->ucTotalSegment = 0;            //充电明细段数
    pOrder->dDefSegPower = 0;              //默认段电量
    pOrder->dDefSegFee = 0;               //默认段电费
    if(pOrder->pChargeSegment != NULL)
    {
        pOrder->pChargeSegment->Delete(pOrder->pChargeSegment);
        pOrder->pChargeSegment = NULL;
    }
    pOrder->pChargeSegment = UserListCreate();
}



ErrorCode_t makeOrder()
{

}
