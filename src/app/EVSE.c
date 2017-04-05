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
//#include "bsp.h"
extern samp Sys_samp;
extern IO_chip2 Chip2;
//extern void read_pca9554_2(void)；
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
    uint8_t tmpLength;
    uint8_t *ptmpSN;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpSN, 0, defEVSESNLength);
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
    tmpLength = strlen(ptmpSN);
    StrToHex(ptmpSN, tmpSN, tmpLength);
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
    uint8_t tmpLength;
    uint8_t *ptmpID;
    ErrorCode_t errcode;
    EVSE_t *pEVSE;

    cJSON *jsItem;
    cJSON *pEVSECfgObj;

    pEVSE = (EVSE_t *)pvEVSE;
    errcode = ERR_NO;
    memset(tmpID, 0, defEVSEIDLength);
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
    tmpLength = strlen(ptmpID);
    StrToHex(ptmpID, tmpID, tmpLength);
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
        pEVSE->info.ucIDLenght = tmpLength;
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
    /** @todo (rgw#1#): 从文件获取 */

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
    if((tmpLng >= 0 && tmpLng <=180) && (tmpLat >= 0 && tmpLat <= 180))
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
static TemplSeg_t *TemplSegCreate(void)
{
    TemplSeg_t *ptemplseg;
    ptemplseg = (TemplSeg_t *)malloc(sizeof(TemplSeg_t));
    return ptemplseg;
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
    ts->tm_hour = strtol(&tbuff, NULL, 10);
    tmptimestr += 3;
    strncpy(tbuff, tmptimestr, 2);
    ts->tm_min = strtol(tbuff, NULL, 10);
    ts->tm_sec = 0;
    now = mktime(ts);
    return now;
}
static ErrorCode_t GetTempl(void *pvEVSE, void *pvCfgObj)
{
    uint8_t tmpServiceType;
    double tmpServiceFee;
    double tmpDefSegFee;
    uint32_t tmpTotalSegs;
    UserList_t *pTemplSegList;
    TemplSeg_t *pTemplSeg;

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
    pTemplSeg = NULL;
    errcode = ERR_NO;

    pEVSECfgObj = (cJSON *)pvCfgObj;
    pTemplSegList = UserListCreate();
    /**  (rgw#1#): 从文件获取 */
    //获取服务费类型
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnServiceFeeType);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpServiceType = jsItem->valueint;
    //获取服务费
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnServiceFee);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpServiceFee = jsItem->valuedouble;
    //获取默认段费率
    jsItem = cJSON_GetObjectItem(pEVSECfgObj, jnDefSegFee);
    if(jsItem == NULL)
    {
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
        return ERR_FILE_PARSE;
    }
    tmpTotalSegs = cJSON_GetArraySize(jsItem);
#ifdef DEBUG_CFG_PARSE
    printf_safe("ulTotalSegs = %d\n", tmpTotalSegs);
