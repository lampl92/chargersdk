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
#include "cJSON.h"
#include "cfg_parse.h"

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
/** @brief �жϵ�ǰʱ���Ƿ���ʱ�����
 *
 * @param now time_t
 * @param ucStart uint8_t
 * @param ucEnd uint8_t
 * @return uint8_t 1����ʱ����ڣ� 0������ʱ�����
 *
 */
static uint8_t JudgeTimeInclude(time_t now, uint8_t ucStart, uint8_t ucEnd)
{
    struct tm *ts;
    ts = localtime(&now);
    if(ts->tm_hour >= ucStart && ts->tm_hour < ucEnd)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/** @brief �жϵ�ǰʱ���Ƿ��ڼ��ƽ��ĳ��ʱ�����
 *
 * @param now time_t
 * @param ucStart uint8_t
 * @param ucEnd uint8_t
 * @return uint8_t  1����ʱ�����  0������ʱ�����
 *
 */
static uint8_t JudgeSegInclude(time_t now, EchSegTime_t SegTime, uint8_t *pos)
{
    int i;
    uint8_t isInclude = 0;

    for(i = 0; i < SegTime.ucSegCont; i++)
    {
        isInclude = JudgeTimeInclude(now, SegTime.ucStart[i], SegTime.ucEnd[i]);
        if(isInclude == 1)
        {
            *pos = (uint8_t)i;
            return 1;
        }
    }
    return 0;
}

/** @brief �жϵ�ǰʱ�����ڵļ��ƽ��״̬
 *
 * @param now time_t
 * @param pProto echProtocol_t*
 * @param uint8_t pos* ��ǰʱ����ʱ�������
 * @return SegTimeState_e
 *
 */     
static SegTimeState_e JudgeSegState(time_t now, echProtocol_t *pProto, uint8_t *pos)
{
    
    if(JudgeSegInclude(now, pProto->info.SegTime_sharp, pos) == 1)
    {
        return STATE_SEG_SHARP;
    }
    if(JudgeSegInclude(now, pProto->info.SegTime_peak, pos) == 1)
    {
        return STATE_SEG_PEAK;
    }
    if(JudgeSegInclude(now, pProto->info.SegTime_shoulder, pos) == 1)
    {
        return STATE_SEG_SHOULDER;
    }
    if(JudgeSegInclude(now, pProto->info.SegTime_off_peak, pos) == 1)
    {
        return STATE_SET_OFF_PEAK;
    }
}

static void SegmentProc(time_t now, CON_t *pCON)
{

    uint8_t ucBoundaryPos;
    int i;
    uint8_t pos;
    SegTimeState_e statSegTime;
    ucBoundaryPos = 0;


    /*1. ���ж�*/
    pCON->order.statOrderSeg = JudgeSegState(now, pechProto, &pos);
    switch(pCON->order.statOrderSeg)
    {
    case STATE_SEG_SHARP:
        
    break;
    case STATE_SEG_PEAK:
    break;
    case STATE_SEG_SHOULDER:
    break;
    case STATE_SET_OFF_PEAK:
    break;
    default:
    break;    
    }
    
    

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
    if(pCON->order.ucServiceFeeType == defOrderSerType_Order)//����
    {
        pCON->order.dTotalServiceFee = pEVSE->info.dServiceFee;
    }
    else if(pCON->order.ucServiceFeeType == defOrderSerType_Power)//����
    {
        pCON->order.dTotalServiceFee = pCON->order.dTotalPower * pEVSE->info.dServiceFee;
    }
    /*4. �ܷ���*/
    pCON->order.dTotalFee = pCON->order.dTotalPowerFee + pCON->order.dTotalServiceFee;
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
        pCON->order.ucPayType = defOrderPayType_Online;
        pCON->order.tStopTime = time(NULL);
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

    memset(pOrder->ucCardID, 0, defCardIDLength);//����//��taskrfid�и�ֵ
    pOrder->ucAccountStatus = 0;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    pOrder->dBalance = 0;           //���

    memset(pOrder->strOrderSN, '\0', defOrderSNLength);
    pOrder->tStartTime = 0;                 //��ʼʱ��
    pOrder->ucStartType = 0;        //4�п� 5�޿�
    pOrder->dLimitFee = 0;                    //���������
    pOrder->dStartPower = 0;                //��ʼ������
    pOrder->ucServiceFeeType = 0;         //���������

    pOrder->dTotalPower = 0;                 //�ܵ���
    pOrder->dTotalPowerFee = 0;             //�ܵ��
    pOrder->dTotalServiceFee = 0;               //�����
    pOrder->dTotalFee = 0;                //�ܷ���
    pOrder->ucTotalSegment = 0;            //�����ϸ����
    pOrder->dDefSegStartPower = 0;          //Ĭ�϶���ʼ������
    pOrder->dDefSegPower = 0;              //Ĭ�϶ε���
    pOrder->dDefSegFee = 0;               //Ĭ�϶ε��

    pOrder->ucPayType = 0;               //֧����ʽ 0.��ƽ̨֧�� 1.Ǯ����֧��
    pOrder->ucStopType = 0;                  //ֹͣ����
    pOrder->tStopTime = 0;              //ֹͣʱ��

    if(pOrder->plChargeSegment != NULL)
    {
        gdsl_list_flush(pOrder->plChargeSegment);
    }
}
