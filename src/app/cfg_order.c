#include "stm32f4xx.h"
#include "bsp.h"
#include "interface.h"
#include "cJSON.h"
#include "utils.h"
#include "evse_config.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "errorcode.h"
#include "yaffsfs.h"
#include "factorycfg.h"
#include <string.h>

static cJSON *CreateNewOrderCfg(OrderData_t *pOrder, echProtocol_t *pProto)
{
    cJSON *jsNewOrderCfgObj;

    jsNewOrderCfgObj = cJSON_CreateObject();
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartType, cJSON_CreateNumber(pOrder->ucStartType));                            //有卡无卡标志
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderOrderSN, cJSON_CreateString(pOrder->strOrderSN));                               //1 交易流水
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderCONID, cJSON_CreateNumber(pOrder->ucCONID + 1));                                //2 充电桩接口
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderCardID, cJSON_CreateString(pOrder->strCardID));                                             //3 卡号
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartPower, cJSON_CreateNumber(pOrder->dStartPower));                           //4 充电前电能总示值
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopPower, cJSON_CreateNumber(pOrder->dStartPower + pOrder->dTotalPower));  //5 充电后电能总示值
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee, cJSON_CreateNumber(pOrder->dTotalPowerFee));                     //6 电费总金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee, cJSON_CreateNumber(pOrder->dTotalServFee));                       //7 服务费总金额
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_sharp, cJSON_CreateNumber(pProto->info.dSegPowerFee[0]));                  //8 尖电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_sharp, cJSON_CreateNumber(pProto->info.dSegServFee[0]));                    //9 尖服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_sharp, cJSON_CreateNumber(pOrder->dSegTotalPower[0]));               //10 尖电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_sharp, cJSON_CreateNumber(pOrder->dSegTotalPowerFee[0]));         //11 尖充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_sharp, cJSON_CreateNumber(pOrder->dSegTotalServFee[0]));           //12 尖服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_sharp, cJSON_CreateNumber(pOrder->ulSegTotalTime[0]));                //13 尖充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_peak, cJSON_CreateNumber(pProto->info.dSegPowerFee[1]));                    //14 峰电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_peak, cJSON_CreateNumber(pProto->info.dSegServFee[1]));                      //15 峰服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_peak, cJSON_CreateNumber(pOrder->dSegTotalPower[1]));                 //16 峰电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_peak, cJSON_CreateNumber(pOrder->dSegTotalPowerFee[1]));           //17 峰充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_peak, cJSON_CreateNumber(pOrder->dSegTotalServFee[1]));             //18 峰服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_peak, cJSON_CreateNumber(pOrder->ulSegTotalTime[1]));                  //19 峰充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_shoulder, cJSON_CreateNumber(pProto->info.dSegPowerFee[2]));            //20 平电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_shoulder, cJSON_CreateNumber(pProto->info.dSegServFee[2]));              //21 平服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_shoulder, cJSON_CreateNumber(pOrder->dSegTotalPower[2]));         //22 平电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_shoulder, cJSON_CreateNumber(pOrder->dSegTotalPowerFee[2]));   //23 平充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_shoulder, cJSON_CreateNumber(pOrder->dSegTotalServFee[2]));     //24 平服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_shoulder, cJSON_CreateNumber(pOrder->ulSegTotalTime[2]));          //25 平充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPowerFee_off_peak, cJSON_CreateNumber(pProto->info.dSegPowerFee[3]));            //26 谷电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_off_peak, cJSON_CreateNumber(pProto->info.dSegServFee[3]));              //27 谷服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPower_off_peak, cJSON_CreateNumber(pOrder->dSegTotalPower[3]));         //28 谷电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalPowerFee_off_peak, cJSON_CreateNumber(pOrder->dSegTotalPowerFee[3]));   //29 谷充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_off_peak, cJSON_CreateNumber(pOrder->dSegTotalServFee[3]));     //30 谷服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_off_peak, cJSON_CreateNumber(pOrder->ulSegTotalTime[3]));          //31 谷充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartTime, cJSON_CreateNumber(pOrder->tStartTime));                             //32 充电开始时间
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime, cJSON_CreateNumber(pOrder->tStopTime - pOrder->tStartTime));     //33 充电持续时间
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopType, cJSON_CreateNumber(pOrder->ucStopType));                              //34 充电停止原因
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderSOC, cJSON_CreateNumber(0));                                                        //35 当前soc
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderPayStatus, cJSON_CreateNumber(pOrder->ucPayStatus));                            //36 支付状态
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopTime, cJSON_CreateNumber(pOrder->tStopTime));                               //37 充电结束时间
    
    return jsNewOrderCfgObj;
}

ErrorCode_t  AddOrderCfg(char *path, OrderData_t *pOrder, echProtocol_t *pProto)
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
        for (i = 0; i < defCfgOrderRemoveOldItem; i++)
        {
            cJSON_DeleteItemFromArray(jsParent, i);
        }
    }
    jsChild = CreateNewOrderCfg(pOrder, pProto);
    cJSON_AddItemToArray(jsParent, jsChild);
    errcode = SetCfgObj(path, jsParent);
    
    return errcode;
}

