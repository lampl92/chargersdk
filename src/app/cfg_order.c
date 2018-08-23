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
    cJSON_AddItemToObject(jsNewOrderCfgObj, jnOrderOrderSN, cJSON_CreateNumber((double)(pOrder->ullOrderSN)));                               //1 交易流水
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

ErrorCode_t GetOrderData(cJSON *jsObj, OrderData_t *pOrder)
{
    ErrorCode_t errcode = ERR_NO;
    double dStopEnergy;
    double dOrderSN;
    uint8_t remote_id;
    
    errcode = cfgobj_get_uint8(jsObj, &pOrder->ucStartType, jnOrderStartType);
    cfgobj_get_double(jsObj, &dOrderSN, jnOrderOrderSN);
    pOrder->ullOrderSN = (uint64_t)dOrderSN;
    cfgobj_get_uint8(jsObj, &remote_id, jnOrderCONID);
    pOrder->ucCONID = EchRemoteIDtoCONID(remote_id);
    cfgobj_get_string(jsObj, pOrder->strCardID, jnOrderCardID);
    cfgobj_get_double(jsObj, &pOrder->dStartEnergy, jnOrderStartEnergy);
    cfgobj_get_double(jsObj, &dStopEnergy, jnOrderStopEnergy);
    pOrder->dTotalEnergy = dStopEnergy - pOrder->dStartEnergy;
    cfgobj_get_double(jsObj, &pOrder->dTotalEnergyFee, jnOrderTotalEnergyFee);
    cfgobj_get_double(jsObj, &pOrder->dTotalServFee, jnOrderTotalServFee);
    
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergy[0], jnOrderTotalEnergy_sharp);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergyFee[0], jnOrderTotalEnergyFee_sharp);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalServFee[0], jnOrderTotalServFee_sharp);
    cfgobj_get_uint32(jsObj, &pOrder->ulSegTotalTime[0], jnOrderTotalTime_sharp);
    
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergy[1], jnOrderTotalEnergy_peak);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergyFee[1], jnOrderTotalEnergyFee_peak);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalServFee[1], jnOrderTotalServFee_peak);
    cfgobj_get_uint32(jsObj, &pOrder->ulSegTotalTime[1], jnOrderTotalTime_peak);
    
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergy[2], jnOrderTotalEnergy_shoulder);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergyFee[2], jnOrderTotalEnergyFee_shoulder);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalServFee[2], jnOrderTotalServFee_shoulder);
    cfgobj_get_uint32(jsObj, &pOrder->ulSegTotalTime[2], jnOrderTotalTime_shoulder);    
    
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergy[3], jnOrderTotalEnergy_off_peak);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalEnergyFee[3], jnOrderTotalEnergyFee_off_peak);
    cfgobj_get_double(jsObj, &pOrder->dSegTotalServFee[3], jnOrderTotalServFee_off_peak);
    cfgobj_get_uint32(jsObj, &pOrder->ulSegTotalTime[3], jnOrderTotalTime_off_peak);  

    cfgobj_get_uint32(jsObj, (uint32_t *)&(pOrder->tStartTime), jnOrderStartTime);  
    cfgobj_get_uint8(jsObj, &pOrder->ucStopType, jnOrderStopType);  
    cfgobj_get_uint8(jsObj, &pOrder->ucPayStatus, jnOrderPayStatus);  
    cfgobj_get_uint32(jsObj, (uint32_t *)&(pOrder->tStopTime), jnOrderStopTime);  
    
    return errcode;
}
ErrorCode_t GetOrderBySN(char *path, uint64_t ullOrderSN, OrderData_t *pOrder)
{
    cJSON *jsParent, *jsChild;
    ErrorCode_t errcode;
    uint32_t ulMaxItem;
    int i;
    
    errcode = ERR_NO;
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
            break;
        }
        errcode = GetOrderData(jsChild, pOrder);
        if (errcode != ERR_NO)
        {
            break;
        }
        if (pOrder->ullOrderSN == ullOrderSN)
        {
            errcode = ERR_NO;
            break;
        }
        else
        {
            continue;
        }
    }
    cJSON_Delete(jsParent);
    return errcode;
}
    
ErrorCode_t GetOrderTmp(char *path, OrderData_t *pOrder)
{
    cJSON *jsParent;
    ErrorCode_t errcode;
    double dStopEnergy;
    double dOrderSN;
    uint8_t remote_id;
    
    errcode = ERR_NO;
    jsParent = GetCfgObj(path, &errcode);
    if (jsParent == NULL)
    {
        return errcode;
    }
    GetOrderData(jsParent, pOrder);
    
    cJSON_Delete(jsParent);
    
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
    char buf[80] = { 0 };
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
            printf_safe("OrderSN\t%lf\n", jsItem->valuedouble);
	        
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
