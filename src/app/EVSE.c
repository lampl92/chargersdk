/**
* @file EVSE.c
* @brief EVSE初始化
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "evse_globals.h"
#include "userlib_list.h"
#include "interface.h"
#include "stringName.h"
#include "cJSON.h"
#include "utils.h"
#include "gdsl_types.h"
#include "gdsl_list.h"
#include "gdsl_perm.h"
#include "user_app.h"
#include "cfg_parse.h"
//extern void read_pca9554_2(void)；
/*---------------------------------------------------------------------------/
/                               设置充电桩信息到配置文件
/---------------------------------------------------------------------------*/
static ErrorCode_t SetTemplEx(void *pvEVSE, cJSON *jsEVSECfgObj, void *pvCfgParam);
static ErrorCode_t SetEVSECfg(void *pvEVSE, uint8_t *jnItemString, void *pvCfgParam, uint8_t type);
static void TemplSegFree (gdsl_element_t e);
static gdsl_element_t TemplSegAlloc(void *pTemplSeg);


void testSetTemplEx(void)
{
    TemplSeg_t tmpTempl;
    gdsl_list_t plCfgTempl;
    struct tm *ts;
    int i;
    plCfgTempl = gdsl_list_alloc ("tmpTempl", TemplSegAlloc, TemplSegFree);
    for(i = 0; i < 5; i++)
    {
        strcpy(tmpTempl.strStartTime, "12:00");
        strcpy(tmpTempl.strEndTime, "13:00");
        tmpTempl.dSegFee = i + 1;
        gdsl_list_insert_tail(plCfgTempl, &tmpTempl);
    }

    pEVSE->info.SetEVSECfg(pEVSE, jnTemplSegArray, plCfgTempl, ParamTypeList);
    gdsl_list_free(plCfgTempl);
}

/** @todo (rgw#1#): 所有设置参数增加范围校验, 可以在这里进行校验, 也可以在界面输入的时候进行校验.  */

/** @brief EVSE配置函数,
 *
 * @param pvEVSE void*
 * @param jnItemString uint8_t*
 * @param pvCfgParam void* 当时段为0时设置为NULL
 * @param type uint8_t
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetEVSECfg(void *pvEVSE, uint8_t *jnItemString, void *pvCfgParam, uint8_t type)
{
    cJSON *jsEVSECfgObj;
    cJSON *jsItem;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    if(jsEVSECfgObj == NULL)
    {
        return errcode;
    }
    jsItem = jsEVSECfgObj->child;
    do
    {
        if(strcmp(jsItem->string, jnItemString) == 0)
        {
            switch(type)
            {
            case ParamTypeU8:
                cJSON_ReplaceItemInObject(jsEVSECfgObj, jnItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            case ParamTypeU16:
                cJSON_ReplaceItemInObject(jsEVSECfgObj, jnItemString, cJSON_CreateNumber(*((uint16_t *)pvCfgParam)));
                break;
            case ParamTypeU32:
                cJSON_ReplaceItemInObject(jsEVSECfgObj, jnItemString, cJSON_CreateNumber(*((uint32_t *)pvCfgParam)));
                break;
            case ParamTypeDouble:
                cJSON_ReplaceItemInObject(jsEVSECfgObj, jnItemString, cJSON_CreateNumber(*((double *)pvCfgParam)));
                break;
            case ParamTypeString:
                cJSON_ReplaceItemInObject(jsEVSECfgObj, jnItemString, cJSON_CreateString((uint8_t *)pvCfgParam));
                break;
            case ParamTypeList:
                SetTemplEx(pvEVSE, jsEVSECfgObj, pvCfgParam);
                break;
            default:
                break;
            }
            break;//退出while循环
        }
        else
        {
            jsItem = jsItem->next;
        }
    }
    while(jsItem != NULL);
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);

    return errcode;
}
static cJSON *jsTemplSegArrayItemObjCreate(void)
{
    cJSON *jsTemplSegArrayItemObj;

    jsTemplSegArrayItemObj = cJSON_CreateObject();

    return jsTemplSegArrayItemObj;
}
#if 0
static ErrorCode_t SetSN(void *pvEVSE, void *pvCfgParam)
{
    EVSE_t *pEVSE;
    cJSON *jsEVSECfgObj;
    ErrorCode_t errcode;
    uint8_t *ptmpSN;
    uint8_t tmpStrLength;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    ptmpSN = (uint8_t *)pvCfgParam;

    if(jsEVSECfgObj == NULL)
    {
        goto exit;
    }
    /** @todo (rgw#1#): 在这里可以获取原参数与新参数.可以通过send队列方式发送到操作记录 */
    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnEVSESN, cJSON_CreateString(ptmpSN));
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);
    if(errcode != ERR_NO)
    {
        goto exit;
    }
