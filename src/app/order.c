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

/** @brief 判断当前时间是否在时间段内
 *
 * @param now time_t
 * @param ucStart uint8_t
 * @param ucEnd uint8_t
 * @return uint8_t 1：在时间段内， 0：不在时间段内
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

/** @brief 判断当前时间是否在尖峰平谷某个时间段中
 *
 * @param now time_t
 * @param ucStart uint8_t
 * @param ucEnd uint8_t
 * @return uint8_t  1：在时间段中  0：不在时间段中
 *
 */
static uint8_t JudgeSegInclude(time_t now, EchSegTime_t SegTime, uint8_t *ppos)
{
    int i;
    uint8_t isInclude = 0;

    for(i = 0; i < SegTime.ucPeriodCont; i++)
    {
        isInclude = JudgeTimeInclude(now, SegTime.ucStart[i], SegTime.ucEnd[i]);
        if(isInclude == 1)
        {
            *ppos = (uint8_t)i;
            return 1;
        }
    }
    return 0;
}

/** @brief 判断当前时间所在的尖峰平谷状态
 *
 * @param now time_t
 * @param pProto echProtocol_t*
 * @param uint8_t pos* 当前时间所时间段内置
 * @return SegTimeState_e
 *
 */
OrderSegState_e JudgeSegState(time_t now, echProtocol_t *pProto, uint8_t *ppos)
{
    int i;
    for (i = 0; i < defOrderSegMax; i++)
    {
        if(JudgeSegInclude(now, pProto->info.SegTime[i], ppos) == 1)
        {
            return (OrderSegState_e)i;
        }
    }
    return STATE_SEG_DEFAULT;
}

double get_current_totalfee(time_t now)
{
    uint8_t pos = 0;
    uint8_t ucSegState;
    double totalfee;
    
    ucSegState = (uint8_t)JudgeSegState(now, pechProto, &pos);
    totalfee = pechProto->info.dSegPowerFee[ucSegState] + pechProto->info.dSegServFee[ucSegState];
 
    return totalfee;
}
ChargePeriodStatus_t *PeriodUpdate(time_t now, CON_t *pCON, OrderSegState_e statOrderSeg)
{
    ChargePeriodStatus_t *pPeriodStatus;
    OrderSegState_e statSegNow;
    uint8_t pos = 0;//当前时间在时段中的位置
    
    statSegNow = JudgeSegState(now, pechProto, &pos);//获取当前所在状态
        ///*状态 或 时段 发生转换，处理上次时段内容*/
    if (pCON->order.statOrderSeg != statSegNow || pCON->order.pos != pos) //相同状态时段转换只有在 0 点时刻发生
    {
        pPeriodStatus = &(pCON->order.chargeSegStatus[statOrderSeg][pCON->order.pos]);
        pPeriodStatus->tEndTime = now; //当前转换时间即上次结束时间
        pCON->order.statOrderSeg = statSegNow;
        pCON->order.pos = pos;
        return pPeriodStatus;//状态已转换，下面的没必要执行了
    }
    pPeriodStatus = &(pCON->order.chargeSegStatus[statOrderSeg][pos]);
    if (pPeriodStatus->tStartTime > 0)
    {
        pPeriodStatus->dPower = pCON->status.dChargingPower - pPeriodStatus->dStartPower;
    }
    else
    {
        //第一次进到这个时段
        pCON->order.pos = pos; //状态转换时已经赋过值了
        pPeriodStatus->tStartTime = now;
        pPeriodStatus->dStartPower = pCON->status.dChargingPower;
    }
    return pPeriodStatus;
}

/** @brief 状态与时段判处理。状态：尖峰平谷  时段：状态中的5个时段
 *
 * @param now time_t
 * @param pCON CON_t*
 * @return void
 *
 */
