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
#include "line.h"
#include "bsp.h"
#include "user_app.h"
#include "cJSON.h"
#include "stringName.h"
#include "cfg_parse.h"
#include "timercallback.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "timers.h"

#if DEBUG_DIAG_DUMMY
    #define DEBUG_DIAG_DUMMY_RELAY
    #define DEBUG_DIAG_DUMMY_METER
#endif

#if 0
static int SetSignalPool(void *pvDev, uint32_t block, uint32_t bit)
{
    CON_t *pCON;
    
    pCON = (CON_t *)pvDev;
    if (block >= CON_MAX_SIGNAL_BLOCK)
    {
        while (1)
            ;
    }
    pCON->status.ulSignalPool[block] |= bit;
    
    return 1;
}
static int ClrSignalPool(void *pvDev, uint32_t block, uint32_t bit)
{
    CON_t *pCON;
    
    pCON = (CON_t *)pvDev;
    if (block >= CON_MAX_SIGNAL_BLOCK)
    {
        while (1)
            ;
    }
    pCON->status.ulSignalPool[block] &= ~bit;
    
    return 1;
}
static int GetSignalPool(void *pvDev, uint32_t block, uint32_t bit)
{
    CON_t *pCON;
    
    pCON = (CON_t *)pvDev;
    if (block >= CON_MAX_SIGNAL_BLOCK)
    {
        while (1)
            ;
    }
    if ((pCON->status.ulSignalPool[block] & bit) == bit)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
#endif
/*---------------------------------------------------------------------------*/
/*                               设置充电接口信息到配置文件                    */
/*---------------------------------------------------------------------------*/

static ErrorCode_t SetCONCfg(void *pvCON, char *jnItemString, void *pvCfgParam, uint8_t type)
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
                cJSON_ReplaceItemInObject(jsCONObj, jnItemString, cJSON_CreateString((char *)pvCfgParam));
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
    errcode = SetCfgObj(pathEVSECfg, jsEVSECfgObj, 0);

    return errcode;
}

/*---------------------------------------------------------------------------*/
/*                              从文件获取充电接口信息                         */
/*---------------------------------------------------------------------------*/
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
    CON_t *pCON;
    int i;
    ErrorCode_t errcode = ERR_NO;

    pCON = (CON_t *)pvCON;

    /*json解析*/
    if (pvCfgObj == NULL)
    {
        jsCfgObj = GetCfgObj(pathEVSECfg, &errcode);
        if (jsCfgObj == NULL)
        {
            return errcode;
        }
    }
    else
    {
        jsCfgObj = (cJSON *)pvCfgObj;
    }
    
    cfgobj_get_uint8(jsCfgObj, &pCON->info.ucCONType, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnType);
    cfgobj_get_uint8(jsCfgObj, &pCON->info.ucSocketType, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnSocketType);
    cfgobj_get_double(jsCfgObj, &pCON->info.dVolatageUpperLimits, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnVolatageUpperLimits);
    cfgobj_get_double(jsCfgObj, &pCON->info.dVolatageLowerLimits, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnVolatageLowerLimits);
    cfgobj_get_double(jsCfgObj, &pCON->info.dACTempUpperLimits, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnACTempUpperLimits);
    cfgobj_get_double(jsCfgObj, &pCON->info.dACTempLowerLimits, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnACTempLowerLimits);
    cfgobj_get_double(jsCfgObj, &pCON->info.dSocketTempUpperLimits, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnSocketTempUpperLimits);
    cfgobj_get_double(jsCfgObj, &pCON->info.dSocketTempLowerLimits, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnSocketTempLowerLimits);
    //cfgobj_get_double(jsCfgObj, &pCON->info.dRatedCurrent, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnRatedCurrent);
    cfgobj_get_double(jsCfgObj, &pCON->info.dRatedPower, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnRatedPower);
    if (pEVSE->info.ucPhaseLine == 1)
    {
        pCON->info.dRatedCurrent = pCON->info.dRatedPower * 1000 / 220.0;
    }
    else// if(pEVSE->info.ucPhaseLine == 3)
    {
        pCON->info.dRatedCurrent = pCON->info.dRatedPower / 3.0 * 1000 / 220.0;
    }
    cfgobj_get_string(jsCfgObj, pCON->info.strQRCode, "%s:%d.%s", jnCONArray, pCON->info.ucCONID, jnQRCode);
    
    ////EVSE使用CON温度
    pEVSE->info.dACTempLowerLimits = pCON->info.dACTempLowerLimits;
    pEVSE->info.dACTempUpperLimits = pCON->info.dACTempUpperLimits;
    ////
    
    cJSON_Delete(jsCfgObj);
    return errcode;
}