exit:
    return errcode;
}
static ErrorCode_t SetID(void *pvEVSE, void *pvCfgParam)
{
    EVSE_t *pEVSE;
    cJSON *jsEVSECfgObj;
    ErrorCode_t errcode;
    uint8_t *ptmpID;
    uint8_t tmpStrLength;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    ptmpID = (uint8_t *)pvCfgParam;

    if(jsEVSECfgObj == NULL)
    {
        goto exit;
    }
    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnEVSEID, cJSON_CreateString(ptmpID));
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);
    if(errcode != ERR_NO)
    {
        goto exit;
    }
exit:
    return errcode;
}
static ErrorCode_t SetType(void *pvEVSE, void *pvCfgParam)
{
    EVSE_t *pEVSE;
    cJSON *jsEVSECfgObj;
    ErrorCode_t errcode;
    uint8_t tmpType;


    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    tmpType = *((uint8_t *)pvCfgParam);

    if(jsEVSECfgObj == NULL)
    {
        goto exit;
    }
    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnTotalCON, cJSON_CreateNumber(tmpType));
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);
    if(errcode != ERR_NO)
    {
        goto exit;
    }
exit:
    return errcode;
}
static ErrorCode_t SetTotalCON(void *pvEVSE, void *pvCfgParam)
{
    EVSE_t *pEVSE;
    cJSON *jsEVSECfgObj;
    ErrorCode_t errcode;
    uint8_t tmpTotal;


    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    tmpTotal = *((uint8_t *)pvCfgParam);

    if(jsEVSECfgObj == NULL)
    {
        goto exit;
    }
    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnTotalCON, cJSON_CreateNumber(tmpTotal));
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);
    if(errcode != ERR_NO)
    {
        goto exit;
    }
exit:
    return errcode;
}
static ErrorCode_t SetLngLat(void *pvEVSE, void *pvCfgParam)
{
    EVSE_t *pEVSE;
    cJSON *jsEVSECfgObj;
    ErrorCode_t errcode;
    uint8_t tmpTotal;


    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    tmpTotal = *((uint8_t *)pvCfgParam);

    if(jsEVSECfgObj == NULL)
    {
        goto exit;
    }
    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnTotalCON, cJSON_CreateNumber(tmpTotal));
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);
    if(errcode != ERR_NO)
    {
        goto exit;
    }
exit:
    return errcode;
}

