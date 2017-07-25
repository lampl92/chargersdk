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
        return STATE_SEG_OFF_PEAK;
    }
}

/** @brief ״̬��ʱ���д���״̬�����ƽ��  ʱ�Σ�״̬�е�5��ʱ��
 *
 * @param now time_t
 * @param pCON CON_t*
 * @return void
 *
 */
static void SegmentProc(time_t now, CON_t *pCON)
{
    ChargeSegStatus_t *pChargeSegStatus;
    SegTimeState_e statSegTime;
    int i;
    uint8_t pos = 0;//��ǰʱ����ʱ���е�λ��
    statSegTime = STATE_SEG_IDLE;
    double tmpTotalPower; //���ڼ�����ƽ���ܵ���
    uint32_t tmpTotalTime;  //���ڼ�����ƽ���ܳ��ʱ��

    tmpTotalPower = 0;
    tmpTotalTime = 0;
    /*1. ״̬�жϡ�ʱ�����ݴ���*/

    switch(pCON->order.statOrderSeg)
    {
    case STATE_SEG_IDLE:
        pCON->order.statOrderSeg = JudgeSegState(now, pechProto, &pos);//��ȡ��ǰ����״̬
        pCON->order.pos = pos;//��ȡ��ǰ����ʱ��
        break;
    case STATE_SEG_SHARP:
        statSegTime = JudgeSegState(now, pechProto, &pos);//��ȡ��ǰ����״̬
        ///*״̬ �� ʱ�� ����ת���������ϴ�ʱ������*/
        if(pCON->order.statOrderSeg != statSegTime || pCON->order.pos != pos) //��ͬ״̬�²�����ʱ��ת������Ҳ��ʱ����
        {
            pChargeSegStatus = &(pCON->order.chargeSegStatus_sharp[pCON->order.pos]);
            pChargeSegStatus->tEndTime = now; //��ǰת��ʱ�伴�ϴν���ʱ��
            pCON->order.statOrderSeg = statSegTime;
            pCON->order.pos = pos;
            break;//״̬��ת���������û��Ҫִ����
        }
        pChargeSegStatus = &(pCON->order.chargeSegStatus_sharp[pos]);
        if(pChargeSegStatus->tStartTime > 0)
        {
            pChargeSegStatus->dPower = pCON->status.dChargingPower - pChargeSegStatus->dStartPower;
        }
        else
        {
            //��һ�ν������ʱ��
            pCON->order.pos = pos; //״̬ת��ʱ�Ѿ�����ֵ��
            pChargeSegStatus->tStartTime = now;
            pChargeSegStatus->dStartPower = pCON->order.dStartPower;
        }
        break;
    case STATE_SEG_PEAK:
        statSegTime = JudgeSegState(now, pechProto, &pos);//��ȡ��ǰ����״̬
        ///*״̬ �� ʱ�� ����ת���������ϴ�ʱ������*/
        if(pCON->order.statOrderSeg != statSegTime || pCON->order.pos != pos) //��ͬ״̬�²�����ʱ��ת������Ҳ��ʱ����
        {
            pChargeSegStatus = &(pCON->order.chargeSegStatus_peak[pCON->order.pos]);
            pChargeSegStatus->tEndTime = now; //��ǰת��ʱ�伴�ϴν���ʱ��
            pCON->order.statOrderSeg = statSegTime;
            pCON->order.pos = pos;
            break;//״̬��ת���������û��Ҫִ����
        }
        pChargeSegStatus = &(pCON->order.chargeSegStatus_peak[pos]);
        if(pChargeSegStatus->tStartTime > 0)
        {
            pChargeSegStatus->dPower = pCON->status.dChargingPower - pChargeSegStatus->dStartPower;
        }
        else
        {
            //��һ�ν������ʱ��
            pCON->order.pos = pos; //״̬ת��ʱ�Ѿ�����ֵ��
            pChargeSegStatus->tStartTime = now;
            pChargeSegStatus->dStartPower = pCON->order.dStartPower;
        }
        break;
    case STATE_SEG_SHOULDER:
        statSegTime = JudgeSegState(now, pechProto, &pos);//��ȡ��ǰ����״̬
        ///*״̬ �� ʱ�� ����ת���������ϴ�ʱ������*/
        if(pCON->order.statOrderSeg != statSegTime || pCON->order.pos != pos) //��ͬ״̬�²�����ʱ��ת������Ҳ��ʱ����
        {
            pChargeSegStatus = &(pCON->order.chargeSegStatus_shoulder[pCON->order.pos]);
            pChargeSegStatus->tEndTime = now; //��ǰת��ʱ�伴�ϴν���ʱ��
            pCON->order.statOrderSeg = statSegTime;
            pCON->order.pos = pos;
            break;//״̬��ת���������û��Ҫִ����
        }
        pChargeSegStatus = &(pCON->order.chargeSegStatus_shoulder[pos]);
        if(pChargeSegStatus->tStartTime > 0)
        {
            pChargeSegStatus->dPower = pCON->status.dChargingPower - pChargeSegStatus->dStartPower;
        }
        else
        {
            //��һ�ν������ʱ��
            pCON->order.pos = pos; //״̬ת��ʱ�Ѿ�����ֵ��
            pChargeSegStatus->tStartTime = now;
            pChargeSegStatus->dStartPower = pCON->order.dStartPower;
        }
        break;
    case STATE_SEG_OFF_PEAK:
        statSegTime = JudgeSegState(now, pechProto, &pos);//��ȡ��ǰ����״̬
        ///*״̬ �� ʱ�� ����ת���������ϴ�ʱ������*/
        if(pCON->order.statOrderSeg != statSegTime || pCON->order.pos != pos) //��ͬ״̬�²�����ʱ��ת������Ҳ��ʱ����
        {
            pChargeSegStatus = &(pCON->order.chargeSegStatus_off_peak[pCON->order.pos]);
            pChargeSegStatus->tEndTime = now; //��ǰת��ʱ�伴�ϴν���ʱ��
            pCON->order.statOrderSeg = statSegTime;
            pCON->order.pos = pos;
            break;//״̬��ת���������û��Ҫִ����
        }
        pChargeSegStatus = &(pCON->order.chargeSegStatus_off_peak[pos]);
        if(pChargeSegStatus->tStartTime > 0)
        {
            pChargeSegStatus->dPower = pCON->status.dChargingPower - pChargeSegStatus->dStartPower;
        }
        else
        {
            //��һ�ν������ʱ��
            pCON->order.pos = pos; //״̬ת��ʱ�Ѿ�����ֵ��
            pChargeSegStatus->tStartTime = now;
            pChargeSegStatus->dStartPower = pCON->order.dStartPower;
        }
        break;
    default:
        break;
    }

    /*2. ����ʱ��*/
    
    //sharp
    for(i = 0; i < pechProto->info.SegTime_sharp.ucSegCont; i++)
    {
        tmpTotalPower += pCON->order.chargeSegStatus_sharp[i].dPower;
        if(pCON->order.chargeSegStatus_sharp[i].tEndTime != 0) //��ʾ�Ѿ�������ʱ��
        {
            tmpTotalTime += pCON->order.chargeSegStatus_sharp[i].tEndTime - pCON->order.chargeSegStatus_sharp[i].tStartTime;
        }
    }
    pCON->order.dTotalPower_sharp = tmpTotalPower;
    pCON->order.dTotalPowerFee_sharp = tmpTotalPower * (pechProto->info.ulPowerFee_sharp * 0.0001);
    pCON->order.dTotalServFee_sharp = tmpTotalPower * (pechProto->info.ulServiceFee_sharp * 0.0001);
    pCON->order.ulTotalTime_sharp = tmpTotalTime;
    
    //peak
    for(i = 0; i < pechProto->info.SegTime_peak.ucSegCont; i++)
    {
        tmpTotalPower += pCON->order.chargeSegStatus_peak[i].dPower;
        if(pCON->order.chargeSegStatus_peak[i].tEndTime != 0) //��ʾ�Ѿ�������ʱ��
        {
            tmpTotalTime += pCON->order.chargeSegStatus_peak[i].tEndTime - pCON->order.chargeSegStatus_peak[i].tStartTime;
        }
    }
    pCON->order.dTotalPower_peak = tmpTotalPower;
    pCON->order.dTotalPowerFee_peak = tmpTotalPower * (pechProto->info.ulPowerFee_peak * 0.0001);
    pCON->order.dTotalServFee_peak = tmpTotalPower * (pechProto->info.ulServiceFee_peak * 0.0001);
    pCON->order.ulTotalTime_peak = tmpTotalTime;
    
    //shoulder
    for(i = 0; i < pechProto->info.SegTime_shoulder.ucSegCont; i++)
    {
        tmpTotalPower += pCON->order.chargeSegStatus_shoulder[i].dPower;
        if(pCON->order.chargeSegStatus_shoulder[i].tEndTime != 0) //��ʾ�Ѿ�������ʱ��
        {
            tmpTotalTime += pCON->order.chargeSegStatus_shoulder[i].tEndTime - pCON->order.chargeSegStatus_shoulder[i].tStartTime;
        }
    }
    pCON->order.dTotalPower_shoulder = tmpTotalPower;
    pCON->order.dTotalPowerFee_shoulder = tmpTotalPower * (pechProto->info.ulPowerFee_shoulder * 0.0001);
    pCON->order.dTotalServFee_shoulder = tmpTotalPower * (pechProto->info.ulServiceFee_shoulder * 0.0001);
    pCON->order.ulTotalTime_shoulder = tmpTotalTime;
    
    //off_peak
    for(i = 0; i < pechProto->info.SegTime_off_peak.ucSegCont; i++)
    {
        tmpTotalPower += pCON->order.chargeSegStatus_off_peak[i].dPower;
        if(pCON->order.chargeSegStatus_off_peak[i].tEndTime != 0) //��ʾ�Ѿ�������ʱ��
        {
            tmpTotalTime += pCON->order.chargeSegStatus_off_peak[i].tEndTime - pCON->order.chargeSegStatus_off_peak[i].tStartTime;
        }
    }
    pCON->order.dTotalPower_off_peak = tmpTotalPower;
    pCON->order.dTotalPowerFee_off_peak = tmpTotalPower * (pechProto->info.ulPowerFee_off_peak * 0.0001);
    pCON->order.dTotalServFee_off_peak = tmpTotalPower * (pechProto->info.ulServiceFee_off_peak * 0.0001);
    pCON->order.ulTotalTime_off_peak = tmpTotalTime;
    
    /*3. �����ܵ���*/
    pCON->order.dTotalPower = pCON->order.dTotalPower_sharp + 
                                pCON->order.dTotalPower_peak + 
                                pCON->order.dTotalPower_shoulder +
                                pCON->order.dTotalPower_off_peak;
    pCON->order.dTotalPowerFee = pCON->order.dTotalPowerFee_sharp + 
                                pCON->order.dTotalPowerFee_peak + 
                                pCON->order.dTotalPowerFee_shoulder +
                                pCON->order.dTotalPowerFee_off_peak;
    pCON->order.dTotalServFee = pCON->order.dTotalServFee_sharp + 
                                pCON->order.dTotalServFee_peak + 
                                pCON->order.dTotalServFee_shoulder +
                                pCON->order.dTotalServFee_off_peak;
                                
    /*4. �ܷ���*/
    pCON->order.dTotalFee = pCON->order.dTotalPowerFee + pCON->order.dTotalServFee;
}

