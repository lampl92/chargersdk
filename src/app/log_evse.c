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

static cJSON *CreateNewEVSELog(uint8_t device, uint8_t level, uint8_t state, uint8_t *msg)
{
    cJSON *jsNewEVSELogObj;

    jsNewEVSELogObj = cJSON_CreateObject();
    cJSON_AddItemToObject(jsNewEVSELogObj, jnLogTime, cJSON_CreateNumber(time(NULL)));                       //有卡无卡标志
    cJSON_AddItemToObject(jsNewEVSELogObj, jnLogDevice, cJSON_CreateNumber(device));                         //0 EVSE 1...n 枪
    cJSON_AddItemToObject(jsNewEVSELogObj, jnLogLevel, cJSON_CreateNumber(level));                           //0 状态 1 告警 2 异常 3 故障
    cJSON_AddItemToObject(jsNewEVSELogObj, jnLogState, cJSON_CreateNumber(state));                           //0 消除 1发生
    cJSON_AddItemToObject(jsNewEVSELogObj, jnLogMessage, cJSON_CreateString(msg));                           //消息
    
    return jsNewEVSELogObj;
}

ErrorCode_t  AddEVSELog(char *path, uint8_t device, uint8_t level, uint8_t state, char *msg)
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
    
    if (ulMaxItem > defCfgLogMaxItem)
    {
        for (i = 0; i < defCfgLogRemoveOldItem; i++)
        {
            cJSON_DeleteItemFromArray(jsParent, i);
        }
    }
    jsChild = CreateNewEVSELog(device, level, state, msg);
    cJSON_AddItemToArray(jsParent, jsChild);
    errcode = SetCfgObj(path, jsParent);
    
    return errcode;
}

int  testSearchEVSELogByTime(char *path, time_t time_start, time_t time_end)
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
	        
            jsItem = cJSON_GetObjectItem(jsChild, jnLogTime);
	        ts = localtime((time_t*)&(jsItem->valueint));
	        strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
	        printf_safe("LogTime\t%s\n", buf);
	        
            jsItem = cJSON_GetObjectItem(jsChild, jnLogDevice);
            printf_safe("LogDevice\t%d\n", jsItem->valueint);
            jsItem = cJSON_GetObjectItem(jsChild, jnLogLevel);
            printf_safe("LogLevel\t%d\n", jsItem->valueint);
            jsItem = cJSON_GetObjectItem(jsChild, jnLogState);
            printf_safe("LogState\t%d\n", jsItem->valueint);
            jsItem = cJSON_GetObjectItem(jsChild, jnLogMessage);
            printf_safe("Message\t%s\n", jsItem->valuestring);
            //........其他条目
        }
    }
    else if (time_start != 0 && time_end != 0)
    {
        for (i = 0; i < ulMaxItem; i++)
        {
            printf_safe("**************Signal log example Arr[%d]*************\n", i);
            jsChild = cJSON_GetArrayItem(jsParent, i);
            jsItem = cJSON_GetObjectItem(jsChild, jnLogTime);
            if (jsItem->valueint <= time_end && jsItem->valueint >= time_start)
            {          
                jsItem = cJSON_GetObjectItem(jsChild, jnLogTime);
                printf_safe("LogTime\t%d\n", jsItem->valueint);
                jsItem = cJSON_GetObjectItem(jsChild, jnLogDevice);
                printf_safe("LogDevice\t%d\n", jsItem->valueint);
                jsItem = cJSON_GetObjectItem(jsChild, jnLogMessage);
                printf_safe("Message\t%s\n", jsItem->valuestring);
            }
            
            //........
        }
    }

    cJSON_Delete(jsParent);
    
    return errcode;
}