static ErrorCode_t SetTempl(void *pvEVSE, void *pvCfgParam)
{
    EVSE_t *pEVSE;
    cJSON *jsEVSECfgObj;
    cJSON *jsTemplSegArray;
    cJSON *jsTemplSegArrayItemObj;
    ErrorCode_t errcode;
    gdsl_list_t plCfgTempl;
    TemplSeg_t *ptCfgTempl;
    uint8_t ucTemplNum;
    TemplSeg_t tmpTemplSeg;
    int i;

    pEVSE = (EVSE_t *)pvEVSE;
    plCfgTempl = (gdsl_list_t)pvCfgParam;
    errcode = ERR_NO;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);


    if(jsEVSECfgObj == NULL)
    {
        return errcode;
    }
    jsTemplSegArray = cJSON_CreateArray();
    ucTemplNum = gdsl_list_get_size(plCfgTempl);
    for(i = 1; i <= ucTemplNum; i++)
    {
        ptCfgTempl = (TemplSeg_t *)gdsl_list_search_by_position(plCfgTempl, i);
        jsTemplSegArrayItemObj = jsTemplSegArrayItemObjCreate();

        cJSON_AddItemToObject(jsTemplSegArrayItemObj, jnStartTime, cJSON_CreateNumber(ptCfgTempl->tStartTime));
        cJSON_AddItemToObject(jsTemplSegArrayItemObj, jnEndTime, cJSON_CreateNumber(ptCfgTempl->tEndTime));
        cJSON_AddItemToObject(jsTemplSegArrayItemObj, jnSegFee, cJSON_CreateNumber(ptCfgTempl->dSegFee));

        cJSON_AddItemToArray(jsTemplSegArray, jsTemplSegArrayItemObj);
    }

    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnTemplSegArray, jsTemplSegArray);
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj);
    if(errcode != ERR_NO)
    {
//        此处注释只为说明该条件下进行的操作
//        cJSON_Delete(jsTemplSegArray);
//        return errcode;
    }
    cJSON_Delete(jsTemplSegArray);
    return errcode;
}
#endif
/** @brief
 *
 * @param pvEVSE void*
 * @param jsEVSECfgObj cJSON*
 * @param pvCfgParam void*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetTemplEx(void *pvEVSE, cJSON *jsEVSECfgObj, void *pvCfgParam)
{
    EVSE_t *pEVSE;

    cJSON *jsTemplSegArray;
    cJSON *jsTemplSegArrayItemObj;
    ErrorCode_t errcode;
    gdsl_list_t plCfgTempl;
    TemplSeg_t *ptCfgTempl;
    uint8_t ucTemplNum;
    TemplSeg_t tmpTemplSeg;
    int i;

    pEVSE = (EVSE_t *)pvEVSE;
    plCfgTempl = (gdsl_list_t)pvCfgParam;
    errcode = ERR_NO;

    jsTemplSegArray = cJSON_CreateArray();
    if(plCfgTempl != NULL)
    {
        ucTemplNum = gdsl_list_get_size(plCfgTempl);
        if(ucTemplNum > 0)
        {
            for(i = 1; i <= ucTemplNum; i++)
            {
                ptCfgTempl = (TemplSeg_t *)gdsl_list_search_by_position(plCfgTempl, i);
                jsTemplSegArrayItemObj = jsTemplSegArrayItemObjCreate();
                cJSON_AddItemToObject(jsTemplSegArrayItemObj, jnStartTime, cJSON_CreateString(ptCfgTempl->strStartTime));
                cJSON_AddItemToObject(jsTemplSegArrayItemObj, jnEndTime, cJSON_CreateString(ptCfgTempl->strEndTime));
                cJSON_AddItemToObject(jsTemplSegArrayItemObj, jnSegFee, cJSON_CreateNumber(ptCfgTempl->dSegFee));

                cJSON_AddItemToArray(jsTemplSegArray, jsTemplSegArrayItemObj);
            }
        }
    }

    cJSON_ReplaceItemInObject(jsEVSECfgObj, jnTemplSegArray, jsTemplSegArray);

    return errcode;
}
/*---------------------------------------------------------------------------/
/                               从文件获取充电桩信息
/---------------------------------------------------------------------------*/
/** @brief 设备唯一序列号,和长度
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetSN(void *pvEVSE, void *pvCfgObj)
{
    uint8_t tmpSN[defEVSESNLength];
    uint8_t tmpStrLength;
    uint8_t tmpLength;
    uint8_t *ptmpSN;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpSN, 0, defEVSESNLength);
    tmpStrLength = 0;
    tmpLength = 0;

    pEVSECfgObj = (cJSON *)pvCfgObj;

    /** (rgw#1#): 从文件获取SN 并获取SN长度*/

    //- 解析EVSESN
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnEVSESN);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    ptmpSN = jsItem->valuestring;
    tmpStrLength = strlen(ptmpSN);
    StrToHex(ptmpSN, tmpSN, tmpStrLength);
    tmpLength = tmpStrLength >> 1;//BCD字符串换十六进制后,占用字节数减半
