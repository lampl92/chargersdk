/**
* @file connector.c
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
#include <string.h>
#include "evse_globals.h"
#include "connector.h"
#include "bsp.h"
#include "user_app.h"
#include "cJSON.h"
#include "stringName.h"
#include "cfg_parse.h"
/*---------------------------------------------------------------------------/
/                               设置充电接口信息到配置文件
/---------------------------------------------------------------------------*/

ErrorCode_t SetCONCfg(void *pvCON, uint8_t *jnItemString, void *pvCfgParam, uint8_t type)
{
    cJSON *jsEVSECfgObj;
    cJSON *jsCONArray;
    cJSON *jsCONObj;
    cJSON *jsItem;
    ErrorCode_t errcode;
    CON_t *pCON;
    uint8_t ucCONID;


    errcode = ERR_NO;
    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    jsEVSECfgObj = GetCfgObj(pathEVSECfg, &errcode);
    if(jsEVSECfgObj == NULL)
    {
        return errcode;
    }
    jsCONArray = cJSON_GetObjectItem(jsEVSECfgObj, jnCONArray);
    jsCONObj = cJSON_GetArrayItem(jsCONArray, ucCONID);
    jsItem = jsCONObj->child;
    do
    {
        if(strcmp(jsItem->string, jnItemString) == 0)
        {
            switch(type)
            {
            case ParamTypeU8:
                cJSON_ReplaceItemInObject(jsCONObj, jnItemString, cJSON_CreateNumber(*((uint8_t *)pvCfgParam)));
                break;
            case ParamTypeU16:
                cJSON_ReplaceItemInObject(jsCONObj, jnItemString, cJSON_CreateNumber(*((uint16_t *)pvCfgParam)));
                break;
            case ParamTypeU32:
                cJSON_ReplaceItemInObject(jsCONObj, jnItemString, cJSON_CreateNumber(*((uint32_t *)pvCfgParam)));
                break;
            case ParamTypeDouble:
                cJSON_ReplaceItemInObject(jsCONObj, jnItemString, cJSON_CreateNumber(*((double *)pvCfgParam)));
                break;
            case ParamTypeString:
                cJSON_ReplaceItemInObject(jsCONObj, jnItemString, cJSON_CreateString((uint8_t *)pvCfgParam));
                break;
            case ParamTypeList:
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
ErrorCode_t SetCONType(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetSocketType(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetVolatageUpperLimits(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetVolatageLowerLimits(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetACTempUpperLimits(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetACTempLowerLimits(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetSocketTempUpperLimits(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetSocketTempLowerLimits(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetRatedCurrent(void *pvCON, void *pvCfgParam)
{

}
ErrorCode_t SetRatedPower(void *pvCON, void *pvCfgParam)
{

}
/*---------------------------------------------------------------------------/
/                               从文件获取充电接口信息
/---------------------------------------------------------------------------*/

/** @todo (rgw#1#): 增加枪充电类型CONType */

static ErrorCode_t GetCONType(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    uint8_t tmpType;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpType = defCONType_AC;
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** 从文件获取 */
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnType);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpType = jsItem->valueint;

    /*********************/
    if(tmpType > 0 && tmpType < 6)
    {
        pCON->info.ucCONType = tmpType;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }
    return  errcode;
}

static ErrorCode_t GetSocketType(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    uint8_t tmpType;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpType = defSocketTypeB;
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;
    /** @todo (rgw#1#): 从文件获取 */
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnSocketType);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpType = jsItem->valueint;

    /*********************/
    if(tmpType == defSocketTypeB || tmpType == defSocketTypeC)
    {
        pCON->info.ucSocketType = tmpType;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }

    return  errcode;
}
static ErrorCode_t GetVolatageLimits(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpVoltUpLim;
    double tmpVoltLowLim;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpVoltUpLim = 250;//(V)
    tmpVoltLowLim = 190;//(V)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;
    /** @todo (rgw#1#): 从文件获取 */
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnVolatageUpperLimits);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpVoltUpLim = jsItem->valuedouble;
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnVolatageLowerLimits);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpVoltLowLim = jsItem->valuedouble;

    /*********************/
    if(tmpVoltLowLim > 0 && tmpVoltUpLim > 0)
    {
        pCON->info.dVolatageUpperLimits = tmpVoltUpLim;
        pCON->info.dVolatageLowerLimits = tmpVoltLowLim;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }

    return  errcode;
}

static ErrorCode_t GetACTempLimits(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpACTempUpperLim;
    double tmpACTempLowerLim;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpACTempUpperLim = 105;//(℃)
    tmpACTempLowerLim = -40;//(℃)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** 从文件获取 */
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnACTempUpperLimits);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpACTempUpperLim = jsItem->valuedouble;
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnACTempLowerLimits);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpACTempLowerLim = jsItem->valuedouble;

    /*********************/
    if(tmpACTempUpperLim < 1000 && tmpACTempLowerLim > -100)
    {
        pCON->info.dACTempUpperLimits = tmpACTempUpperLim;
        pCON->info.dACTempLowerLimits = tmpACTempLowerLim;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }

    return  errcode;
}

static ErrorCode_t GetSocketTempLimits(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpSocketTempUpperLim;
    double tmpSocketTempLowerLim;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpSocketTempUpperLim = 105;//(℃)
    tmpSocketTempLowerLim = -40;//(℃)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** 从文件获取 */
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnSocketTempUpperLimits);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpSocketTempUpperLim = jsItem->valuedouble;
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnSocketTempLowerLimits);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpSocketTempLowerLim = jsItem->valuedouble;

    /*********************/
    if(tmpSocketTempUpperLim < 1000 && tmpSocketTempLowerLim > -100)
    {
        pCON->info.dSocketTempUpperLimits = tmpSocketTempUpperLim;
        pCON->info.dSocketTempLowerLimits = tmpSocketTempLowerLim;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }

    return  errcode;
}

static ErrorCode_t GetRatedCurrent(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpCurr;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpCurr = 32;//(A)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** 从文件获取 */

    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnRatedCurrent);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpCurr = jsItem->valuedouble;

    /*********************/
    if(tmpCurr > 0)
    {
        pCON->info.dRatedCurrent = tmpCurr;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }

    return  errcode;
}
static ErrorCode_t GetRatedPower(void *pvCON, void *pvCfgObj)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpPow;
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpPow = 7;//(kW)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** 从文件获取 */

    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnRatedPower);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    tmpPow = jsItem->valuedouble;

    /*********************/
    if(tmpPow > 0)
    {
        pCON->info.dRatedPower = tmpPow;
    }
    else
    {
        errcode = ERR_FILE_PARAM;
    }

    return  errcode;
}

/** @brief 从cfg文件获取充电枪配置
 *
 * @param pvCON void*
 * @param pvCfgObj void* NULL
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetCONCfg(void *pvCON, void *pvCfgObj)
{
    cJSON *jsCfgObj;
    cJSON *jsCONArray;
    cJSON *jsCONObj;
    int iArraySize;

    CON_t *pCON;
    int i;
    ErrorCode_t errcode = ERR_NO;

    pCON = (CON_t *)pvCON;

        /*json解析*/
    jsCfgObj = GetCfgObj(pathEVSECfg, &errcode);
    if(jsCfgObj == NULL)
    {
        //errcode 已经在GetCfgObj中获得
        goto exit;
    }
    /*取出CON相关配置*/
    jsCONArray = cJSON_GetObjectItem(jsCfgObj, jnCONArray);
    if(jsCONArray == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto exit_parse;
    }
    iArraySize = cJSON_GetArraySize(jsCONArray);//有多少个充电枪配置
    if(iArraySize != pEVSE->info.ucTotalCON)
    {
        errcode = ERR_FILE_PARAM;
        goto exit_parse;
    }
    jsCONObj = cJSON_GetArrayItem(jsCONArray, pCON->info.ucCONID);
    if(jsCONObj == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto exit_parse;
    }
    THROW_ERROR(defDevID_File, errcode = GetCONType(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetCONType()");
    THROW_ERROR(defDevID_File, errcode = GetSocketType(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetSocketType()");
    THROW_ERROR(defDevID_File, errcode = GetVolatageLimits(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetVolatageLimits()");
    THROW_ERROR(defDevID_File, errcode = GetACTempLimits(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetACTempLimits()");
    THROW_ERROR(defDevID_File, errcode = GetSocketTempLimits(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetSocketTempLimits()");
    THROW_ERROR(defDevID_File, errcode = GetRatedCurrent(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetRatedCurrent()");
    THROW_ERROR(defDevID_File, errcode = GetRatedPower(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetRatedPower()");
exit_parse:
    cJSON_Delete(jsCfgObj);
exit:
    return errcode;
}
/*---------------------------------------------------------------------------/
/                               从驱动获取充电接口状态
/---------------------------------------------------------------------------*/



/** ！！！ 注意不同ID对硬件的不同操作 ！！！ */
/** ！！！ 注意不同ID对硬件的不同操作 ！！！ */
/** ！！！ 注意不同ID对硬件的不同操作 ！！！ */



/** @brief 获取充电电压，检测精度 +/-0.1V
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_METER_FAULT
 *
 */
static ErrorCode_t GetChargingVoltage(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpVolt;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpVolt = 0;
    errcode = ERR_NO;

    /** 获取电压 */

    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpVolt = 220;
#else
        tmpVolt = get_va();
#endif
    }
    if(ucCONID == 1)
    {
        //tmpVolt=get_va();
    }

    /*********************/

    pCON->status.dChargingVoltage = tmpVolt;

    return errcode;
}

/** @brief 获取充电电流，检测精度+/-0.1A
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_METER_FAULT
 *
 */
static ErrorCode_t GetChargingCurrent(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpCurr;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpCurr = 0;
    errcode = ERR_NO;

    /** 获取电流 */

    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpCurr = 32;
#else
        tmpCurr = get_ia();
#endif
    }
    if(ucCONID == 1)
    {
        //tmpCurr=get_ia();
    }

    /*********************/

    pCON->status.dChargingCurrent = tmpCurr;

    return errcode;
}

/** @brief 获取电源频率
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_METER_FAULT
 *
 */
static ErrorCode_t GetChargingFrequence(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpFreq;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;

    /** @todo (yuye#1#): 从电表获取 */
    //meter id 0 == CON id 0
#ifdef DEBUG_DIAG_DUMMY
    tmpFreq = 50;
#else
    Get_electricity_meter_massage(ucCONID, read, frequency, 1);
    tmpFreq = Electricity_meter[ucCONID].massage.massage_frequency;
#endif

    /*********************/

    pCON->status.dChargingFrequence = tmpFreq;

    return errcode;
}

static ErrorCode_t GetChargingPower(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpPower;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;

    /** @todo (yuye#1#): 从电表获取 */
#ifdef DEBUG_DIAG_DUMMY
    tmpPower = 100;
#else
    Get_electricity_meter_massage(ucCONID, read, electric_energy, 1);
    tmpPower = Electricity_meter[ucCONID].massage.massage_electric_energy;
#endif
    /*********************/

    pCON->status.dChargingPower = tmpPower;

    return errcode;
}

/** @brief 获取CP状态
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CP_FAULT
 *
 */
static ErrorCode_t GetCPState(void *pvCON)
{
    float cp1;
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpCPState;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpCPState = CP_12V;
    errcode = ERR_NO;

    /** ***********/

    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpCPState = CP_6V_PWM;
#else
        cp1 = get_CP1();
        if((cp1 < 12.8f) && (cp1 > 11.2f))
        {
            if(TIM2->CCR1 != 1000)
            {
                tmpCPState = CP_12V_PWM;
            }
            else
            {
                tmpCPState = CP_12V;
            }
        }
        else if((cp1 < 9.8f) && (cp1 > 8.2f))
        {
            if(TIM2->CCR1 != 1000)
            {
                tmpCPState = CP_9V_PWM;
            }
            else
            {
                tmpCPState = CP_9V;
            }
        }
        else if((cp1 < 6.8f) && (cp1 > 5.2f))
        {
            if(TIM2->CCR1 != 1000)
            {
                tmpCPState = CP_6V_PWM;
            }
            else
            {
                tmpCPState = CP_6V;
            }
        }
        else
        {
            tmpCPState = CP_ERR;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        ;
    }
    /*********************/

    pCON->status.xCPState = tmpCPState;

    return errcode;
}
/** @brief 控制S1开关
 *
 * @param pvCON void*
 * @param cmd uint8_t   传递开关控制命令，SWITCH_ON /SWITCH_OFF
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetCPSwitch(void *pvCON, uint8_t cmd)
{
    CON_t *pCON;
    uint8_t ucCONID;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;

    /** ****************  */

    //...
    if(ucCONID == 0)
    {
        if(cmd == SWITCH_ON)
        {
            PWM1_535;
        }
        else
        {
            PWM1_1000;
        }
    }
    else if(ucCONID == 1)
    {


        if(cmd == SWITCH_ON)
        {
            PWM2_535;
        }
        else
        {
            PWM2_1000;
        }

    }


    /*********************/

    return errcode;
}
/** @brief 设置PWM占空比 详情请看18487.1-2015 P22
 *
 * @param pvCON void*
 * @param ucLoadPercent uint8_t 负载百分比
 *                              负载100%时，PWM=53
                                负载50%时，PWM= 27
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetLoadPercent(void *pvCON, uint8_t ucLoadPercent)
{
    CON_t *pCON;
    uint8_t ucCONID;
    uint8_t tmpCPPWM;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpCPPWM = 53;
    errcode = ERR_NO;

    /** ************* */
    if(ucCONID == 0)
    {
        TIM2->CCR1 = ucLoadPercent * 10;
    }
    else if(ucCONID == 1)
    {
        TIM4->CCR1 = ucLoadPercent * 10;
    }
    //负载百分比输入范围0~1000；
    //PWM

    /*********************/

    pCON->status.ucLoadPercent = ucLoadPercent;

    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CC_FAULT
 *
 */
static ErrorCode_t GetCCState(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpCCState;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpCCState = CC_NO;
    errcode = ERR_NO;

    /** *****************  */

#ifdef DEBUG_DIAG_DUMMY
    tmpCCState = CC_PE;
#else
    if(ucCONID == 0)
    {
        if(GET_CC1 == 0) //已经连接CC1点，PE连接正常
        {
            tmpCCState = CC_PE;
        }
        else if(GET_CC1 == 1)
        {
            tmpCCState = CC_NO;
        }

    }
    else if(ucCONID == 1)
    {
        if(GET_CC2 == 0) //已经连接CC1点，PE连接正常
        {
            tmpCCState = CC_PE;
        }
        else if(GET_CC2 == 1)
        {
            tmpCCState = CC_NO;
        }
    }
#endif
    /*********************/

    pCON->status.xCCState = tmpCCState;

    return errcode;
}

/** @brief 获取插枪状态，应同时检测检测点1（CC）和检测点4（CP）
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_PLUG_FAULT
 *
 */
static ErrorCode_t GetPlugState(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    uint32_t tmpPlugState; //    PLUG,  UNPLUG,
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpPlugState = UNPLUG;
    errcode = ERR_NO;

    /** 检测插枪状态驱动接口  */
    if(pCON->info.ucSocketType == defSocketTypeB)
    {
        THROW_ERROR(ucCONID, GetCCState(pvCON), ERR_LEVEL_CRITICAL, "GetPlug->GetCC");
        THROW_ERROR(ucCONID, GetCPState(pvCON), ERR_LEVEL_CRITICAL, "GetPlug->GetCP");
        if(pCON->status.xCCState == CC_PE &&
                pCON->status.xCPState != CP_12V &&
                pCON->status.xCPState != CP_ERR)
        {
            tmpPlugState = PLUG;
        }
        else
        {
            tmpPlugState = UNPLUG;
        }
    }
    else if(pCON->info.ucSocketType == defSocketTypeC)
    {
        THROW_ERROR(ucCONID, GetCPState(pvCON), ERR_LEVEL_CRITICAL, "GetPlug->GetCP");
        if(pCON->status.xCPState != CP_12V &&
                pCON->status.xCPState != CP_ERR)
        {
            tmpPlugState = PLUG;
        }
        else
        {
            tmpPlugState = UNPLUG;
        }
    }
    /*********************/

    pCON->status.xPlugState = tmpPlugState;

    return errcode;
}

/** @brief B型连接枪锁状态
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_B_LOCK_FAULT
 *
 */
static ErrorCode_t GetBTypeSocketLock(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpLockState; //lock unlock
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpLockState = UNLOCK;
    errcode = ERR_NO;

    /** 实现代码  */

    if(ucCONID = 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpLockState = LOCK;
#else
        if(GET_GUN_STATE_1 == 1)
        {
            tmpLockState = UNLOCK;
        }
        else if(GET_GUN_STATE_1 == 0)
        {
            tmpLockState = LOCK;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        if(GET_GUN_STATE_2 == 1)
        {
            tmpLockState = UNLOCK;
        }
        else if(GET_GUN_STATE_2 == 0)
        {
            tmpLockState = LOCK;
        }
    }
    /*********************/


    pCON->status.xBTypeSocketLockState = tmpLockState;

    return errcode;
}

/** @brief B型连接枪锁开关
 *
 * @param pvCON void*
 * @param cmd uint8_t   开关控制，SWITCH_ON /SWITCH_OFF
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CANT_LOCK
 *
 */
static ErrorCode_t SetBTypeSocketLock(void *pvCON, uint8_t cmd)
{
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpLockState;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpLockState = UNLOCK;
    errcode = ERR_NO;

    /**  执行锁止动作 */
    if(ucCONID == 0)
    {
        if(cmd == SWITCH_ON)
        {
            Close_gun_1();
        }
        else
        {
            Open_gun_1();
        }
    }
    else if(ucCONID == 1)
    {
        if(cmd == SWITCH_ON)
        {
            Close_gun_2();
        }
        else
        {
            Open_gun_2();
        }
    }
    errcode = GetBTypeSocketLock(pvCON);

    return errcode;
}

/** @brief 获取L进线温度
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_ACLTEMP_DECT_FAULT
 *
 */
static ErrorCode_t GetACLTemp(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double  tmpACLTemp;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpACLTemp = 0;
    errcode = ERR_NO;

    /** 实现代码  */
    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpACLTemp = 25;
#else
        tmpACLTemp = Sys_samp.DC.TEMP1;
#endif
    }
    else if(ucCONID == 1)
    {
        tmpACLTemp = Sys_samp.DC.TEMP2;
    }


    /*********************/

    pCON->status.dACLTemp = tmpACLTemp;

    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_ACNTEMP_DECT_FAULT
 *
 */
static ErrorCode_t GetACNTemp(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpACNTemp;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpACNTemp = 0;
    errcode = ERR_NO;

    /** 实现代码  */

    //...
    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpACNTemp = 25;
#else
        tmpACNTemp = Sys_samp.DC.TEMP3;
#endif
    }
    else if(ucCONID == 1)
    {
        tmpACNTemp = Sys_samp.DC.TEMP4;
    }

    /*********************/

    pCON->status.dACNTemp = tmpACNTemp;

    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_BTEMP1_DECT_FAULT
 *
 */
static ErrorCode_t GetBTypeSocketTemp1(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpTemp;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpTemp = 0;
    errcode = ERR_NO;

    /** 实现代码  */

    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = Sys_samp.DC.TEMP_ARM1;
#endif
    }
    else if(ucCONID == 1)
    {
        tmpTemp = Sys_samp.DC.TEMP_ARM2;
    }
    /*********************/

    pCON->status.dBTypeSocketTemp1 = tmpTemp;

    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_BTEMP2_DECT_FAULT
 */
static ErrorCode_t GetBTypeSocketTemp2(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpTemp;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpTemp = 0;
    errcode = ERR_NO;

    /** 实现代码  */

    //...
    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = Sys_samp.DC.TEMP_ARM3;
#endif
    }
    else if(ucCONID == 1)
    {
        tmpTemp = Sys_samp.DC.TEMP_ARM4;
    }

    /*********************/

    pCON->status.dBTypeSocketTemp2 = tmpTemp;

    return errcode;
}
/** @brief 获取输出继电器状态
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetRelayState(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    uint8_t tmpLStat;
    uint8_t tmpNStat;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpLStat = SWITCH_OFF;
    tmpNStat = SWITCH_OFF;
    errcode = ERR_NO;

    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
    tmpLStat = SWITCH_ON;
    tmpNStat = tmpLStat;
#else
    read_pca9554_2();
    tmpLStat = (read_pca9554_2() >> 6) && 0x01;
    tmpNStat = tmpLStat;
#endif
    /*********************/
    /* @todo (yuye#1#): 触电粘连处理2017年4月10日 */
//    if(触电粘连)
//    {
//        errcode = ERR_RELAY_PASTE;
//    }


    pCON->status.ucRelayLState = tmpLStat;
    pCON->status.ucRelayNState = tmpNStat;

    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @param cmd uint8_t SWITCH_ON SWITCH_OFF
 * @return ErrorCode_t
 *
 */          //K1 K2指的是什么
static ErrorCode_t SetRelay(void *pvCON, uint8_t cmd)
{
    CON_t *pCON;
    uint8_t ucCONID;

    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;


    /** 操作K1,K2输出继电器 */
    if(ucCONID == 0)
    {
        if(cmd == SWITCH_OFF)
        {
            POWER_L_OPEN();
            POWER_N_OPEN();
        }
        else if(cmd == SWITCH_ON)
        {
            POWER_L_CLOSE();
            POWER_N_CLOSE();
        }
    }
    else if(ucCONID == 1)
    {
        ;
    }
    /*********************/

    errcode = GetRelayState(pvCON);
    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_STARTCHARGE
 *
 */
static ErrorCode_t StartCharge(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpChargeStatus;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;
    /**  操作输出继电器，保存继电器状态 */

    errcode = SetRelay(pvCON, SWITCH_ON);

    /*********************/
    return errcode;
}
/** @brief
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_STOPCHARGE
 *
 */
static ErrorCode_t StopCharge(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpChargeStatus;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;

    /** 操作输出继电器，保存继电器状态 */

    errcode = SetRelay(pvCON, SWITCH_OFF);

    /*********************/
    return errcode;
}

CON_t *CONGetHandle(uint8_t ucCONID)
{
    CON_t *pCON;

    pCON =  (CON_t *)(pListCON->pListPointArray[ucCONID]);

    return pCON;
}
CONState_t CONGetState(uint8_t ucCONID)
{
    CON_t *pCON = NULL;

    pCON = CONGetHandle(ucCONID);

    return pCON->state;
}
static void CONDelete(CON_t *pCON)
{
    vEventGroupDelete(pCON->status.xHandleEventCharge);
    vEventGroupDelete(pCON->status.xHandleEventException);
    if(pCON->order.plChargeSegment != NULL)
    {
        gdsl_list_free(pCON->order.plChargeSegment);
    }
    free(pCON);
    pCON = NULL;
}
CON_t *CONCreate(uint8_t ucCONID )
{
    CON_t *pCON;
    pCON = (CON_t *)malloc(sizeof(CON_t));
    if(pCON == NULL)
    {
        return NULL;
    }
    pCON->info.ucCONID = ucCONID;
    pCON->info.ucCONType = defCONType_AC;
    pCON->info.ucSocketType = defSocketTypeB;
    pCON->info.dVolatageUpperLimits = 0;
    pCON->info.dVolatageLowerLimits = 0;
    pCON->info.dACTempUpperLimits = 0;
    pCON->info.dACTempLowerLimits = -0;
    pCON->info.dSocketTempUpperLimits = 0;
    pCON->info.dSocketTempLowerLimits = 0;
    pCON->info.dRatedCurrent = 32;
    pCON->info.dRatedPower = 7;

    pCON->info.GetCONCfg = GetCONCfg;
    pCON->info.SetCONCfg = SetCONCfg;
//    pCON->info.SetCONType = SetCONType;
//    pCON->info.SetSocketType = SetSocketType;
//    pCON->info.SetVolatageUpperLimits = SetVolatageUpperLimits;
//    pCON->info.SetVolatageLowerLimits = SetVolatageLowerLimits;
//    pCON->info.SetACTempUpperLimits = SetACTempUpperLimits;
//    pCON->info.SetACTempLowerLimits = SetACTempLowerLimits;
//    pCON->info.SetSocketTempUpperLimits = SetSocketTempUpperLimits;
//    pCON->info.SetSocketTempLowerLimits = SetSocketTempLowerLimits;
//    pCON->info.SetRatedCurrent = SetRatedCurrent;
//    pCON->info.SetRatedPower = SetRatedPower;

    //memset(pCON->status.ucHeldCardID, 0, defCardIDLength);
    pCON->status.dACLTemp = 0;
    pCON->status.dACNTemp = 0;
    pCON->status.dBTypeSocketTemp1 = 0;
    pCON->status.dBTypeSocketTemp2 = 0;
    pCON->status.dChargingCurrent = 0;
    pCON->status.dChargingFrequence = 0;
    pCON->status.dChargingVoltage = 0;
    pCON->status.dChargingPower = 0;
    pCON->status.xBTypeSocketLockState = 0;
    pCON->status.xCCState = 0;
    pCON->status.xCPState = 0;
    pCON->status.ucLoadPercent = 100;//(%)
    pCON->status.xPlugState = 0;
    pCON->status.xHandleEventCharge = xEventGroupCreate();
    pCON->status.xHandleEventException = xEventGroupCreate();
    pCON->status.xHandleTimerVolt = NULL;
    pCON->status.xHandleTimerCurr = NULL;
    pCON->status.xHandleTimerCharge = NULL;
    pCON->status.GetChargingVoltage = GetChargingVoltage;
    pCON->status.GetChargingCurrent = GetChargingCurrent;
    pCON->status.GetChargingFrequence = GetChargingFrequence;
    pCON->status.GetChargingPower = GetChargingPower;
    pCON->status.xVoltStat = STATE_VOLT_OK;
    pCON->status.xCurrStat = STATE_CURR_INIT;

    pCON->status.GetCPState = GetCPState;
    pCON->status.SetCPSwitch = SetCPSwitch;
    pCON->status.SetLoadPercent = SetLoadPercent;
    pCON->status.GetCCState = GetCCState;
    pCON->status.GetPlugState = GetPlugState;
    pCON->status.GetBTypeSocketLock = GetBTypeSocketLock;
    pCON->status.SetBTypeSocketLock = SetBTypeSocketLock;
    pCON->status.GetACLTemp = GetACLTemp;
    pCON->status.GetACNTemp = GetACNTemp;
    pCON->status.GetBTypeSocketTemp1 = GetBTypeSocketTemp1;
    pCON->status.GetBTypeSocketTemp2 = GetBTypeSocketTemp2;
    pCON->status.StartCharge = StartCharge;
    pCON->status.StopCharge = StopCharge;
    pCON->status.GetRelayState = GetRelayState;
    pCON->status.SetRelay = SetRelay;

    pCON->state = STATE_CON_IDLE;

    OrderCreate(&(pCON->order));
    OrderInit(&(pCON->order));

    return pCON;
}
