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

//static void OrderDelete(OrderData_t *pOrder)
//{
//    gdsl_list_free(pOrder->plChargeSegment);
//    free(pOrder);
//}
//
//OrderData_t *OrderCreate(void)
//{
//    OrderData_t *pOrder = NULL;
//    pOrder = (OrderData_t *)malloc(sizeof(OrderData_t));
//    pOrder->Delete = OrderDelete;
//    pOrder->plChargeSegment = gdsl_list_alloc("SegList",ChargeSegAlloc,ChargeSegFree);
//    return pOrder;
//}


/** @brief �ж�ʱ��߽磬����little��������big
 *
 * @param src time_t
 * @param little time_t
 * @param big time_t
 * @return uint8_t 1:�������У�0������������
 *
 */
static gdsl_element_t ChargeSegAlloc(void *pChargeSeg)
{
    gdsl_element_t copyChSeg;
    copyChSeg = (gdsl_element_t)malloc(sizeof(ChargeSegment_t));
    if(copyChSeg != NULL)
    {
        memcpy(copyChSeg, pChargeSeg, sizeof(ChargeSegment_t));
    }
    return copyChSeg;
}
static void ChargeSegFree (gdsl_element_t e)
{
    free (e);
}
static uint8_t JudgeTimeInclude(time_t tSrc, time_t tStart, time_t tEnd)
{
    if(tSrc >= tStart && tSrc < tEnd)
    {
        return 1;
    }
    else if(tSrc == tEnd)
    {
        return 2;//ʱ�α߽硣��ʱ�жϰ취������ʱһ��Ҫ��ֲ���ϵͳ�Ƿ���Լ�⵽ʱ�α߽硣
    }
    else
    {
        return 0;
    }
}
static void SegmentProc(time_t now, CON_t *pCON)
{
    uint32_t ulTotalTemplSegs;
    TemplSeg_t *pTemplSeg;
    ChargeSegment_t ChSeg;
    ChargeSegment_t *pChSeg;
    uint8_t ucResJudge;
    struct tm *ts;
    uint8_t ucBoundaryPos;
    int i;

    gdsl_list_t plChSeg;
    gdsl_list_t plTemplSeg;

    plChSeg = pCON->order.plChargeSegment;
    plTemplSeg = pEVSE->info.plTemplSeg;

    ucBoundaryPos = 0;
    ucResJudge = 0;
    ts = localtime(&now);
    ulTotalTemplSegs = gdsl_list_get_size(plTemplSeg);
    /*1. ���ж�*/
#ifdef DEBUG_DIAG_DUMMY
    ulTotalTemplSegs = 0;
#endif
    if(ulTotalTemplSegs > 0)
    {
        for(i = 1; i <= ulTotalTemplSegs; i++ )
        {
            pTemplSeg = (TemplSeg_t *)(gdsl_list_search_by_position(plTemplSeg, i));
            ucResJudge = JudgeTimeInclude(now, pTemplSeg->tStartTime, pTemplSeg->tEndTime);
            if(ucResJudge == 1)
            {
                switch(pCON->order.statOrderSeg)
                {
                case STATE_ORDERSEG_DEF:
                    pCON->order.statOrderSeg = STATE_ORDERSEG_START;
                    break;
                case STATE_ORDERSEG_START:
                    ChSeg.state = STATE_SEG_START;
                    ChSeg.ucTemplPos = i;
                    gdsl_list_insert_tail(plChSeg, &ChSeg);
                    pCON->order.statOrderSeg = STATE_ORDERSEG_IN;
                    break;
                case STATE_ORDERSEG_IN:
                    pChSeg = (ChargeSegment_t *)gdsl_list_get_tail(plChSeg);//Ҫ�����Ķα�Ȼ������β
                    if(pChSeg->ucTemplPos != i)//ʱ���Ѿ����ڵ�ǰ����
                    {

//                        pChSeg->state = STATE_SEG_OUT;
                        pCON->order.statOrderSeg = STATE_ORDERSEG_END;
                        break;
                    }
                    switch(pChSeg->state)
                    {
                    case STATE_SEG_START:
                        pChSeg->dSegStartPower = pCON->status.dChargingPower;
                        pChSeg->tStartTime = now;
                        pChSeg->state = STATE_SEG_IN;
                        break;
                    case STATE_SEG_IN:
                        pChSeg->dSegPower = pCON->status.dChargingPower - pChSeg->dSegStartPower;
                        pChSeg->dSegFee = pChSeg->dSegPower * pTemplSeg->dSegFee;
                        break;
                    case STATE_SEG_END:
                        //��������������
                        break;
                    default:
                        break;
                    }
                    break;
//                case STATE_ORDERSEG_END:
//                    break;
                default:
                    break;
                }
//
//                switch()
//                    ChSeg.state
//                    ChSeg->dSegStartPower =
//                        pCON->order.dTotalPower = pCON->status.GetChargingPower - pCON->order.dStartPower;
//                pCON->order.dTotalPowerFee = pCON->order.dTotalPower * pTemplSeg->dSegFee;
                break;//�˳�ѭ��
            }
            else if(ucResJudge == 2)
            {
                ucBoundaryPos = i;
                pCON->order.statOrderSeg = STATE_ORDERSEG_END;
                break;//�˳�ѭ��
            }
            else //��ǰʱ��������seg��û���ҵ�ʱ��Σ�ִ��Ĭ�϶�
            {
                pCON->order.statOrderSeg = STATE_ORDERSEG_DEF;
            }
        }
    }
    else//����û��seg����Ҫִ��Ĭ�϶�
    {
        pCON->order.statOrderSeg = STATE_ORDERSEG_DEF;
    }
    /*2. �δ���*/
    switch(pCON->order.statOrderSeg)
    {
    case STATE_ORDERSEG_DEF:
        pCON->order.dTotalPower = pCON->status.dChargingPower - pCON->order.dStartPower;
        if(ulTotalTemplSegs == 0)
        {
            pCON->order.dDefSegPower = pCON->order.dTotalPower;
            pCON->order.dDefSegFee = pCON->order.dDefSegPower * pEVSE->info.dDefSegFee;
            pCON->order.dTotalPowerFee = pCON->order.dDefSegFee;
        }
        else
        {
//            pCON->order.dDefSegPower = pCON->order.dTotalPower - ʱ���ܵ���;
//            pCON->order.dDefSegFee = pCON->order.dDefSegPower * pEVSE->info.dDefSegFee;
//            pCON->order.dTotalPowerFee = pCON->order.dDefSegFee + ʱ���ܵ��;
        }
        break;
    case STATE_ORDERSEG_END:
        pTemplSeg = (TemplSeg_t *)(gdsl_list_search_by_position(plTemplSeg, ucBoundaryPos));
        pChSeg = (ChargeSegment_t *)gdsl_list_get_tail(plChSeg);//Ҫ�����Ķα�Ȼ������β
        pChSeg->tEndTime = now;
        break;
    default:
        break;
    }
    /*3. ���������*/
    if(pCON->order.ucServiceFeeType == 0)//����
    {
        pCON->order.dServiceFee = pEVSE->info.dServiceFee;
    }
    else if(pCON->order.ucServiceFeeType == 1)//����
    {
        pCON->order.dServiceFee = pCON->order.dTotalPower * pEVSE->info.dServiceFee;
    }
}

ErrorCode_t makeOrder(CON_t *pCON)
{
    OrderState_t statOrder;
//    OrderSegState_t statOrderSeg;
    ErrorCode_t errcode;

    statOrder = pCON->order.statOrder;
    errcode = ERR_NO;
    switch(statOrder)
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
        SegmentProc(pCON->order.tStartTime, pCON);
        break;
    case STATE_ORDER_UPDATE:
        SegmentProc(time(NULL), pCON);
        break;
    case STATE_ORDER_FINISH:
        /** @todo (rgw#1#): ��Ӷ�������ʱ���� */
        break;
    }
    return errcode;
}

void OrderCreate(OrderData_t *pOrder)
{
    pOrder->plChargeSegment = gdsl_list_alloc("SegList", ChargeSegAlloc, ChargeSegFree);
}

void OrderInit(OrderData_t *pOrder)
{
    pOrder->statOrder = STATE_ORDER_IDLE;
    pOrder->statOrderSeg = STATE_ORDERSEG_DEF;
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
    if(pOrder->plChargeSegment != NULL)
    {
        gdsl_list_flush(pOrder->plChargeSegment);
    }
}
