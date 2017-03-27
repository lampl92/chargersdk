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
    pOrder->ucAccountStatus = 0;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    pOrder->dBalance = 0;           //���

    pOrder->dTotalPower = 0;                 //�ܵ���
    pOrder->ucPayType = 0;               //֧����ʽ 0.��ƽ̨֧�� 1.Ǯ����֧��
    pOrder->ucStopType = 0;                  //ֹͣ����
    pOrder->ucReserved = 0;                  //����
    memset(pOrder->ucCardID, 0, defCardIDLength);//����//��taskrfid�и�ֵ
    pOrder->dTotalFee = 0;                //�ܷ���
    memset(pOrder->strStartTime, 0, 7);       //������ˮ��
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



ErrorCode_t makeOrder()
{

}