static void SegmentUpdate(time_t now, CON_t *pCON, OrderState_t statOrder)
{
    ChargePeriodStatus_t *pPeriodStatus;
    uint8_t pos = 0;//当前时间在时段中的位置
    int i, j;
    double tmpTotalPower = 0; //用于计算尖峰平谷总电量
    double tmpTotalPowerFee = 0;
    double tmpTotalServFee = 0;
    uint32_t tmpTotalTime = 0;  //用于计算尖峰平谷总充电时间

    /*1. 状态判断、时段内容处理*/
    switch(pCON->order.statOrderSeg)
    {
    case STATE_SEG_IDLE:
        pCON->order.statOrderSeg = JudgeSegState(now, pechProto, &pos);//获取当前所在状态
        pCON->order.pos = pos;//获取当前所在时段
        break;
    case STATE_SEG_SHARP:
        pPeriodStatus = PeriodUpdate(now, pCON, STATE_SEG_SHARP);
        break;
    case STATE_SEG_PEAK:
        pPeriodStatus = PeriodUpdate(now, pCON, STATE_SEG_PEAK);
        break;
    case STATE_SEG_SHOULDER:
        pPeriodStatus = PeriodUpdate(now, pCON, STATE_SEG_SHOULDER);
        break;
    case STATE_SEG_OFF_PEAK:
        pPeriodStatus = PeriodUpdate(now, pCON, STATE_SEG_OFF_PEAK);
        break;   
    case STATE_SEG_DEFAULT:
        pPeriodStatus = PeriodUpdate(now, pCON, STATE_SEG_DEFAULT);
        break;
    default:
        pPeriodStatus = PeriodUpdate(now, pCON, STATE_SEG_DEFAULT);
        break;
    }
    if(statOrder == STATE_ORDER_FINISH)
    {
        pPeriodStatus->tEndTime = now; //pChargeSegStatus 指针已经在上面的switch中获取，所以这条判断语句位置不能动
    }

    /*2. 汇总时段*/
    for (i = 0; i < defOrderSegMax; i++)
    {
        tmpTotalPower = 0;
        tmpTotalTime = 0;
        //  ↓:j    ↓:j                                          ↓:j
        for(j = 0; j < pechProto->info.SegTime[i].ucPeriodCont; j++)
        {
            tmpTotalPower += pCON->order.chargeSegStatus[i][j].dPower;
            if (pCON->order.chargeSegStatus[i][j].tEndTime != 0) //不为0表示时段已经结束
            {
                tmpTotalTime += (pCON->order.chargeSegStatus[i][j].tEndTime - pCON->order.chargeSegStatus[i][j].tStartTime);
            }
        }
        pCON->order.dSegTotalPower[i] = (uint32_t)(tmpTotalPower * 100) / 100.0; //防止sprintf对double精度进行四舍五入
        pCON->order.dSegTotalPowerFee[i] = tmpTotalPower * pechProto->info.dSegPowerFee[i];
        pCON->order.dSegTotalServFee[i] = tmpTotalPower * pechProto->info.dSegServFee[i];
        pCON->order.ulSegTotalTime[i] = tmpTotalTime;
    }

    /*3. 汇总总电量*/
    tmpTotalPower = 0;
    tmpTotalPowerFee = 0;
    tmpTotalServFee = 0;
    for (i = 0; i < defOrderSegMax; i++)
    {
        tmpTotalPower += pCON->order.dSegTotalPower[i];
        tmpTotalPowerFee += pCON->order.dSegTotalPowerFee[i];
        tmpTotalServFee += pCON->order.dSegTotalServFee[i];
    }
    pCON->order.dTotalPower = tmpTotalPower;
    pCON->order.dTotalPowerFee = (uint32_t)(tmpTotalPowerFee * 100) / 100.0;
    pCON->order.dTotalServFee = (uint32_t)(tmpTotalServFee * 100) / 100.0;
    /*4. 总费用*/
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
        strcpy(pCON->order.strCardID, pRFIDDev->order.strCardID);
        pCON->order.ucAccountStatus = pRFIDDev->order.ucAccountStatus;
        pCON->order.ucCardStatus = pRFIDDev->order.ucCardStatus;
        pCON->order.dBalance = pRFIDDev->order.dBalance;
        pCON->order.ucCONID = pCON->info.ucCONID;
        pCON->order.dLimitFee = pRFIDDev->order.dLimitFee;
        pCON->order.ulLimitTime = pRFIDDev->order.ulLimitTime;
        strcpy(pCON->order.strOrderSN, pRFIDDev->order.strOrderSN);
        break;
    case STATE_ORDER_MAKE:
        pCON->order.tStartTime = time(NULL);
        pCON->order.dStartPower = pCON->status.dChargingPower;
        SegmentUpdate(pCON->order.tStartTime, pCON, statOrder);
        break;
    case STATE_ORDER_UPDATE:
        SegmentUpdate(time(NULL), pCON, statOrder);
        break;
    case STATE_ORDER_FINISH:
        pCON->order.ucPayType = defOrderPayType_Online;
        pCON->order.tStopTime = time(NULL);
        SegmentUpdate(pCON->order.tStopTime, pCON, statOrder);
        break;
    default:
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
        strcpy(pCON->order.strCardID, pRFIDDev->order.strCardID);
        pCON->order.ucAccountStatus = 1;
        pCON->order.dBalance = 888;
        pCON->order.ucCONID = pCON->info.ucCONID;
        break;
    case STATE_ORDER_MAKE:
        pCON->order.tStartTime = testtime;
        pCON->order.dStartPower = pCON->status.dChargingPower;
        SegmentUpdate(pCON->order.tStartTime, pCON, statOrder);
        break;
    case STATE_ORDER_UPDATE:
        SegmentUpdate(testtime, pCON, statOrder);
        break;
    case STATE_ORDER_FINISH:
        pCON->order.ucPayType = defOrderPayType_Online;
        pCON->order.tStopTime = testtime;
        SegmentUpdate(pCON->order.tStopTime, pCON, statOrder);
        break;
    default:
        break;
    }
    return errcode;
}

void OrderInit(OrderData_t *pOrder)
{
    memset(pOrder, 0, sizeof(OrderData_t));
    pOrder->statOrder = STATE_ORDER_IDLE;
    pOrder->statOrderSeg = STATE_SEG_IDLE;
	pOrder->statRemoteProc.card.stat = CARDCTRL_IDLE;
	pOrder->statRemoteProc.order.stat = REMOTEOrder_IDLE;
    pOrder->statRemoteProc.rmt_ctrl.stat = REMOTECTRL_IDLE;
    pOrder->statRemoteProc.rtdata.stat = REMOTERTData_IDLE;
}
