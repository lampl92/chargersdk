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
#include "libEcharge/ech_globals.h"
#include <string.h>
//程序中的Order枪号从0开始，文件中的枪号从1开始
static cJSON *CreateNewOrderCfg(OrderData_t *pOrder, echProtocol_t *pProto)
{
    cJSON *jsNewOrderCfgObj;

    jsNewOrderCfgObj = cJSON_CreateObject();
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartType, cJSON_CreateNumber(pOrder->ucStartType));                            //有卡无卡标志
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderOrderSN, cJSON_CreateString(pOrder->strOrderSN));                               //1 交易流水
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderCONID, cJSON_CreateNumber(EchCONIDtoRemoteID(pOrder->ucCONID, pEVSE->info.ucTotalCON)));                                 //2 充电桩接口
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderCardID, cJSON_CreateString(pOrder->strCardID));                                             //3 卡号
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStartEnergy, cJSON_CreateNumber(pOrder->dStartEnergy));                           //4 充电前电能总示值
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderStopEnergy, cJSON_CreateNumber(pOrder->dStartEnergy + pOrder->dTotalEnergy));  //5 充电后电能总示值
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergyFee, cJSON_CreateNumber(pOrder->dTotalEnergyFee));                     //6 电费总金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee, cJSON_CreateNumber(pOrder->dTotalServFee));                       //7 服务费总金额
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderEnergyFee_sharp, cJSON_CreateNumber(pProto->info.dSegEnergyFee[0]));                  //8 尖电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_sharp, cJSON_CreateNumber(pProto->info.dSegServFee[0]));                    //9 尖服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergy_sharp, cJSON_CreateNumber(pOrder->dSegTotalEnergy[0]));               //10 尖电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergyFee_sharp, cJSON_CreateNumber(pOrder->dSegTotalEnergyFee[0]));         //11 尖充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_sharp, cJSON_CreateNumber(pOrder->dSegTotalServFee[0]));           //12 尖服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_sharp, cJSON_CreateNumber(pOrder->ulSegTotalTime[0]));                //13 尖充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderEnergyFee_peak, cJSON_CreateNumber(pProto->info.dSegEnergyFee[1]));                    //14 峰电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_peak, cJSON_CreateNumber(pProto->info.dSegServFee[1]));                      //15 峰服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergy_peak, cJSON_CreateNumber(pOrder->dSegTotalEnergy[1]));                 //16 峰电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergyFee_peak, cJSON_CreateNumber(pOrder->dSegTotalEnergyFee[1]));           //17 峰充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_peak, cJSON_CreateNumber(pOrder->dSegTotalServFee[1]));             //18 峰服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_peak, cJSON_CreateNumber(pOrder->ulSegTotalTime[1]));                  //19 峰充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderEnergyFee_shoulder, cJSON_CreateNumber(pProto->info.dSegEnergyFee[2]));            //20 平电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_shoulder, cJSON_CreateNumber(pProto->info.dSegServFee[2]));              //21 平服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergy_shoulder, cJSON_CreateNumber(pOrder->dSegTotalEnergy[2]));         //22 平电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergyFee_shoulder, cJSON_CreateNumber(pOrder->dSegTotalEnergyFee[2]));   //23 平充电金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalServFee_shoulder, cJSON_CreateNumber(pOrder->dSegTotalServFee[2]));     //24 平服务费金额
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalTime_shoulder, cJSON_CreateNumber(pOrder->ulSegTotalTime[2]));          //25 平充电时长
    
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderEnergyFee_off_peak, cJSON_CreateNumber(pProto->info.dSegEnergyFee[3]));            //26 谷电价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderServFee_off_peak, cJSON_CreateNumber(pProto->info.dSegServFee[3]));              //27 谷服务费单价
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergy_off_peak, cJSON_CreateNumber(pOrder->dSegTotalEnergy[3]));         //28 谷电量
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderTotalEnergyFee_off_peak, cJSON_CreateNumber(pOrder->dSegTotalEnergyFee[3]));   //29 谷充电金额
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
    errcode = SetCfgObj(path, jsParent, 0);
    
    return errcode;
}