#ifdef DEBUG_CFG_PARSE
    int i;
    printf_safe("EVSE SN =");
    for(i = 0; i < 8; i++)
    {
        printf_safe(" %02d ", tmpSN[i]);
    }
    printf_safe("\n");
#endif

    /*********************/
    if(tmpLength <= defEVSESNLength && tmpLength > 0)
    {
        pEVSE->info.ucSNLength = tmpLength;
        memmove(pEVSE->info.ucSN, tmpSN, tmpLength);
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return errcode;
}

/** @brief 平台分配ID
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetID(void *pvEVSE, void *pvCfgObj)
{
    uint8_t tmpID[defEVSEIDLength];
    uint8_t tmpStrLength;
    uint8_t tmpLength;
    uint8_t *ptmpID;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpID, 0, defEVSEIDLength);
    tmpStrLength = 0;
    tmpLength = 0;


    pEVSECfgObj = (cJSON *)pvCfgObj;
    /**  (rgw#1#): 从文件获取ID 并获取ID长度*/

    //- 解析EVSEID
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnEVSEID);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    ptmpID = jsItem->valuestring;
    tmpStrLength = strlen(ptmpID);
    StrToHex(ptmpID, tmpID, tmpStrLength);
    tmpLength = tmpStrLength >> 1;

#ifdef DEBUG_CFG_PARSE
    int i;
    printf_safe("EVSE ID =");
    for(i = 0; i < 8; i++)
    {
        printf_safe(" %02d ", tmpID[i]);
    }
    printf_safe("\n");
#endif

    /*********************/
    if(tmpLength <= defEVSEIDLength && tmpLength > 0)
    {
        pEVSE->info.ucIDLength = tmpLength;
        memmove(pEVSE->info.ucID, tmpID, tmpLength);
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return errcode;
}

/** @brief 充电设备类型
 *         1：直流设备
 *         2：交流设备
 *         3：交直流一体设备
 *         4：无线设备
 *         5：其他
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetType(void *pvEVSE, void *pvCfgObj)
{
    uint8_t tmpType;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpType = 0;
    errcode = ERR_NO;

    pEVSECfgObj = (cJSON *)pvCfgObj;
    /**  (rgw#1#): 从文件获取 */

    //- 解析EVSEType
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnEVSEType);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpType = jsItem->valueint;
#ifdef DEBUG_CFG_PARSE
    printf_safe("EVSE Type = %d\n", tmpType);
#endif

    /*********************/
    if(tmpType >= 1 && tmpType <= 5)
    {
        pEVSE->info.ucType = tmpType;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return errcode;
}

/** @brief 枪的数量
 *
 * @param pvEVSE void*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetTotalCON(void *pvEVSE, void *pvCfgObj)
{
    uint8_t tmpTotal;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpTotal = 0;//枪的数量，2代表两把枪
    errcode = ERR_NO;

    pEVSECfgObj = (cJSON *)pvCfgObj;
    /** (rgw#1#): 从文件获取 */

    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnTotalCON);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpTotal = jsItem->valueint;
#ifdef DEBUG_CFG_PARSE
    printf_safe("EVSE TotalCON = %d\n", tmpTotal);
#endif
    /*********************/
    if(tmpTotal > 0)
    {
        pEVSE->info.ucTotalCON = tmpTotal;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return errcode;
}

/** @brief 经纬度，保留后六位
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *                     ERR_NO
 *                     ERR_FILE_RW
 *                     ERR_FILE_NO
 *
 */
static ErrorCode_t GetLngLat(void *pvEVSE, void *pvCfgObj)
{
    double tmpLng, tmpLat;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpLng = 0;
    tmpLat = 0;
    errcode = ERR_NO;

    pEVSECfgObj = (cJSON *)pvCfgObj;
    /**  (rgw#1#): 从文件获取 */

    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnLng);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpLng = jsItem->valuedouble;

    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnLat);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpLat = jsItem->valuedouble;
#ifdef DEBUG_CFG_PARSE
    printf_safe("EVSE Lng,Lat = (%.6lf , %.6lf)\n", tmpLng, tmpLat);
