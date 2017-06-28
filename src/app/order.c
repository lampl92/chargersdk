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

volatile uint8_t *tmpcjson;

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


/** @brief 判断时间边界，包含little，不包含big
 *
 * @param src time_t
 * @param little time_t
 * @param big time_t
 * @return uint8_t 1:在区间中，0：不在区间中
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
        return 2;//时段边界。暂时判断办法，测试时一定要充分测试系统是否可以检测到时段边界。
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
    /*1. 段判断*/
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
                    pChSeg = (ChargeSegment_t *)gdsl_list_get_tail(plChSeg);//要操作的段必然在链表尾
                    if(pChSeg->ucTemplPos != i)//时间已经不在当前段中
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
                        //处理见下面的流程
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
                break;//退出循环
            }
            else if(ucResJudge == 2)
            {
                ucBoundaryPos = i;
                pCON->order.statOrderSeg = STATE_ORDERSEG_END;
                break;//退出循环
            }
            else //当前时间在所有seg中没有找到时间段，执行默认段
            {
                pCON->order.statOrderSeg = STATE_ORDERSEG_DEF;
            }
        }
    }
    else//根本没有seg，需要执行默认段
    {
        pCON->order.statOrderSeg = STATE_ORDERSEG_DEF;
    }
    /*2. 段处理*/
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
//            pCON->order.dDefSegPower = pCON->order.dTotalPower - 时段总电量;
//            pCON->order.dDefSegFee = pCON->order.dDefSegPower * pEVSE->info.dDefSegFee;
//            pCON->order.dTotalPowerFee = pCON->order.dDefSegFee + 时段总电费;
        }
        break;
    case STATE_ORDERSEG_END:
        pTemplSeg = (TemplSeg_t *)(gdsl_list_search_by_position(plTemplSeg, ucBoundaryPos));
        pChSeg = (ChargeSegment_t *)gdsl_list_get_tail(plChSeg);//要操作的段必然在链表尾
        pChSeg->tEndTime = now;
        break;
    default:
        break;
    }
    /*3. 服务费数据*/
    if(pCON->order.ucServiceFeeType == defOrderSerType_Order)//按单
    {
        pCON->order.dTotalServiceFee = pEVSE->info.dServiceFee;
    }
    else if(pCON->order.ucServiceFeeType == defOrderSerType_Power)//按度
    {
        pCON->order.dTotalServiceFee = pCON->order.dTotalPower * pEVSE->info.dServiceFee;
    }
    /*4. 总费用*/
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

    memset(pOrder->ucCardID, 0, defCardIDLength);//卡号//在taskrfid中赋值
    pOrder->ucAccountStatus = 0;    //帐户状态 1：注册卡 0：未注册卡
    pOrder->dBalance = 0;           //余额

    memset(pOrder->strOrderSN, 0, defOrderSNLength);
    pOrder->dLimitFee = 0;                    //充电金额限制
    pOrder->tStartTime = 0;                 //起始时间
    pOrder->ucStartType = 0;        //4有卡 5无卡
    pOrder->dStartPower = 0;                //起始电表读数
    pOrder->ucServiceFeeType = 0;         //服务费类型

    pOrder->dTotalPower = 0;                 //总电量
    pOrder->dTotalPowerFee = 0;             //总电费
    pOrder->dTotalServiceFee = 0;               //服务费
    pOrder->dTotalFee = 0;                //总费用
    pOrder->ucTotalSegment = 0;            //充电明细段数
    pOrder->dDefSegStartPower = 0;          //默认段起始电表读数
    pOrder->dDefSegPower = 0;              //默认段电量
    pOrder->dDefSegFee = 0;               //默认段电费

    pOrder->ucPayType = 0;               //支付方式 0.云平台支付 1.钱包卡支付
    pOrder->ucStopType = 0;                  //停止类型
    pOrder->tStopTime = 0;              //停止时间

    if(pOrder->plChargeSegment != NULL)
    {
        gdsl_list_flush(pOrder->plChargeSegment);
    }
}
#if 0
void saveOrder(CON_t *pCON)
{
	int temp;
    FIL fp;
    ErrorCode_t errcode;
    UINT bw;
    char result;
    uint8_t *p;
    cJSON *pJsonRoot = NULL;
    cJSON *jsOrderObj = NULL;
    cJSON *pSub = NULL;
    cJSON *pSubJson = NULL;
    cJSON *pSubJsonSeg = NULL;
    cJSON *seg0,*seg1,*seg2,*seg3,*seg4,*seg5,*seg6,*seg7;// = NULL;

    f_open(&fp, "system/order.txt", FA_CREATE_NEW | FA_WRITE);

    pJsonRoot = GetCfgObj("system/order.txt", &errcode);
    if(pJsonRoot == NULL || errcode != ERR_NO)
    {
        return 1;
    }
    tmpcjson = cJSON_Print(pJsonRoot);

    pSub = cJSON_GetObjectItem(pJsonRoot,"MaxIndex");

    if(pSub == NULL)
    {
        cJSON_Delete(pSub);
        return 1;
    }

    pSub->valueint = pSub->valueint+1;
    cJSON_ReplaceItemInObject(pJsonRoot,"MaxIndex",cJSON_CreateNumber(*((uint32_t *)(pSub->valueint))));
    jsOrderObj = cJSON_GetObjectItem(pJsonRoot,"Order");//取得数组

    pSubJson = cJSON_CreateObject();
    if(pSubJson == NULL)
    {
        cJSON_Delete(pSubJson);
        return 1;
    }
    cJSON_AddItemToArray(pSubJson,jsOrderObj);

    cJSON_AddNumberToObject(pSubJson,"Index",pSub->valueint);
    cJSON_AddStringToObject(pSubJson, "EVSEID",&(pCON->order.ucCardID));//数据错误
    cJSON_AddNumberToObject(pSubJson,"CONID",pCON->order.ucCONID);
    cJSON_AddNumberToObject(pSubJson,"TotalPower",pCON->order.dTotalPower);
    cJSON_AddNumberToObject(pSubJson,"PayType",pCON->order.ucPayType);
    cJSON_AddNumberToObject(pSubJson,"StopType",pCON->order.ucStopType);
    cJSON_AddStringToObject(pSubJson,"CardID",&(pCON->order.ucCardID));
    cJSON_AddNumberToObject(pSubJson,"TotalFee",pCON->order.dTotalFee);
    cJSON_AddStringToObject(pSubJson,"OrderSN",&(pCON->order.ucCardID));//数据错误
    cJSON_AddNumberToObject(pSubJson,"ServiceFeeType",pCON->order.ucServiceFeeType);
    cJSON_AddNumberToObject(pSubJson,"ServiceFee",pCON->order.dTotalServiceFee);
    cJSON_AddNumberToObject(pSubJson,"TotalSegment",pCON->order.ucTotalSegment);
    cJSON_AddNumberToObject(pSubJson,"DefSegPower",pCON->order.dDefSegPower);
    cJSON_AddNumberToObject(pSubJson,"DefSegFee",pCON->order.dDefSegFee);

    pSubJsonSeg = cJSON_CreateArray();
    cJSON_AddItemToObject(pSubJson,"Segments",pSubJsonSeg);
    seg0 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg0",seg0);
    cJSON_AddNumberToObject(seg0,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg0,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg0,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg0,"StartTime",pCON->order.tStartTime);
    seg1 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg1",seg1);
    cJSON_AddNumberToObject(seg1,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg1,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg1,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg1,"StartTime",pCON->order.tStartTime);
    seg2 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg2",seg2);
    cJSON_AddNumberToObject(seg2,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg2,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg2,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg2,"StartTime",pCON->order.tStartTime);
    seg3 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg3",seg3);
    cJSON_AddNumberToObject(seg3,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg3,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg3,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg3,"StartTime",pCON->order.tStartTime);
    seg4 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg4",seg4);
    cJSON_AddNumberToObject(seg4,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg4,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg4,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg4,"StartTime",pCON->order.tStartTime);
    seg5 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg5",seg5);
    cJSON_AddNumberToObject(seg5,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg5,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg5,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg5,"StartTime",pCON->order.tStartTime);
    seg6 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg6",seg6);
    cJSON_AddNumberToObject(seg6,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg6,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg6,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg6,"StartTime",pCON->order.tStartTime);
    seg7 = cJSON_CreateObject();
    cJSON_AddItemToObject(pSubJsonSeg,"seg7",seg7);
    cJSON_AddNumberToObject(seg7,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg7,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg7,"StartTime",pCON->order.tStartTime);
    cJSON_AddNumberToObject(seg7,"StartTime",pCON->order.tStartTime);

    //p = cJSON_Print(pJsonRoot);
//    f_write(&fp, p, strlen(p), &bw);

    f_close(&fp);
//    SetCfgObj("system/order.txt",pJsonRoot);

    cJSON_Delete(pJsonRoot);
    return 0;
}
#endif