ErrorCode_t  AddOrderTmp(char *path, OrderData_t *pOrder, echProtocol_t *pProto)
{
    cJSON *jsObj;
    ErrorCode_t errcode;
    pOrder->tStopTime = time(NULL);//添加临时订单时或无停止时间，增加添加临时订单时间作为临时停止时间
    jsObj = CreateNewOrderCfg(pOrder, pProto);
    errcode = SetCfgObj(path, jsObj, 0);
    
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
        *((uint32_t *)pvCfgItem) = (uint32_t)(jsItem->valuedouble);
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
    double dStopEnergy;
    uint8_t remote_id;
    
    errcode = ERR_NO;
    jsParent = GetCfgObj(path, &errcode);
    if (jsParent == NULL)
    {
        return errcode;
    }
    
    GetOrderCfgItem(jsParent, jnOrderStartType, &pOrder->ucStartType, ParamTypeU8);
    GetOrderCfgItem(jsParent, jnOrderOrderSN, pOrder->strOrderSN, ParamTypeString);
    GetOrderCfgItem(jsParent, jnOrderCONID, &remote_id, ParamTypeU8);
    pOrder->ucCONID = EchRemoteIDtoCONID(remote_id);
    GetOrderCfgItem(jsParent, jnOrderCardID, pOrder->strCardID, ParamTypeString);
    GetOrderCfgItem(jsParent, jnOrderStartEnergy, &pOrder->dStartEnergy, ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderStopEnergy, &dStopEnergy, ParamTypeDouble);
    pOrder->dTotalEnergy = dStopEnergy - pOrder->dStartEnergy;
    GetOrderCfgItem(jsParent, jnOrderTotalEnergyFee, &pOrder->dTotalEnergyFee, ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee, &pOrder->dTotalServFee, ParamTypeDouble);

    GetOrderCfgItem(jsParent, jnOrderTotalEnergy_sharp, &pOrder->dSegTotalEnergy[0], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalEnergyFee_sharp, &pOrder->dSegTotalEnergyFee[0], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_sharp, &pOrder->dSegTotalServFee[0], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_sharp, &pOrder->ulSegTotalTime[0], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderTotalEnergy_peak, &pOrder->dSegTotalEnergy[1], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalEnergyFee_peak, &pOrder->dSegTotalEnergyFee[1], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_peak, &pOrder->dSegTotalServFee[1], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_peak, &pOrder->ulSegTotalTime[1], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderTotalEnergy_shoulder, &pOrder->dSegTotalEnergy[2], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalEnergyFee_shoulder, &pOrder->dSegTotalEnergyFee[2], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_shoulder, &pOrder->dSegTotalServFee[2], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_shoulder, &pOrder->ulSegTotalTime[2], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderTotalEnergy_off_peak, &pOrder->dSegTotalEnergy[3], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalEnergyFee_off_peak, &pOrder->dSegTotalEnergyFee[3], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalServFee_off_peak, &pOrder->dSegTotalServFee[3], ParamTypeDouble);
    GetOrderCfgItem(jsParent, jnOrderTotalTime_off_peak, &pOrder->ulSegTotalTime[3], ParamTypeDouble);
        
    GetOrderCfgItem(jsParent, jnOrderStartTime, &pOrder->tStartTime, ParamTypeU32);
    GetOrderCfgItem(jsParent, jnOrderStopType, &pOrder->ucStopType, ParamTypeU8);
    GetOrderCfgItem(jsParent, jnOrderPayStatus, &pOrder->ucPayStatus, ParamTypeU8);
    GetOrderCfgItem(jsParent, jnOrderStopTime, &pOrder->tStopTime, ParamTypeU32);
    
    cJSON_Delete(jsParent);
    
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
    double dStopEnergy;
    
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
        GetOrderCfgItem(jsChild, jnOrderStartEnergy, &pOrder->dStartEnergy, ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderStopEnergy, &dStopEnergy, ParamTypeDouble);
        pOrder->dTotalEnergy = dStopEnergy - pOrder->dStartEnergy;
        GetOrderCfgItem(jsChild, jnOrderTotalEnergyFee, &pOrder->dTotalEnergyFee, ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee, &pOrder->dTotalServFee, ParamTypeDouble);

        GetOrderCfgItem(jsChild, jnOrderTotalEnergy_sharp, &pOrder->dSegTotalEnergy[0], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalEnergyFee_sharp, &pOrder->dSegTotalEnergyFee[0], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_sharp, &pOrder->dSegTotalServFee[0], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_sharp, &pOrder->ulSegTotalTime[0], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderTotalEnergy_peak, &pOrder->dSegTotalEnergy[1], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalEnergyFee_peak, &pOrder->dSegTotalEnergyFee[1], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_peak, &pOrder->dSegTotalServFee[1], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_peak, &pOrder->ulSegTotalTime[1], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderTotalEnergy_shoulder, &pOrder->dSegTotalEnergy[2], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalEnergyFee_shoulder, &pOrder->dSegTotalEnergyFee[2], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalServFee_shoulder, &pOrder->dSegTotalServFee[2], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalTime_shoulder, &pOrder->ulSegTotalTime[2], ParamTypeDouble);
        
        GetOrderCfgItem(jsChild, jnOrderTotalEnergy_off_peak, &pOrder->dSegTotalEnergy[3], ParamTypeDouble);
        GetOrderCfgItem(jsChild, jnOrderTotalEnergyFee_off_peak, &pOrder->dSegTotalEnergyFee[3], ParamTypeDouble);
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
    errcode = SetCfgObj(path, jsParent, 0);
    
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
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderCONID);
            printf_safe("CONID\t%d\n", jsItem->valueint);
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
	        
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderTotalEnergyFee);
            printf_safe("TotalEnergyFee\t%.3lf\n", jsItem->valuedouble);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderTotalServFee);
            printf_safe("TotalServFee\t%.3lf\n", jsItem->valuedouble);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderStopType);
            printf_safe("StopType\t%d\n", jsItem->valueint);
            jsItem = cJSON_GetObjectItem(jsChild, jnOrderPayStatus);
            printf_safe("PayStatus\t%d\n", jsItem->valueint);
	        
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