#endif

    /*********************/
    if((tmpLng >= 0 && tmpLng <= 180) && (tmpLat >= 0 && tmpLat <= 180))
    {
        pEVSE->info.dLng = tmpLng;
        pEVSE->info.dLat = tmpLat;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return errcode;
}
static gdsl_element_t TemplSegAlloc(void *pTemplSeg)
{
    gdsl_element_t copyTempl;
    copyTempl = (gdsl_element_t)malloc(sizeof(TemplSeg_t));
    if(copyTempl != NULL)
    {
        memcpy(copyTempl, pTemplSeg, sizeof(TemplSeg_t));
    }
    return copyTempl;
}
static void TemplSegFree (gdsl_element_t e)
{
    free (e);
}
static time_t SegTimeFormat(uint8_t *timestr, uint32_t ulStrlen)
{
    time_t now;
    struct tm *ts;
    uint8_t tbuff[2];
    uint8_t *tmptimestr;

    now = time(NULL);
    ts = localtime(&now);
    tmptimestr = timestr;
    strncpy(tbuff, tmptimestr, 2);
    ts->tm_hour = strtol(tbuff, NULL, 10);
    tmptimestr += 3;
    strncpy(tbuff, tmptimestr, 2);
    ts->tm_min = strtol(tbuff, NULL, 10);
    ts->tm_sec = 0;
    now = mktime(ts);
    return now;
}
static void TemplSegDup(gdsl_list_t dst, gdsl_list_t src)
{
    uint8_t ucSrcListSize;
    int i;
    ucSrcListSize = gdsl_list_get_size(src);
    if(dst != NULL)
    {
        gdsl_list_flush(dst);
    }
    for(i = 1; i <= ucSrcListSize; i++)
    {
        gdsl_list_insert_tail(dst, gdsl_list_search_by_position(src, i));
    }
}
static ErrorCode_t GetTempl(void *pvEVSE, void *pvCfgObj)
{
    uint8_t tmpServiceType;
    double tmpServiceFee;
    double tmpDefSegFee;
    uint32_t tmpTotalSegs;
    gdsl_list_t pTemplSegList;
    TemplSeg_t tmpTemplSeg;

    ErrorCode_t errcode;
    EVSE_t *pEVSE;
    int i;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;
    cJSON *jsArrayItem;
    cJSON *jsArrayObjItem;

    pEVSE = (EVSE_t *)pvEVSE;
    tmpServiceType = 0;
    tmpServiceFee = 0;
    tmpDefSegFee = 0;
    tmpTotalSegs = 0;
    errcode = ERR_NO;

    pEVSECfgObj = (cJSON *)pvCfgObj;
    pTemplSegList = gdsl_list_alloc ("tmpTempl", TemplSegAlloc, TemplSegFree);
    /**  (rgw#1#): 从文件获取 */
    //获取服务费类型
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnServiceFeeType);
    if(jsItem == NULL)
    {
        gdsl_list_free(pTemplSegList);
        return ERR_FILE_PARSE;
    }
    tmpServiceType = jsItem->valueint;
    //获取服务费
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnServiceFee);
    if(jsItem == NULL)
    {
        gdsl_list_free(pTemplSegList);
        return ERR_FILE_PARSE;
    }
    tmpServiceFee = jsItem->valuedouble;
    //获取默认段费率
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnDefSegFee);
    if(jsItem == NULL)
    {
        gdsl_list_free(pTemplSegList);
        return ERR_FILE_PARSE;
    }
    tmpDefSegFee = jsItem->valuedouble;
    //获取段费率
    jsItem = pEVSECfgObj->child;
    do
    {
        if(strcmp(jsItem->string, jnTemplSegArray) == 0)
        {
            break;
        }
        else
        {
            jsItem = jsItem->next;
        }
    }
    while(jsItem != NULL);
    if(jsItem == NULL)
    {
        gdsl_list_free(pTemplSegList);
        return ERR_FILE_PARSE;
    }
    tmpTotalSegs = cJSON_GetArraySize(jsItem);
#ifdef DEBUG_CFG_PARSE
    printf_safe("ulTotalSegs = %d\n", tmpTotalSegs);