#endif
    tmpTotalSegs = 0;
    if(tmpTotalSegs > 0)
    {
        for(i = 0; i < tmpTotalSegs; i++)
        {
            pTemplSeg = TemplSegCreate();
#ifdef DEBUG_CFG_PARSE
            printf_safe("Seg %d ", i);
#endif
            jsArrayItem = cJSON_GetArrayItem(jsItem, i);
            jsArrayObjItem = cJSON_GetObjectItem(jsArrayItem, jnStartTime);
#ifdef DEBUG_CFG_PARSE
            printf_safe("StartTime: %s | ", jsArrayObjItem->valuestring);
#endif
            pTemplSeg->tStartTime = SegTimeFormat(jsArrayObjItem->valuestring,
                                                  strlen(jsArrayObjItem->valuestring));

            jsArrayObjItem = cJSON_GetObjectItem(jsArrayItem, jnEndTime);
#ifdef DEBUG_CFG_PARSE
            printf_safe("EndTime: %s | ", jsArrayObjItem->valuestring);
#endif
            pTemplSeg->tEndTime = SegTimeFormat(jsArrayObjItem->valuestring,
                                                strlen(jsArrayObjItem->valuestring));;
            jsArrayObjItem = cJSON_GetObjectItem(jsArrayItem, jnSegFee);
#ifdef DEBUG_CFG_PARSE
            printf_safe("SegFee: %.2lf\n", jsArrayObjItem->valuedouble);
#endif
            pTemplSeg->dSegFee = jsArrayObjItem->valuedouble;

            pTemplSegList->Add(pTemplSegList, pTemplSeg);
        }
#ifdef DEBUG_CFG_PARSE
        printf_safe("List Num = %d\n", pTemplSegList->Total);
        struct tm *ts_dbg;
        TemplSeg_t *tmlseg_dgb;
        for(i = 0; i < pTemplSegList ->Total; i++)
        {
            tmlseg_dgb = (TemplSeg_t *)(pTemplSegList->pListPointArray[i]);
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
        //pEVSE->info.pTemplSeg = pTemplSegList;
    }
    pTemplSegList->Delete(pTemplSegList);

    if(jsItem == NULL)
    {
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
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return errcode;
}

static ErrorCode_t GetEVSECfg(void *pvEVSE, void *pvCfgObj)
{
    /** @todo (rgw#1#): 增加读取参数校验 */

    FIL f;
    FRESULT res;
    uint8_t *rbuff;
    FSIZE_t fsize;
    UINT  br;   //byte read

    cJSON *jsEVSEObj;
    int i;
    ErrorCode_t errcode;

    /*读取文件*/
    ThrowFSCode(res = f_open(&f, pathEVSECfg, FA_READ));

    if(res != FR_OK)
    {
        errcode = ERR_FILE_RW;
        return errcode;
    }
    fsize = f_size(&f);
    rbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    ThrowFSCode(res = f_read(&f, rbuff, fsize, &br));
    if(fsize != br)
    {
        errcode = ERR_FILE_RW;
        return errcode;
    }

    /*json解析*/
    jsEVSEObj = cJSON_Parse(rbuff);

    THROW_ERROR(defDevID_File, GetSN(pEVSE, jsEVSEObj), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, GetID(pEVSE, jsEVSEObj), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, GetType(pEVSE, jsEVSEObj), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, GetTotalCON(pEVSE, jsEVSEObj), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, GetLngLat(pEVSE, jsEVSEObj), ERR_LEVEL_WARNING);
    THROW_ERROR(defDevID_File, GetTempl(pEVSE, jsEVSEObj), ERR_LEVEL_WARNING);

    cJSON_Delete(jsEVSEObj);
    free(rbuff);
    f_close(&f);
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

    /** @todo (rgw#1#): 实现代码 */

    //...

    tmpScramState = (uint32_t)(read_pca9554_2()>>5)&&0x01;
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

    /** @todo (rgw#1#): 获取重力加速度传感器 */

    //...

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

    /** @todo (rgw#1#): 实现代码 */

    //...
    tmpPEState=GET_CC1;
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

    /** @todo (rgw#1#): 实现代码 */

    //...
    if(get_va()<=180.0)//检测间隔10mS
    {
        tmpOffState=1;
    }
    else
    {
     tmpOffState=0;
    }

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

    /** @todo (rgw#1#): 实现代码 */

    //...

    tmpArresterState= (read_pca9554_2()>>4)&&0x01;

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
    pEVSE->info.ucType = 2;
    pEVSE->info.ucTotalCON = 2;
    pEVSE->info.dLng = 116.275833;
    pEVSE->info.dLat = 39.831944;
    pEVSE->info.ucServiceFeeType = 0;
    pEVSE->info.dServiceFee = 0;
    pEVSE->info.dDefSegFee = 0;

    pEVSE->info.GetEVSECfg = GetEVSECfg;
    pEVSE->info.GetSN = GetSN;
    pEVSE->info.GetID = GetID;
    pEVSE->info.GetType = GetType;
    pEVSE->info.GetTotalCON = GetTotalCON;
    pEVSE->info.GetLngLat = GetLngLat;
    pEVSE->info.GetTempl = GetTempl;

    pEVSE->info.pTemplSeg = NULL;
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

        THROW_ERROR(i, pCON[i]->info.GetSocketType(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetVolatageUpperLimits(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetVolatageLowerLimits(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetRatedCurrent(pCON[i]), ERR_LEVEL_WARNING);
        THROW_ERROR(i, pCON[i]->info.GetRatedPower(pCON[i]), ERR_LEVEL_WARNING);

        pListCON->Add(pListCON, pCON[i]);
    }
}
void EVSEinit(void)
{
    pEVSE = EVSECreate();
    THROW_ERROR(defDevID_File, pEVSE->info.GetEVSECfg(pEVSE, NULL), ERR_LEVEL_WARNING);
//    THROW_ERROR(defDevID_File, pEVSE->info.GetSN(pEVSE), ERR_LEVEL_WARNING);
//    THROW_ERROR(defDevID_File, pEVSE->info.GetID(pEVSE), ERR_LEVEL_WARNING);
//    THROW_ERROR(defDevID_File, pEVSE->info.GetType(pEVSE), ERR_LEVEL_WARNING);
//    THROW_ERROR(defDevID_File, pEVSE->info.GetTotalCON(pEVSE), ERR_LEVEL_WARNING);
//    THROW_ERROR(defDevID_File, pEVSE->info.GetLngLat(pEVSE), ERR_LEVEL_WARNING);

    CONInit();

    pRFIDDev = RFIDDevCreate();
}