ErrorCode_t  AddOrderTmp(char *path, OrderData_t *pOrder, echProtocol_t *pProto)
{
    cJSON *jsObj;
    ErrorCode_t errcode;
    pOrder->tStopTime = time(NULL);//添加临时订单时或无停止时间，增加添加临时订单时间作为临时停止时间
    jsObj = CreateNewOrderCfg(pOrder, pProto);
    errcode = SetCfgObj(path, jsObj);
    
    return errcode;
}
void RemoveOrderTmp(char *path)
{
    yaffs_unlink(path);
}

ErrorCode_t GetOrderCfgItem(void *pvCfgObj, char *jnItemName, void *pvCfgItem, uint8_t type)
{
    cJSON *jsItem;
    cJSON *pCfgObj;
    ErrorCode_t errcode;
    
    pCfgObj = (cJSON *)pvCfgObj;
    jsItem = cJSON_GetObjectItem(pCfgObj, jnItemName);
    if (jsItem == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto err_return;
    }
    switch (type)
    {       
    case ParamTypeU8:
        *((uint8_t *)pvCfgItem) = (uint8_t)(jsItem->valueint);
        break;
    case ParamTypeU16:
        *((uint16_t *)pvCfgItem) = (uint16_t)(jsItem->valueint);
        break;
    case ParamTypeU32:
        *((uint32_t *)pvCfgItem) = (uint32_t)(jsItem->valueint);
        break;    
    case ParamTypeS32:
        *((int32_t *)pvCfgItem) = (int32_t)(jsItem->valueint);
        break;
    case ParamTypeDouble:
        *((double *)pvCfgItem) = (double)(jsItem->valuedouble);
        break;
    case ParamTypeString:
        strcpy((uint8_t *)pvCfgItem, jsItem->valuestring);
        break;
    default:
        break;
    }
err_return:
    return errcode;
}
ErrorCode_t GetOrderTmp(char *path, OrderData_t *pOrder)
{
    cJSON *jsParent;
    ErrorCode_t errcode;
    double dStopPower;
    
    errcode = ERR_NO;
    jsParent = GetCfgObj(path, &errcode);
    if (jsParent == NULL)
    {
        return errcode;
    }
    
    GetOrderCfgItem(jsParent, jnOrderStartType, &pOrder->ucStartType, ParamTypeU8);
    GetOrderCfgItem(jsParent, jnOrderOrderSN, pOrder->strOrderSN, ParamTypeString);
    GetOrderCfgItem(jsParent, jnOrderCONID, &pOrder->ucCONID, ParamTypeU8);
    pOrder->ucCONID = pOrder->ucCONID - 1;
    GetOrderCfgItem(jsParent, jnOrderCardID, pOrder->strCardID, ParamTypeString);
    GetOrderCfgItem(jsParent, jnOrderStartPower, &pOrder->dStartPower, ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderStopPower, &dStopPower, ParamTypeDouble);
    pOrder->dTotalPower = dStopPower - pOrder->dStartPower;
    GetOrderCfgItem(jsParent, jnOrderTotalPowerFee, &pOrder->dTotalPowerFee, ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee, &pOrder->dTotalServFee, ParamTypeDouble);

    GetOrderCfgItem(jsParent, jnOrderTotalPower_sharp, &pOrder->dSegTotalPower[0], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalPowerFee_sharp, &pOrder->dSegTotalPowerFee[0], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_sharp, &pOrder->dSegTotalServFee[0], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_sharp, &pOrder->ulSegTotalTime[0], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderTotalPower_peak, &pOrder->dSegTotalPower[1], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalPowerFee_peak, &pOrder->dSegTotalPowerFee[1], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_peak, &pOrder->dSegTotalServFee[1], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_peak, &pOrder->ulSegTotalTime[1], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderTotalPower_shoulder, &pOrder->dSegTotalPower[2], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalPowerFee_shoulder, &pOrder->dSegTotalPowerFee[2], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_shoulder, &pOrder->dSegTotalServFee[2], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_shoulder, &pOrder->ulSegTotalTime[2], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderTotalPower_off_peak, &pOrder->dSegTotalPower[3], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalPowerFee_off_peak, &pOrder->dSegTotalPowerFee[3], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_off_peak, &pOrder->dSegTotalServFee[3], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_off_peak, &pOrder->ulSegTotalTime[3], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderStartTime, &pOrder->tStartTime, ParamTypeU32);
    GetOrderCfgItem(jsParent, jnOrderStopType, &pOrder->ucStopType, ParamTypeU8);
    GetOrderCfgItem(jsParent, jnOrderPayStatus, &pOrder->ucPayStatus, ParamTypeU8);
    GetOrderCfgItem(jsParent, jnOrderStopTime, &pOrder->tStopTime, ParamTypeU32);
    
    return errcode;
}
ErrorCode_t GetNoPayOrder(char *path, OrderData_t *pOrder)
{
    cJSON *jsParent;
    cJSON *jsChild;
    ErrorCode_t errcode;
    uint32_t ulMaxItem;
    int i;
    uint8_t ucPayStatus;
    char strCardID[17] = { 0 };
    double dStopPower;
    
    jsParent = GetCfgObj(path, &errcode);
    if (jsParent == NULL)
    {
        return errcode;
    }
    ulMaxItem  = cJSON_GetArraySize(jsParent);
    
    for (i = 0; i < ulMaxItem; i++)
    {
        jsChild = cJSON_GetArrayItem(jsParent, i);
        if (jsChild == NULL)
        {
            errcode = ERR_FILE_PARSE;
            cJSON_Delete(jsParent);
            return errcode;
        }
        errcode = GetOrderCfgItem(jsChild, jnOrderPayStatus, &ucPayStatus, ParamTypeU8);
        if (errcode != ERR_NO)
        {
            cJSON_Delete(jsParent);
            return errcode;
        }
        if (ucPayStatus == 0)
        {
            break;
        }
    }
    if (i < ulMaxItem)
    {
        jsChild = cJSON_GetArrayItem(jsParent, i);
        if (jsChild == NULL)
        {
            errcode = ERR_FILE_PARSE;
            cJSON_Delete(jsParent);
            return errcode;
        }
        GetOrderCfgItem(jsChild, jnOrderStartType, &pOrder->ucStartType, ParamTypeU8);
        GetOrderCfgItem(jsChild, jnOrderOrderSN, pOrder->strOrderSN, ParamTypeString);
        GetOrderCfgItem(jsChild, jnOrderCONID, &pOrder->ucCONID, ParamTypeU8);
        pOrder->ucCONID = pOrder->ucCONID - 1;
        GetOrderCfgItem(jsChild, jnOrderCardID, pOrder->strCardID, ParamTypeString);
        GetOrderCfgItem(jsChild, jnOrderStartPower, &pOrder->dStartPower, ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderStopPower, &dStopPower, ParamTypeDouble);
        pOrder->dTotalPower = dStopPower - pOrder->dStartPower;
        GetOrderCfgItem(jsChild, jnOrderTotalPowerFee, &pOrder->dTotalPowerFee, ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee, &pOrder->dTotalServFee, ParamTypeDouble);

        GetOrderCfgItem(jsChild, jnOrderTotalPower_sharp, &pOrder->dSegTotalPower[0], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalPowerFee_sharp, &pOrder->dSegTotalPowerFee[0], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_sharp, &pOrder->dSegTotalServFee[0], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_sharp, &pOrder->ulSegTotalTime[0], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderTotalPower_peak, &pOrder->dSegTotalPower[1], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalPowerFee_peak, &pOrder->dSegTotalPowerFee[1], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_peak, &pOrder->dSegTotalServFee[1], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_peak, &pOrder->ulSegTotalTime[1], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderTotalPower_shoulder, &pOrder->dSegTotalPower[2], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalPowerFee_shoulder, &pOrder->dSegTotalPowerFee[2], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_shoulder, &pOrder->dSegTotalServFee[2], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_shoulder, &pOrder->ulSegTotalTime[2], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderTotalPower_off_peak, &pOrder->dSegTotalPower[3], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalPowerFee_off_peak, &pOrder->dSegTotalPowerFee[3], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_off_peak, &pOrder->dSegTotalServFee[3], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_off_peak, &pOrder->ulSegTotalTime[3], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderStartTime, &pOrder->tStartTime, ParamTypeU32);
        GetOrderCfgItem(jsChild, jnOrderStopType, &pOrder->ucStopType, ParamTypeU8);
        GetOrderCfgItem(jsChild, jnOrderPayStatus, &pOrder->ucPayStatus, ParamTypeU8);
        GetOrderCfgItem(jsChild, jnOrderStopTime, &pOrder->tStopTime, ParamTypeU32);
    }
    else
    {
        cJSON_Delete(jsParent);
        return ERR_OTHER;
    }
    cJSON_DeleteItemFromArray(jsParent, i);
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
	struct tm *ts;
	char buf[80];
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
	        jsItem = cJSON_GetObjectItem(jsChild, jnCardID);
	        printf_safe("CardID\t%s\n", jsItem->valuestring);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderOrderSN);
            printf_safe("OrderSN\t%s\n", jsItem->valuestring);
	        
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderStartTime);
	        ts = localtime((time_t*)&(jsItem->valueint));
	        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
            printf_safe("StartTime\t%s\n", buf);
	        
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderTotalPowerFee);
            printf_safe("TotalPowerFee\t%.3lf\n", jsItem->valuedouble);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderTotalServFee);
            printf_safe("TotalServFee\t%.3lf\n", jsItem->valuedouble);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderStopType);
            printf_safe("StopType\t%d\n", jsItem->valueint);
	        
	        jsItem = cJSON_GetObjectItem(jsChild, jnOrderStopTime);
	        ts = localtime((time_t*)&(jsItem->valueint));
	        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	        printf_safe("StopTime\t%s\n", buf);
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