#endif
    if(tmpTotalSegs > 0)
    {
        for(i = 0; i < tmpTotalSegs; i++)
        {
#ifdef DEBUG_CFG_PARSE
            printf_safe("Seg %d ", i);
#endif
            jsArrayItem = cJSON_GetArrayItem(jsItem, i);
            jsArrayObjItem = cJSON_GetObjectItem(jsArrayItem, jnStartTime);
#ifdef DEBUG_CFG_PARSE
            printf_safe("StartTime: %s | ", jsArrayObjItem->valuestring);
#endif
            tmpTemplSeg.tStartTime = SegTimeFormat(jsArrayObjItem->valuestring,
                                                   strlen(jsArrayObjItem->valuestring));

            jsArrayObjItem = cJSON_GetObjectItem(jsArrayItem, jnEndTime);
#ifdef DEBUG_CFG_PARSE
            printf_safe("EndTime: %s | ", jsArrayObjItem->valuestring);
#endif
            tmpTemplSeg.tEndTime = SegTimeFormat(jsArrayObjItem->valuestring,
                                                 strlen(jsArrayObjItem->valuestring));;
            jsArrayObjItem = cJSON_GetObjectItem(jsArrayItem, jnSegFee);
#ifdef DEBUG_CFG_PARSE
            printf_safe("SegFee: %.2lf\n", jsArrayObjItem->valuedouble);
#endif
            tmpTemplSeg.dSegFee = jsArrayObjItem->valuedouble;

            gdsl_list_insert_tail(pTemplSegList, (void *)&tmpTemplSeg);
        }
#ifdef DEBUG_CFG_PARSE
        uint8_t listsize_dbg = gdsl_list_get_size(pTemplSegList);
        printf_safe("List Num = %d\n", listsize_dbg);
        struct tm *ts_dbg;
        TemplSeg_t *tmlseg_dgb;

        for(i = 1; i <= listsize_dbg; i++)
        {
            tmlseg_dgb = (TemplSeg_t *)(gdsl_list_search_by_position(pTemplSegList, i));
            ts_dbg = localtime(&(tmlseg_dgb->tStartTime));
            printf_safe("List seg %d  StartTime:%02d:%02d | ",
                        i , ts_dbg->tm_hour, ts_dbg->tm_min  );
            ts_dbg = localtime(&(tmlseg_dgb->tEndTime));
            printf_safe("EndTime:%02d:%02d | ",
                        ts_dbg->tm_hour, ts_dbg->tm_min  );
            printf_safe("SegFee:%.2lf\n",
                        tmlseg_dgb->dSegFee );
        }
#endif
#ifdef _DEBUG_CFG_PARSE
        printf_safe("****Parse EVSE list****\n");
        listsize_dbg = gdsl_list_get_size((gdsl_element_t)(pEVSE->info.plTemplSeg));
        printf_safe("EVSE List Num = %d\n", listsize_dbg);
        for(i = 1; i <= listsize_dbg; i++)
        {
            tmlseg_dgb = (TemplSeg_t *)(gdsl_list_search_by_position((gdsl_element_t)(pEVSE->info.plTemplSeg), i));
            ts_dbg = localtime(&(tmlseg_dgb->tStartTime));
            printf_safe("List seg %d  StartTime:%02d:%02d | ",
                        i , ts_dbg->tm_hour, ts_dbg->tm_min  );
            ts_dbg = localtime(&(tmlseg_dgb->tEndTime));
            printf_safe("EndTime:%02d:%02d | ",
                        ts_dbg->tm_hour, ts_dbg->tm_min  );
            printf_safe("SegFee:%.2lf\n",
                        tmlseg_dgb->dSegFee );
        }
#endif
    }
    if(jsItem == NULL)
    {
        gdsl_list_free(pTemplSegList);
        return ERR_FILE_PARSE;
    }

#ifdef DEBUG_CFG_PARSE
    printf_safe("EVSE ServiceType = %d ,Fee = %.2lf\n", tmpServiceType, tmpServiceFee);
    printf_safe("EVSE DefSegFee = %.2lf\n", tmpDefSegFee);