ErrorCode_t makeOrder(CON_t *pCON)
{
    OrderState_t statOrder;
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

ErrorCode_t testmakeOrder(CON_t *pCON, time_t testtime, OrderState_t statOrder)
{
    ErrorCode_t errcode;

    errcode = ERR_NO;
    switch(statOrder)
    {
    case STATE_ORDER_TMP:
        memmove(pCON->order.ucCardID, pRFIDDev->order.ucCardID, defCardIDLength);
        pCON->order.ucAccountStatus = 1;
        pCON->order.dBalance = 888;
        pCON->order.ucCONID = pCON->info.ucCONID;
        break;
    case STATE_ORDER_MAKE:
        pCON->order.tStartTime = testtime;
        pCON->order.dStartPower = pCON->status.dChargingPower;
        SegmentProc(pCON->order.tStartTime, pCON);
        break;
    case STATE_ORDER_UPDATE:
        SegmentProc(testtime, pCON);
        break;
    case STATE_ORDER_FINISH:
        pCON->order.ucPayType = defOrderPayType_Online;
        pCON->order.tStopTime = testtime;
        break;
    }
    return errcode;
}

void OrderCreate(OrderData_t *pOrder)
{
    //pOrder->plChargeSegment = gdsl_list_alloc("SegList", ChargeSegAlloc, ChargeSegFree);
}

void SegTimeInit(ChargeSegStatus_t *pSegStatus)
{
    pSegStatus->tStartTime = 0;
    pSegStatus->tEndTime = 0;
    pSegStatus->dStartPower = 0;
    pSegStatus->dPower = 0;
}
void OrderInit(OrderData_t *pOrder)
{
    int i;
    pOrder->statOrder = STATE_ORDER_IDLE;
    pOrder->statOrderSeg = STATE_SEG_IDLE;
    pOrder->pos = 0;

    memset(pOrder->ucCardID, 0, defCardIDLength);//����//��taskrfid�и�ֵ
    pOrder->ucAccountStatus = 0;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    pOrder->dBalance = 0;           //���

    memset(pOrder->strOrderSN, '\0', defOrderSNLength);
    pOrder->tStartTime = 0;                 //��ʼʱ��
    pOrder->ucStartType = 0;        //4�п� 5�޿�
    pOrder->dLimitFee = 0;                    //���������
    pOrder->dStartPower = 0;                //��ʼ������

    pOrder->dTotalPower = 0;                 //�ܵ���
    pOrder->dTotalPowerFee = 0;             //�ܵ��
    pOrder->dTotalServFee = 0;               //�����
    pOrder->dTotalFee = 0;                //�ܷ���

    for(i = 0; i < 5; i++)
    {
        SegTimeInit(&(pOrder->chargeSegStatus_sharp[i]));
        SegTimeInit(&(pOrder->chargeSegStatus_peak[i]));
        SegTimeInit(&(pOrder->chargeSegStatus_shoulder[i]));
        SegTimeInit(&(pOrder->chargeSegStatus_off_peak[i]));
    }

    pOrder->ucPayType = 0;               //֧����ʽ 0.��ƽ̨֧�� 1.Ǯ����֧��
    pOrder->ucStopType = 0;                  //ֹͣ����
    pOrder->tStopTime = 0;              //ֹͣʱ��

}