/*---------------------------------------------------------------------------*/
/*                              从驱动获取充电接口状态                         */
/*---------------------------------------------------------------------------*/


/** ！！！ 注意不同ID对硬件的不同操作 ！！！ */
/** ！！！ 注意不同ID对硬件的不同操作 ！！！ */
/** ！！！ 注意不同ID对硬件的不同操作 ！！！ */

/**
读电表
*/
static ErrorCode_t GetChargingData(void *pvCON)
{
    CON_t *pCON;
    uint8_t ucCONID;
    double tmpVolt = 0;
    double tmpCurr = 0;
    double tmpPower = 0;
    double tmpFreq = 0;
    double tmpEnergy = 0;
    ErrorCode_t errcode;
    int res;
    int i;
    
    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;

    /** 从电表获取 */
#ifdef  DEBUG_DIAG_DUMMY_METER
    tmpVolt = 220;
    tmpCurr = 32;
    tmpFreq = 50;
    tmpPower = pCON->status.dChargingPower;
    tmpPower += 0.005;
    tmpEnergy = pCON->status.dChargingEnergy;
    tmpEnergy += 0.005;
#else
    res = meter->get_all(meter, ucCONID + 1);
    if (res < 0)
    {
        pCON->status.dChargingVoltage = 0;
        pCON->status.dChargingCurrent = 0;
//        pCON->status.dChargingPower = 0;
        pCON->status.dChargingFrequence = 0; 
        //保留电表读数不清零
        errcode = ERR_CON_METER_FAULT;
        
        return errcode; 
    }
    for (i = 0; i < pEVSE->info.ucPhaseLine; i++)
    {
        pCON->status.dLineVolt[i] = meter->status.volt[i];
        pCON->status.dLineCurr[i] = meter->status.curr[i];
        
        tmpVolt = meter->status.volt[i] > tmpVolt ? meter->status.volt[i] : tmpVolt;//取大
        tmpCurr = meter->status.curr[i] > tmpCurr ? meter->status.curr[i] : tmpCurr;//取大
        tmpPower += meter->status.pwr[i];//求和
    }
    tmpFreq = meter->status.freq;
    tmpEnergy = meter->status.energy;
#endif

    pCON->status.dChargingVoltage = tmpVolt;
    pCON->status.dChargingCurrent = tmpCurr;
    pCON->status.dChargingPower = tmpPower;
    pCON->status.dChargingFrequence = tmpFreq; 
    pCON->status.dChargingEnergy = tmpEnergy; 

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
    if (ucCONID == 0)
    {
        if (cmd == SWITCH_ON)
        {
            curr2pwm(pCON->info.dRatedCurrent, ucCONID);
        }
        else if (cmd == SWITCH_OFF)
        {
            PWM1_1000;
        }
    }
    else if (ucCONID == 1)
    {
        if (cmd == SWITCH_ON)
        {
            curr2pwm(pCON->info.dRatedCurrent, ucCONID);
        }
        else
        {
            PWM2_1000;
        }
    }
    /*********************/

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
    float cp;
    CON_t *pCON;
    uint8_t ucCONID;
    CONStatusType_t tmpCPState;
    ErrorCode_t errcode;
    static uint32_t cp1_err_cont;
    static uint32_t cp2_err_cont;
    uint32_t *pCP_err_cont;
    __IO uint32_t *pCCRx;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpCPState = CP_12V;
    errcode = ERR_NO;

    /** ***********/
#ifdef DEBUG_DIAG_DUMMY
        tmpCPState = CP_6V_PWM;
#else
    if (ucCONID == 0)
    {
        pCP_err_cont = &cp1_err_cont;
        pCCRx = &(TIM2->CCR1);
        pCON->status.dCPVolt = Sys_samp.DC.CP1; 
    }
    else if (ucCONID == 1)
    {
        pCP_err_cont = &cp2_err_cont;
        pCCRx = &(TIM4->CCR2);

        pCON->status.dCPVolt = Sys_samp.DC.CP2; 
    }
    
    if ((pCON->status.dCPVolt < 12.8f) && (pCON->status.dCPVolt > 11.2f))
    {
        if (*pCCRx != TIMER_MAX)
        {
            tmpCPState = CP_12V_PWM;
            *pCP_err_cont = 0;
        }
        else
        {
            tmpCPState = CP_12V;
            *pCP_err_cont = 0;
        }
        pCON->status.ulSignalFault &= ~defSignalCON_Fault_CP;
    }
    else if((pCON->status.dCPVolt < 9.8f) && (pCON->status.dCPVolt > 7.8f))//标准8.2~9.8
    {
        if (*pCCRx != TIMER_MAX)
        {
            tmpCPState = CP_9V_PWM;
            *pCP_err_cont = 0;
        }
        else
        {
            tmpCPState = CP_9V;
            *pCP_err_cont = 0;
        }
        pCON->status.ulSignalFault &= ~defSignalCON_Fault_CP;
    }
    else if((pCON->status.dCPVolt < 6.8f) && (pCON->status.dCPVolt > 5.2f))
    {
        if (*pCCRx != TIMER_MAX)
        {
            tmpCPState = CP_6V_PWM;
            *pCP_err_cont = 0;
        }
        else
        {
            tmpCPState = CP_6V;
            *pCP_err_cont = 0;
        }
        pCON->status.ulSignalFault &= ~defSignalCON_Fault_CP;
    }
    else
    {
//        printf_safe("CP%d_ERR %lf\n", ucCONID+1, pCON->status.dCPVolt);
//        printf_safe("CP%d_ERR %lf\n", ucCONID+1, pCON->status.dCPVolt);
//        printf_safe("CP%d_ERR %lf\n", ucCONID+1, pCON->status.dCPVolt);
//        printf_safe("CP%d_ERR %lf\n", ucCONID+1, pCON->status.dCPVolt);
//        printf_safe("CP%d_ERR %lf\n", ucCONID+1, pCON->status.dCPVolt);
//        printf_safe("CP%d_ERR %lf\n", ucCONID+1, pCON->status.dCPVolt);
        tmpCPState = CP_ERR;
        if (tmpCPState == CP_ERR)
        {
            (*pCP_err_cont)++;
            tmpCPState = pCON->status.xCPState;
        }
        if (*pCP_err_cont >= 3)//3次cp故障
        {
            SetCPSwitch(pCON, SWITCH_OFF);
            *pCP_err_cont = 0;
            tmpCPState = CP_ERR;
            pCON->status.ulSignalFault |= defSignalCON_Fault_CP;
            errcode =  ERR_CON_CP_FAULT;
        }
    }
#endif
    /*********************/

    pCON->status.xCPState = tmpCPState;

    return errcode;
}