#endif

    /*********************/
    if(tmpServiceType <= 1 && tmpServiceFee >= 0 && tmpDefSegFee >= 0)
    {
        pEVSE->info.ucServiceFeeType = tmpServiceType;
        pEVSE->info.dServiceFee = tmpServiceFee;
        pEVSE->info.dDefSegFee = tmpDefSegFee;
        if(tmpTotalSegs > 0)
        {
            TemplSegDup(pEVSE->info.plTemplSeg, pTemplSegList);
        }

    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    gdsl_list_free(pTemplSegList);
    return errcode;
}

static ErrorCode_t GetEVSECfg(void *pvEVSE, void *pvCfgObj)
{
    cJSON *jsEVSEObj;
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /*json解析*/
    jsEVSEObj = GetCfgObj(pathEVSECfg, &errcode);
    if(jsEVSEObj == NULL || errcode != ERR_NO)
    {
        return errcode;
    }
    THROW_ERROR(defDevID_File, errcode = GetSN(pvEVSE, jsEVSEObj), ERR_LEVEL_WARNING, "GetSN()");
    THROW_ERROR(defDevID_File, errcode = GetID(pvEVSE, jsEVSEObj), ERR_LEVEL_WARNING, "GetID()");
    THROW_ERROR(defDevID_File, errcode = GetType(pvEVSE, jsEVSEObj), ERR_LEVEL_WARNING, "GetType()");
    THROW_ERROR(defDevID_File, errcode = GetTotalCON(pvEVSE, jsEVSEObj), ERR_LEVEL_WARNING, "GetTotalCON()");
    THROW_ERROR(defDevID_File, errcode = GetLngLat(pvEVSE, jsEVSEObj), ERR_LEVEL_WARNING, "GetLngLat()");
    THROW_ERROR(defDevID_File, errcode = GetTempl(pvEVSE, jsEVSEObj), ERR_LEVEL_WARNING, "GetTempl");
#ifdef DEBUG_CFG_PARSE
    printf_safe("********************************\n");
#endif
    cJSON_Delete(jsEVSEObj);
    return errcode;
}

/*---------------------------------------------------------------------------/
/                               从驱动获取充电桩状态
/---------------------------------------------------------------------------*/


/** @brief 获得急停状态
 *          0 无急停
 *          1 有急停
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_SCRAM_FAULT
 *
 */
static ErrorCode_t GetScramState(void *pvEVSE)
{
    uint32_t tmpScramState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    //errcode = ERR_SCRAM_FAULT;
    errcode = ERR_NO;
    tmpScramState = 0;
    /* @todo (yuye#1#): 确认取反 */

#ifdef DEBUG_DIAG_DUMMY
    tmpScramState = 0;
#else
    tmpScramState = ~((uint8_t)(read_pca9554_2() >> 5)) & 0x01;
#endif
    /*********************/

    pEVSE->status.ulScramState = tmpScramState;

    return errcode;
}

/** @brief 从传感器获取撞机状态
 *          0 无撞机
 *          1 有撞击
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_GSENSOR_FAULT
 *
 */
static ErrorCode_t GetKnockState(void *pvEVSE)
{
    uint32_t tmpKnockState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    tmpKnockState = 0;
    /* @todo (yuye#1#): 添加重力传感器驱动 */
#ifdef DEBUG_DIAG_DUMMY
    tmpKnockState = 0;
#else
    //在这添加代码
#endif

    /*********************/

    pEVSE->status.ulKnockState = tmpKnockState;

    return  errcode;
}

/** @brief 获取保护接地连续性状态
 *          0 正常
 *          1 异常
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_PE_FAULT
 *
 */
static ErrorCode_t GetPEState(void *pvEVSE)
{
    uint32_t tmpPEState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    tmpPEState = 0;

    /* @todo (yuye#1#): 增加硬件功能，检测PE和相序。 */
#ifdef DEBUG_DIAG_DUMMY
    tmpPEState = 0;
#else
    //在这添加代码
#endif
    /*********************/

    pEVSE->status.ulPEState = tmpPEState;

    return errcode;
}

