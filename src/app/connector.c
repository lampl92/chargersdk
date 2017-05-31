/**
* @file connector.c
* @brief ������С��絥Ԫ��Ҫ�����������
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
#include "electric_energy_meter.h"
/*---------------------------------------------------------------------------/
/                               ���ó��ӿ���Ϣ�������ļ�
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
            break;//�˳�whileѭ��
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
#if 0
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
#endif
/*---------------------------------------------------------------------------/
/                               ���ļ���ȡ���ӿ���Ϣ
/---------------------------------------------------------------------------*/

/** @todo (rgw#1#): ����ǹ�������CONType */

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

    /** ���ļ���ȡ */
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
static ErrorCode_t GetCONQRCode(void *pvCON, void *pvCfgObj)
{
    /** @todo (rgw#1#): �ú���δ���� */
    CON_t *pCON;
    uint8_t ucCONID;
    uint8_t tmpQRCode[defQRCodeLength];
    ErrorCode_t errcode;

    cJSON *jsItem;
    cJSON *pCONCfgObj;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    memset(tmpQRCode, 0, defQRCodeLength);
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** ���ļ���ȡ */
    jsItem = cJSON_GetObjectItem(pCONCfgObj, jnQRCode);
    if(jsItem == NULL)
    {
        return ERR_FILE_PARSE;
    }
    strncpy(tmpQRCode, jsItem->valuestring, strlen(jsItem->valuestring));

    /*********************/
    if(strlen(tmpQRCode) > 0)
    {
        strncpy(pCON->info.strQRCode, tmpQRCode, strlen(tmpQRCode));
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
    /** @todo (rgw#1#): ���ļ���ȡ */
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
    /** @todo (rgw#1#): ���ļ���ȡ */
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
    tmpACTempUpperLim = 105;//(��)
    tmpACTempLowerLim = -40;//(��)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** ���ļ���ȡ */
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
    tmpSocketTempUpperLim = 105;//(��)
    tmpSocketTempLowerLim = -40;//(��)
    errcode = ERR_NO;

    pCONCfgObj = (cJSON *)pvCfgObj;

    /** ���ļ���ȡ */
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

    /** ���ļ���ȡ */

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

    /** ���ļ���ȡ */

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

/** @brief ��cfg�ļ���ȡ���ǹ����
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

    /*json����*/
    jsCfgObj = GetCfgObj(pathEVSECfg, &errcode);
    if(jsCfgObj == NULL)
    {
        //errcode �Ѿ���GetCfgObj�л��
        goto exit;
    }
    /*ȡ��CON�������*/
    jsCONArray = cJSON_GetObjectItem(jsCfgObj, jnCONArray);
    if(jsCONArray == NULL)
    {
        errcode = ERR_FILE_PARSE;
        goto exit_parse;
    }
    iArraySize = cJSON_GetArraySize(jsCONArray);//�ж��ٸ����ǹ����
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
    THROW_ERROR(defDevID_File, errcode = GetCONQRCode(pvCON, jsCONObj), ERR_LEVEL_WARNING, "GetCONQRCode()");
exit_parse:
    cJSON_Delete(jsCfgObj);
exit:
    return errcode;
}
/*---------------------------------------------------------------------------/
/                               ��������ȡ���ӿ�״̬
/---------------------------------------------------------------------------*/



/** ������ ע�ⲻͬID��Ӳ���Ĳ�ͬ���� ������ */
/** ������ ע�ⲻͬID��Ӳ���Ĳ�ͬ���� ������ */
/** ������ ע�ⲻͬID��Ӳ���Ĳ�ͬ���� ������ */



/** @brief ��ȡ����ѹ����⾫�� +/-0.1V
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

    /** ��ȡ��ѹ */

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

/** @brief ��ȡ����������⾫��+/-0.1A
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CON_METER_FAULT
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

    /** ��ȡ���� */
    if(Electricity_meter[ucCONID].flag.flag_erro == 1)
    {
        return ERR_CON_METER_FAULT;
    }
    else
    {

#ifdef DEBUG_DIAG_DUMMY
        tmpCurr = 32;
#else
        tmpCurr = Get_Electricity_meter_massage_current(ucCONID+1);
#endif

    }

    /*********************/

    pCON->status.dChargingCurrent = tmpCurr;

    return errcode;
}

/** @brief ��ȡ��ԴƵ��
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CON_METER_FAULT
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

    /** @todo (yuye#1#): �ӵ���ȡ */
    //meter id 0 == CON id 0
#ifdef DEBUG_DIAG_DUMMY
    tmpFreq = 50;