/** @brief 设置PWM占空比 详情请看18487.1-2015 P22
 *
 * @param pvCON void*
 * @param ucLoadPercent uint8_t 负载百分比 0 ~ 100
 *
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetLoadPercent(void *pvCON, uint8_t ucLoadPercent)
{
    CON_t *pCON;
    uint8_t ucCONID;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;

    if (ucLoadPercent == pCON->status.ucLoadPercent)//相同负载百分比不进行设置
    {
        return errcode;
    }
    /*********************/
    curr2pwm(pCON->info.dRatedPower * (ucLoadPercent / 100.0) * 1000 / pEVSE->info.ucPhaseLine / 220, ucCONID);
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
            pCON->status.ulSignalState |= defSignalCON_State_CC;
        }
        else if(GET_CC1 == 1)
        {
            tmpCCState = CC_NO;
            pCON->status.ulSignalState &= ~defSignalCON_State_CC;
        }

    }
    else if(ucCONID == 1)
    {
        if(GET_CC2 == 0) //已经连接CC1点，PE连接正常
        {
            tmpCCState = CC_PE;
            pCON->status.ulSignalState |= defSignalCON_State_CC;
        }
        else if(GET_CC2 == 1)
        {
            tmpCCState = CC_NO;
            pCON->status.ulSignalState &= ~defSignalCON_State_CC;
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

    /** 检测插枪状态驱动接口  */
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
#ifdef DEBUG_DIAG_DUMMY
        tmpLockState = LOCK;
#else
    if(ucCONID == 0)
    {
        if(GET_GUN_STATE_1 == 1)
        {
            tmpLockState = UNLOCK;
        }
        else if(GET_GUN_STATE_1 == 0)
        {
            tmpLockState = LOCK;
        }
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
#endif
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
    int i;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpACLTemp = -60;
    errcode = ERR_NO;

    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
    tmpACLTemp = 40;
#else
    for (i = 0; i < 3; i++)
    {
        errcode = pCON->line[i].GetTemp(&(pCON->line[i]), pCON, i);
        if (pCON->line[i].status.dTemp > tmpACLTemp)
        {
            tmpACLTemp = pCON->line[i].status.dTemp;
        }
    }
        
    if (tmpACLTemp > 200 || tmpACLTemp < -60)
    {
        errcode = ERR_CON_ACTEMP_DECT_FAULT;
    }
#endif
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
    tmpACNTemp = -60;
    errcode = ERR_NO;

    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
        tmpACNTemp = 25;
#else
    errcode = pCON->line[defLineN].GetTemp(&(pCON->line[defLineN]), pCON, defLineN);
    if (pCON->line[defLineN].status.dTemp > tmpACNTemp)
    {
        tmpACNTemp = pCON->line[defLineN].status.dTemp;
    }
    
    if (tmpACNTemp > 200 || tmpACNTemp < -60)
    {
        errcode = ERR_CON_ACTEMP_DECT_FAULT;
    }
#endif
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
        tmpTemp = get_dc_massage(TEMP_GUN1_NEGATIVE);
        if(tmpTemp > 200 || tmpTemp < -60)
        {
            errcode = ERR_CON_BTEMP1_DECT_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = get_dc_massage(TEMP_GUN2_NEGATIVE);
        if(tmpTemp > 100 || tmpTemp < -60)
        {
            errcode = ERR_CON_BTEMP1_DECT_FAULT;
        }
#endif
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
    if(ucCONID == 0)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = get_dc_massage(TEMP_GUN1_POSITIVE);
        if(tmpTemp > 100 || tmpTemp < -60)
        {
            errcode = ERR_CON_BTEMP2_DECT_FAULT;
        }
#endif
    }
    else if(ucCONID == 1)
    {
#ifdef DEBUG_DIAG_DUMMY
        tmpTemp = 25;
#else
        tmpTemp = get_dc_massage(TEMP_GUN2_POSITIVE);
        if(tmpTemp > 100 || tmpTemp < -60)
        {
            errcode = ERR_CON_BTEMP2_DECT_FAULT;
        }
#endif
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
    uint8_t ucRelayID;
    uint8_t tmpLStat;
    uint8_t tmpNStat;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    tmpLStat = SWITCH_OFF;
    tmpNStat = SWITCH_OFF;
    errcode = ERR_NO;

    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY_RELAY
    tmpLStat = pCON->status.ucRelayLState;
    tmpNStat = tmpLStat;
#else
    if (pEVSE->info.ucPhaseLine == 3)
    {
        ucRelayID = ucCONID;
        tmpLStat = Get_State_relay(ucRelayID);//1 : switch on
        tmpNStat = tmpLStat; 
    }
    else
    {
        //tmpLStat = 1 ==》switch on
        tmpLStat = Get_State_relay(0);//L
        tmpNStat = tmpLStat; 
    }
#endif
    if (tmpLStat == SWITCH_ON)
    {
        pCON->status.ulSignalState |= defSignalCON_State_AC_A_Relay;
    }
    else if (tmpLStat == SWITCH_OFF)
    {
        pCON->status.ulSignalState &= ~defSignalCON_State_AC_A_Relay;
    }
    if (tmpNStat == SWITCH_ON)
    {
        pCON->status.ulSignalState |= defSignalCON_State_AC_N_Relay;
    }
    else if (tmpNStat == SWITCH_OFF)
    {
        pCON->status.ulSignalState &= ~defSignalCON_State_AC_N_Relay;
    }
    /*********************/

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
 */
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
#ifdef DEBUG_DIAG_DUMMY_RELAY
            if (pEVSE->info.ucPhaseLine == 3)
            {
                pCON->status.ucRelayLState = SWITCH_OFF;
            }
            else
            {
                pCON->status.ucRelayLState = SWITCH_OFF;
                pCON->status.ucRelayNState = SWITCH_OFF;
            }
#else
            if (pEVSE->info.ucPhaseLine == 3)
            {
                POWER_L_OPEN();
            }
            else
            {
                POWER_L_OPEN();
                POWER_N_OPEN();
            }
#endif
        }
        else if(cmd == SWITCH_ON)
        {
#ifdef DEBUG_DIAG_DUMMY_RELAY
            if (pEVSE->info.ucPhaseLine == 3)
            {
                pCON->status.ucRelayLState = SWITCH_ON;
            }
            else
            {
                pCON->status.ucRelayLState = SWITCH_ON;
                pCON->status.ucRelayNState = SWITCH_ON;
            }
#else
            if (pEVSE->info.ucPhaseLine == 3)
            {
                POWER_L_CLOSE();
            }
            else
            {
                POWER_L_CLOSE();
                POWER_N_CLOSE();
            }
#endif
        }
    }
    else if(ucCONID == 1)
    {
        if (cmd == SWITCH_OFF)
        {
#ifdef DEBUG_DIAG_DUMMY_RELAY
            pCON->status.ucRelayNState = SWITCH_OFF;
#else
            POWER_N_OPEN();
#endif
        }
        else if (cmd == SWITCH_ON)
        {
#ifdef DEBUG_DIAG_DUMMY_RELAY
            pCON->status.ucRelayNState = SWITCH_ON;
#else
            POWER_N_CLOSE();
#endif
        }
    }
    /*********************/
    vTaskDelay(defRelayDelay);
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
    if (pCON->status.ucRelayLState == SWITCH_ON &&
            pCON->status.ucRelayNState == SWITCH_ON)
    {
        errcode = ERR_NO;
    }

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
    EventBits_t uxBits;
    ErrorCode_t errcode;

    pCON = (CON_t *)pvCON;
    ucCONID = pCON->info.ucCONID;
    errcode = ERR_NO;
    
    SetCPSwitch(pCON, SWITCH_OFF);
#ifdef DEBUG_DIAG_DUMMY
    pCON->status.xCPState = CP_12V;
#endif

    if (pCON->status.xCPState == CP_6V ||
       pCON->status.xCPState == CP_9V ||
       pCON->status.xCPState == CP_12V)
    {
        uxBits = xEventGroupWaitBits(pCON->status.xHandleEventCharge,
            defEventBitCONS2Opened,
            pdFALSE,
            pdTRUE,
            3000);//S1转换到12V后S2应在3s内断开，否则强制带载断电。
        //此处判断uxbits无意义，因为无论如何3s内或者3s外都要断电。
        if ((uxBits & defEventBitCONS2Opened) == defEventBitCONS2Opened)
        {
            printf_safe("S2 Opened OK\n");
        }
        else
        {
            printf_safe("S2 Opend timeout\n");
        }
    }
    else if (pCON->status.xCPState != CP_ERR) //PWM
    {
        //CP未转换;
        return ERR_CON_CP_SWITCH_FAULT;
    }
    else//CP_ERR
    {
        
    }
    errcode = SetRelay(pvCON, SWITCH_OFF);
#ifdef DEBUG_DIAG_DUMMY
    pCON->status.ucRelayLState = SWITCH_OFF;
    pCON->status.ucRelayNState = SWITCH_OFF;
#endif
    if (pCON->status.ucRelayLState == SWITCH_OFF &&
        pCON->status.ucRelayNState == SWITCH_OFF)
    {
        errcode = ERR_NO;
    }
    else
    {
        errcode = ERR_RELAY_PASTE;
    }
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
    vEventGroupDelete(pCON->status.xHandleEventOrder);
    vEventGroupDelete(pCON->status.xHandleEventException);
    vEventGroupDelete(pCON->status.xHandleEventTimerCBNotify);
    vEventGroupDelete(pCON->status.xHandleEventDiag);
    xTimerDelete(pCON->status.xHandleTimerRTData, 100);
    xTimerDelete(pCON->OrderTmp.xHandleTimerOrderTmp, 100);
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
    memset(pCON, 0, sizeof(CON_t));
   
    pCON->info.ucCONID = ucCONID;
    pCON->status.ucLoadPercent = 100;
    
    pCON->info.GetCONCfg = GetCONCfg;
    pCON->info.SetCONCfg = SetCONCfg;
    
    pCON->status.xHandleEventCharge    = xEventGroupCreate();
    pCON->status.xHandleEventOrder     = xEventGroupCreate();
    pCON->status.xHandleEventException = xEventGroupCreate();
    pCON->status.xHandleEventTimerCBNotify = xEventGroupCreate();
    pCON->status.xHandleEventDiag      = xEventGroupCreate();
    pCON->status.xHandleTimerVolt      = NULL;
    pCON->status.xHandleTimerCurr      = NULL;
    pCON->status.xHandleTimerFreq      = NULL;
    pCON->status.xHandleTimerCharge    = NULL;
    pCON->status.xHandleTimerRTData    = NULL;

    pCON->status.xVoltStat             = STATE_VOLT_OK;
    pCON->status.xCurrStat             = STATE_CURR_INIT;
    pCON->status.xFreqStat             = STATE_FREQ_OK;

    pCON->status.GetChargingData     = GetChargingData;
    pCON->status.GetCPState          = GetCPState;
    pCON->status.SetCPSwitch         = SetCPSwitch;
    pCON->status.SetLoadPercent      = SetLoadPercent;
    pCON->status.GetCCState          = GetCCState;
    pCON->status.GetPlugState        = GetPlugState;
    pCON->status.GetBTypeSocketLock  = GetBTypeSocketLock;
    pCON->status.SetBTypeSocketLock  = SetBTypeSocketLock;
    pCON->status.GetACLTemp          = GetACLTemp;
    pCON->status.GetACNTemp          = GetACNTemp;
    pCON->status.GetBTypeSocketTemp1 = GetBTypeSocketTemp1;
    pCON->status.GetBTypeSocketTemp2 = GetBTypeSocketTemp2;
    pCON->status.StartCharge         = StartCharge;
    pCON->status.StopCharge          = StopCharge;
    pCON->status.GetRelayState       = GetRelayState;
    pCON->status.SetRelay            = SetRelay;

    pCON->state = STATE_CON_IDLE;

    pCON->status.xHandleTimerRTData = xTimerCreate("TimerRemoteRTData",
                                      defRemoteRTDataCyc,
                                      pdTRUE,
                                      (void *)(int)ucCONID,
                                      vRemoteRTDataTimerCB);
    
    LineInit(&pCON->line[defLineA], defLineA);
    LineInit(&pCON->line[defLineB], defLineB);
    LineInit(&pCON->line[defLineC], defLineC);
    LineInit(&pCON->line[defLineN], defLineN);
    
    //order init
    OrderInit(&(pCON->order));
    //OrderTmp init
    OrderInit(&(pCON->OrderTmp.order));
    pCON->OrderTmp.ucCheckOrderTmp = 0;
    sprintf(pCON->OrderTmp.strOrderTmpPath, "%sOrderCON%d.tmp", pathSystemDir, pCON->info.ucCONID);
    pCON->OrderTmp.xHandleTimerOrderTmp = xTimerCreate("TimerOrderTmp",
                                                    defOrderTmpCyc,
                                                    pdTRUE,
                                                    (void *)(int)(pCON->info.ucCONID),
                                                    vOrderTmpTimerCB);

    return pCON;
}