/** @brief 获取掉电状态
 *          0 正常
 *          1 掉电
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_POWEROFF_DECT_FAULT
 *
 */
static ErrorCode_t GetPowerOffState(void *pvEVSE)
{
    uint32_t tmpOffState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    tmpOffState = 0;

    /* @todo (yuye#1#): 确认电压范围 */
    /**  (rgw#1#): 实现代码 */

#ifdef DEBUG_DIAG_DUMMY
    tmpOffState = 0;
#else
    if(get_va() <= 100.0) //检测间隔10mS
    {
        tmpOffState = 1;
    }
    else if((get_va() >= 180) && (get_va() <= 250))
    {
        tmpOffState = 0;
    }
#endif
    /*********************/

    pEVSE->status.ulPowerOffState = tmpOffState;

    return errcode;
}

/** @brief 获取避雷器状态
 *          0 正常
 *          1 异常
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *             ERR_NO
 *             ERR_ARRESTER_FAULT
 *
 */
static ErrorCode_t GetArresterState(void *pvEVSE)
{
    uint32_t tmpArresterState;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    tmpArresterState = 0;

    /**  (rgw#1#): 实现代码 */

#ifdef DEBUG_DIAG_DUMMY
    tmpArresterState = 0;
#else
    tmpArresterState = (~(read_pca9554_2() >> 4)) & 0x01;
#endif

    /*********************/

    pEVSE->status.ulArresterState = tmpArresterState;
    return errcode;
}

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));

    memset(pEVSE->info.ucSN, 0, defEVSESNLength);
    memset(pEVSE->info.ucID, 0, defEVSEIDLength);
    pEVSE->info.ucType = defEVSEType_AC;
    pEVSE->info.ucTotalCON = 2;
    pEVSE->info.dLng = 116.275833;
    pEVSE->info.dLat = 39.831944;
    pEVSE->info.ucServiceFeeType = 0;
    pEVSE->info.dServiceFee = 0;
    pEVSE->info.dDefSegFee = 0;

    pEVSE->info.GetEVSECfg = GetEVSECfg;
    /** @todo (rgw#1#): 以下修改为Set参数 */
    pEVSE->info.SetEVSECfg = SetEVSECfg;
//    pEVSE->info.SetSN = SetSN;
//    pEVSE->info.SetID = SetID;
//    pEVSE->info.SetType = SetType;
//    pEVSE->info.SetTotalCON = SetTotalCON;
//    pEVSE->info.SetLngLat = SetLngLat;
//    pEVSE->info.SetTempl = SetTempl;


    pEVSE->info.plTemplSeg = gdsl_list_alloc("Templ", TemplSegAlloc, TemplSegFree);
    if(pEVSE->info.plTemplSeg == NULL)
    {
        return NULL;
    }
    //pEVSE->info.pTemplSeg = UserListCreate();

    pEVSE->status.ulArresterState = 0;
    pEVSE->status.ulKnockState = 0;
    pEVSE->status.ulPEState = 0;
    pEVSE->status.ulPowerOffState = 0;
    pEVSE->status.ulScramState = 0;

    pEVSE->status.GetArresterState = GetArresterState;
    pEVSE->status.GetKnockState = GetKnockState;
    pEVSE->status.GetPEState = GetPEState;
    pEVSE->status.GetPowerOffState = GetPowerOffState;
    pEVSE->status.GetScramState = GetScramState;

    return pEVSE;
}

static void CONInit(void)
{
    static CON_t *pCON[2];  //在堆中定义

    pListCON = UserListCreate();
    int i;
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON[i] = CONCreate(i);

        THROW_ERROR(i, pCON[i]->info.GetCONCfg(pCON[i], NULL), ERR_LEVEL_WARNING, "CONInit GetCONCfg");

        pListCON->Add(pListCON, pCON[i]);
    }
}
void EVSEinit(void)
{
    pEVSE = EVSECreate();
    THROW_ERROR(defDevID_File, pEVSE->info.GetEVSECfg(pEVSE, NULL), ERR_LEVEL_WARNING, "EVSEinit GetEVSECfg");
    CONInit();

    pRFIDDev = RFIDDevCreate();
}
