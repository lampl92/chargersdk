#include "stm32f4xx.h"
#include "bsp.h"
#include "interface.h"
#include "cJSON.h"
#include "utils.h"
#include "evse_config.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "errorcode.h"
#include <string.h>

static cJSON *CreateNewOrderCfg(CON_t *pCON, echProtocol_t *pProto)
{
    cJSON *jsNewOrderCfgObj;
    uint8_t strCardID[17] = { 0 };

    HexToStr(pCON->order.ucCardID, strCardID, 8);
    jsNewOrderCfgObj = cJSON_CreateObject();
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartType, cJSON_CreateNumber(pCON->order.ucStartType));                            //有卡无卡标志
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderOrderSN, cJSON_CreateString(pCON->order.strOrderSN));                               //1 交易流水
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderCONID, cJSON_CreateNumber(pCON->order.ucCONID + 1));                                //2 充电桩接口
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderCardID, cJSON_CreateString(strCardID));                                             //3 卡号
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartPower, cJSON_CreateNumber(pCON->order.dStartPower));                           //4 充电前电能总示值
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopPower, cJSON_CreateNumber(pCON->order.dStartPower + pCON->order.dTotalPower));  //5 充电后电能总示值
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee, cJSON_CreateNumber(pCON->order.dTotalPowerFee));                     //6 电费总金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee, cJSON_CreateNumber(pCON->order.dTotalServFee));                       //7 服务费总金额
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_sharp, cJSON_CreateNumber(pProto->info.dPowerFee_sharp));                  //8 尖电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_sharp, cJSON_CreateNumber(pProto->info.dServFee_sharp));                    //9 尖服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_sharp, cJSON_CreateNumber(pCON->order.dTotalPower_sharp));               //10 尖电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_sharp, cJSON_CreateNumber(pCON->order.dTotalPowerFee_sharp));         //11 尖充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_sharp, cJSON_CreateNumber(pCON->order.dTotalServFee_sharp));           //12 尖服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_sharp, cJSON_CreateNumber(pCON->order.ulTotalTime_sharp));                //13 尖充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_peak, cJSON_CreateNumber(pProto->info.dPowerFee_peak));                    //14 峰电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_peak, cJSON_CreateNumber(pProto->info.dServFee_peak));                      //15 峰服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_peak, cJSON_CreateNumber(pCON->order.dTotalPower_peak));                 //16 峰电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_peak, cJSON_CreateNumber(pCON->order.dTotalPowerFee_peak));           //17 峰充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_peak, cJSON_CreateNumber(pCON->order.dTotalServFee_peak));             //18 峰服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_peak, cJSON_CreateNumber(pCON->order.ulTotalTime_peak));                  //19 峰充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_shoulder, cJSON_CreateNumber(pProto->info.dPowerFee_shoulder));            //20 平电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_shoulder, cJSON_CreateNumber(pProto->info.dServFee_shoulder));              //21 平服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_shoulder, cJSON_CreateNumber(pCON->order.dTotalPower_shoulder));         //22 平电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_shoulder, cJSON_CreateNumber(pCON->order.dTotalPowerFee_shoulder));   //23 平充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_shoulder, cJSON_CreateNumber(pCON->order.dTotalServFee_shoulder));     //24 平服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_shoulder, cJSON_CreateNumber(pCON->order.ulTotalTime_shoulder));          //25 平充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_off_peak, cJSON_CreateNumber(pProto->info.dPowerFee_off_peak));            //26 谷电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_off_peak, cJSON_CreateNumber(pProto->info.dServFee_off_peak));              //27 谷服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_off_peak, cJSON_CreateNumber(pCON->order.dTotalPower_off_peak));         //28 谷电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_off_peak, cJSON_CreateNumber(pCON->order.dTotalPowerFee_off_peak));   //29 谷充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_off_peak, cJSON_CreateNumber(pCON->order.dTotalServFee_off_peak));     //30 谷服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_off_peak, cJSON_CreateNumber(pCON->order.ulTotalTime_off_peak));          //31 谷充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartTime, cJSON_CreateNumber(pCON->order.tStartTime));                             //32 充电开始时间
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime, cJSON_CreateNumber(pCON->order.tStopTime - pCON->order.tStartTime));     //33 充电持续时间
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopType, cJSON_CreateNumber(pCON->order.ucStopType));                              //34 充电停止原因
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderSOC, cJSON_CreateNumber(0));                                                        //35 当前soc
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPayStatus, cJSON_CreateNumber(pCON->order.ucPayStatus));                            //36 支付状态
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopTime, cJSON_CreateNumber(pCON->order.tStopTime));                               //37 充电结束时间
    
    return jsNewOrderCfgObj;
}

ErrorCode_t  AddOrderCfg(char *path, CON_t *pCON, echProtocol_t *pProto)
{
    cJSON *jsParent;
    cJSON *jsChild;
    ErrorCode_t errcode;
    uint32_t ulMaxItem;
    int i;
    jsParent = GetCfgObj(path, &errcode);
    if (jsParent == NULL)
    {
        return errcode;
    }
    ulMaxItem  = cJSON_GetArraySize(jsParent);
    
    if (ulMaxItem > defCfgOrderMaxItem)
    {
        for (i = 0; i < 10; i++)
        {
            cJSON_DeleteItemFromArray(jsParent, i);
        }
    }
    jsChild = CreateNewOrderCfg(pCON, pProto);
    cJSON_AddItemToArray(jsParent, jsChild);
    errcode = SetCfgObj(path, jsParent);
    
    return errcode;
}

int  testSearchOrderCfg(char *path, time_t time_start, time_t time_end)
{
    cJSON *jsParent;
    cJSON *jsChild;
    cJSON *jsItem;
    ErrorCode_t errcode;
    uint32_t ulMaxItem;
    int i;
    jsParent = GetCfgObj(path, &errcode);
    if (jsParent == NULL)
    {
        return errcode;
    }
    ulMaxItem  = cJSON_GetArraySize(jsParent);
    if (ulMaxItem == 0)
    {
        return 0;
    }
    if (time_start == 0 || time_end == 0)
    {
        for (i = 0; i < ulMaxItem; i++)
        {
            printf_safe("**************Signal Item example Arr[%d]*************\n", i);
            jsChild = cJSON_GetArrayItem(jsParent, i);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderStartType);
            printf_safe("StartType\t%d\n", jsItem->valueint);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderOrderSN);
            printf_safe("OrderSN\t%s\n", jsItem->valuestring);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderStartTime);
            printf_safe("StartTime\t%d\n", jsItem->valueint);
            //........
        }
    }
    else if(time_start != 0 && time_end != 0)
    {
        for (i = 0; i < ulMaxItem; i++)
        {
            printf_safe("**************Signal Item example Arr[%d]*************\n", i);
            jsChild = cJSON_GetArrayItem(jsParent, i);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderStartTime);
            if (jsItem->valueint <= time_end && jsItem->valueint >= time_start)
            {          
                jsItem = cJSON_GetObjectItem(jsParent, jnOrderStartType);
                printf_safe("StartType\t%d\n", jsItem->valueint);
                jsItem = cJSON_GetObjectItem(jsParent, jnOrderOrderSN);
                printf_safe("OrderSN\t%s\n", jsItem->valuestring);
                jsItem = cJSON_GetObjectItem(jsChild, jnOrderStartTime);
                printf_safe("StartTime\t%d\n", jsItem->valueint);  
            }
            
            //........
        }
    }

    cJSON_Delete(jsParent);
    
    return errcode;
}
