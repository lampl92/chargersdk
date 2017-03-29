/**
* @file order.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#include "includes.h"
#include "interface.h"
#include <time.h>

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
    pOrder->state = STATE_ORDER_IDLE;
    pOrder->ucAccountStatus = 0;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    pOrder->dBalance = 0;           //���

    pOrder->dTotalPower = 0;                 //�ܵ���
    pOrder->ucPayType = 0;               //֧����ʽ 0.��ƽ̨֧�� 1.Ǯ����֧��
    pOrder->ucStopType = 0;                  //ֹͣ����
    memset(pOrder->ucCardID, 0, defCardIDLength);//����//��taskrfid�и�ֵ
    pOrder->dTotalFee = 0;                //�ܷ���
    pOrder->tStartTime = 0;
    pOrder->ucServiceFeeType = 0;         //���������
    pOrder->dServiceFee = 0;               //�����
    pOrder->ucTotalSegment = 0;            //�����ϸ����
    pOrder->dDefSegPower = 0;              //Ĭ�϶ε���
    pOrder->dDefSegFee = 0;               //Ĭ�϶ε��
    if(pOrder->pChargeSegment != NULL)
    {
        pOrder->pChargeSegment->Delete(pOrder->pChargeSegment);
        pOrder->pChargeSegment = NULL;
    }
    pOrder->pChargeSegment = UserListCreate();
}

void JudgeSegment(time_t now, CON_t *pCON)
{
    uint32_t ulTotalSegs;
    TemplSeg_t *pTemplSeg;
    struct tm *ts;
    int i;

    ts = localtime(&now);
    ulTotalSegs = pEVSE->info.pTemplSeg->Total;
    for(i = 0; i < ulTotalSegs; i++ )
    {
        pTemplSeg = (TemplSeg_t *)(pEVSE->info.pTemplSeg->pListPointArray[i]);
        //pTemplSeg->tStartTime
    }



}

ErrorCode_t makeOrder(CON_t *pCON)
{
    OrderState_t stat;
    ErrorCode_t errcode;

    stat = pCON->order.state;
    errcode = ERR_NO;
    switch(stat)
    {
    case STATE_ORDER_TMP:
        memmove(pCON->order.ucCardID, pRFIDDev->order.ucCardID, defCardIDLength);
        pCON->order.ucAccountStatus = pRFIDDev->order.ucAccountStatus;
        pCON->order.dBalance = pRFIDDev->order.dBalance;
        pCON->order.ucCONID = pCON->info.ucCONID;
        break;
    case STATE_ORDER_MAKE:
        pCON->order.tStartTime = time(NULL);
        pCON->order.dStartPower = pCON->status.dChargingPower;
        pCON->order.ucServiceFeeType = pEVSE->info.ucServiceFeeType;
        JudgeSegment(pCON->order.tStartTime, pCON);
        break;
    case STATE_ORDER_UPDATE:
        JudgeSegment(time(NULL), pCON);
    case STATE_ORDER_FINISH:
        break;
    }
    return errcode;
}