#else
    if(Electricity_meter[ucCONID].flag.flag_erro == 1)
    {
        return ERR_CON_METER_FAULT;
    }
    else
    {
        tmpFreq = Get_Electricity_meter_massage_frequency(ucCONID + 1);
    }

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

    /** @todo (yuye#1#): �ӵ���ȡ */
    if(Electricity_meter[ucCONID].flag.flag_erro == 1)
    {
        return ERR_CON_METER_FAULT;
    }
    else
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpPower = 100;
    #else
        tmpPower = Get_Electricity_meter_massage_energy(ucCONID+1);
    #endif
    }

    /*********************/

    pCON->status.dChargingPower = pCON->status.dChargingPower+0.01;//tmpPower;

    return errcode;
}

/** @brief ��ȡCP״̬
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CP_FAULT
 *
 */
static ErrorCode_t GetCPState(void *pvCON)
{
    float cp1, cp2;
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
        cp1 = Sys_samp.DC.CP1;//get_CP1();
        if((cp1 < 12.8f) && (cp1 > 11.2f))
        {
            if(TIM2->CCR1 != TIMER_MAX)
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
            if(TIM2->CCR1 != TIMER_MAX)
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
            if(TIM2->CCR1 != TIMER_MAX)
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
            printf_safe("CPERR %lf\n",cp1);
            tmpCPState = CP_ERR;
            return ERR_CON_CP_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        cp1 = get_CP2();
        if((cp2 < 12.8f) && (cp2 > 11.2f))
        {
            if(TIM4->CCR1 != TIMER_MAX)
            {
                tmpCPState = CP_12V_PWM;
            }
            else
            {
                tmpCPState = CP_12V;
            }
        }
        else if((cp2 < 9.8f) && (cp2 > 8.2f))
        {
            if(TIM4->CCR1 != TIMER_MAX)
            {
                tmpCPState = CP_9V_PWM;
            }
            else
            {
                tmpCPState = CP_9V;
            }
        }
        else if((cp2 < 6.8f) && (cp2 > 5.2f))
        {
            if(TIM2->CCR1 != TIMER_MAX)
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
            return ERR_CON_CP_FAULT;
        } ;
    }
    /*********************/

    pCON->status.xCPState = tmpCPState;

    return errcode;
}
/** @brief ����S1����
 *
 * @param pvCON void*
 * @param cmd uint8_t   ���ݿ��ؿ������SWITCH_ON /SWITCH_OFF
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
        else if(cmd == SWITCH_OFF)
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
/** @brief ����PWMռ�ձ� �����뿴18487.1-2015 P22
 *
 * @param pvCON void*
 * @param ucLoadPercent uint8_t ���ذٷֱ�
 *                              ����100%ʱ��PWM=53
                                ����50%ʱ��PWM= 27
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
        TIM2->CCR1 = 1001-ucLoadPercent * 10;
    }
    else if(ucCONID == 1)
    {
        TIM4->CCR1 = 1001-ucLoadPercent * 10;
    }
    //���ذٷֱ����뷶Χ0~1000��
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
        if(GET_CC1 == 0) //�Ѿ�����CC1�㣬PE��������
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
        if(GET_CC2 == 0) //�Ѿ�����CC1�㣬PE��������
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

/** @brief ��ȡ��ǹ״̬��Ӧͬʱ������1��CC���ͼ���4��CP��
 *
 * @param pvCON void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CON_PLUG_FAULT
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

    /** ����ǹ״̬�����ӿ�  */
    if(pCON->info.ucSocketType == defSocketTypeB)
    {
        THROW_ERROR(ucCONID, GetCCState(pvCON), ERR_LEVEL_CRITICAL, "GetPlug->GetCC");
        THROW_ERROR(ucCONID, GetCPState(pvCON), ERR_LEVEL_CRITICAL, "GetPlug->GetCP");
        if(pCON->status.xCCState == CC_PE &&
                pCON->status.xCPState != CP_12V &&
                pCON->status.xCPState != CP_12V_PWM &&
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
           pCON->status.xCPState != CP_12V_PWM &&
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

/** @brief B������ǹ��״̬
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

    /** ʵ�ִ���  */

    if(ucCONID == 0)
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

/** @brief B������ǹ������
 *
 * @param pvCON void*
 * @param cmd uint8_t   ���ؿ��ƣ�SWITCH_ON /SWITCH_OFF
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

    /**  ִ����ֹ���� */
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

/** @brief ��ȡL�����¶�
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

    /** ʵ�ִ���  */
    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpACLTemp = 25;
#else
        get_dc_massage(TEMP_L_OUT);
        tmpACLTemp = (double)Sys_samp.DC.TEMP1;
        if(tmpACLTemp > 100 || tmpACLTemp < -40)
        {
            return ERR_CON_ACLTEMP_DECT_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        tmpACLTemp = get_dc_massage(TEMP_L_OUT);
        if(tmpACLTemp > 100 || tmpACLTemp < -40)
        {
            return ERR_CON_ACLTEMP_DECT_FAULT;
        }
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

    /** ʵ�ִ���  */

    //...
    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpACNTemp = 25;
#else
        get_dc_massage(TEMP_N_OUT);
        tmpACNTemp = (double)Sys_samp.DC.TEMP3;
        if(tmpACNTemp > 100 || tmpACNTemp < -40)
        {
            return ERR_CON_ACNTEMP_DECT_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        tmpACNTemp = get_dc_massage(TEMP_N_OUT);
        if(tmpACNTemp > 100 || tmpACNTemp < -40)
        {
            return ERR_CON_ACNTEMP_DECT_FAULT;
        }
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

    /** ʵ�ִ���  */

    if(ucCONID == 0)
    {
#ifndef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = get_dc_massage(TEMP_GUN1_NEGATIVE);
        if(tmpTemp > 100 || tmpTemp < -40)
        {
            return ERR_CON_BTEMP1_DECT_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        tmpTemp = get_dc_massage(TEMP_GUN2_NEGATIVE);
        if(tmpTemp > 100 || tmpTemp < -40)
        {
            return ERR_CON_BTEMP1_DECT_FAULT;
        }
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

    /** ʵ�ִ���  */

    //...
    if(ucCONID == 0)
    {
#ifndef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = get_dc_massage(TEMP_GUN1_POSITIVE);
        if(tmpTemp > 100 || tmpTemp < -40)
        {
            return ERR_CON_BTEMP2_DECT_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
        tmpTemp = get_dc_massage(TEMP_GUN2_POSITIVE);
        if(tmpTemp > 100 || tmpTemp < -40)
        {
            return ERR_CON_BTEMP2_DECT_FAULT;
        }
    }

    /*********************/

    pCON->status.dBTypeSocketTemp2 = tmpTemp;

    return errcode;
}
/** @brief ��ȡ����̵���״̬
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

    /** ʵ�ִ���  */
#ifdef DEBUG_DIAG_DUMMY
    tmpLStat = SWITCH_ON;
    tmpNStat = tmpLStat;
#else

    tmpLStat = flag_power_out_l;//Get_State_relay();//1 : switch on
    tmpNStat = tmpLStat;
#endif
    /*********************/
    /* @todo (yuye#1#): ����ճ������2017��4��10�� */
//    if(����ճ��)
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
 */          //K1 K2ָ����ʲô
static ErrorCode_t SetRelay(void *pvCON, uint8_t cmd)
{
    CON_t *pCON;
    uint8_t ucCONID;

    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;


    /** ����K1,K2����̵��� */
    if(ucCONID == 0)
    {
        if(cmd == SWITCH_OFF)
        {
#ifdef DEBUG_DIAG_DUMMY
#else
            POWER_L_OPEN();
            POWER_N_OPEN();
#endif
        }
        else if(cmd == SWITCH_ON)
        {
#ifdef DEBUG_DIAG_DUMMY
#else
            POWER_L_CLOSE();
            POWER_N_CLOSE();
#endif
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
    /**  ��������̵���������̵���״̬ */

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

    /** ��������̵���������̵���״̬ */

    errcode = SetRelay(pvCON, SWITCH_OFF);

    /*********************/
    return errcode;
}

CON_t *CONGetHandle(uint8_t ucCONID)
{
    CON_t *pCON;
    uint32_t ulTotalCON;

    ulTotalCON = pListCON->Total;

    if(ucCONID < ulTotalCON)
    {
        return (CON_t *)(pListCON->pListPointArray[ucCONID]);
    }
    else
    {
        return NULL;
    }
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
    memset(pCON->info.strQRCode, 0, sizeof(pCON->info.strQRCode));

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
    pCON->status.xHandleEventOrder = xEventGroupCreate();
    pCON->status.xHandleEventException = xEventGroupCreate();
    pCON->status.xHandleTimerVolt = NULL;
    pCON->status.xHandleTimerCurr = NULL;
    pCON->status.xHandleTimerCharge = NULL;
    pCON->status.xHandleTimerRTData = NULL;
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

    pCON->status.xHandleTimerRTData = xTimerCreate("TimerRemoteRTData",
                                                   defRemoteRTDataCyc,
                                                   pdTRUE,
                                                   (void *)(int)ucCONID,
                                                   vRemoteRTDataTimerCB);


    OrderCreate(&(pCON->order));
    OrderInit(&(pCON->order));

    return pCON;
}
