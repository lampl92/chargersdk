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
#include "ST_LIS2DH12.h"

uint8_t isEVSEStandby(void)
{
    int id;
    uint32_t ulTotalCON;
    uint32_t ulTotalCONStandby = 0;
    CON_t *pCON;
    
    
    ulTotalCON = pListCON->Total;
    
    for (id = 0; id < ulTotalCON; id++)
    {
        pCON = CONGetHandle(id);
        if ((pCON->status.ulSignalState & defSignalCON_State_Standby) == defSignalCON_State_Standby)
        {
            ulTotalCONStandby++;
        }
    }
    //所有standby，evse才standby
    if (ulTotalCONStandby == ulTotalCON)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
uint8_t isEVSEWorking(void)
{
    int id;
    uint32_t ulTotalCON;
    CON_t *pCON;
    
    ulTotalCON = pListCON->Total;
    
    for (id = 0; id < ulTotalCON; id++)
    {
        pCON = CONGetHandle(id);
        //有一个working，evse就是working
        if ((pCON->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
        {
            return 1;
        }
    }
    return 0;
}

#if 0
static int SetSignalPool(void *pvDev, uint32_t block, uint32_t bit)
{
    EVSE_t *pEVSE;
    
    pEVSE = (EVSE_t *)pvDev;
    if (block >= EVSE_MAX_SIGNAL_BLOCK)
    {
        while (1)
            ;
    }
    pEVSE->status.ulSignalPool[block] |= bit;
    
    return 1;
}
static int ClrSignalPool(void *pvDev, uint32_t block, uint32_t bit)
{
    EVSE_t *pEVSE;
    
    pEVSE = (EVSE_t *)pvDev;
    if (block >= EVSE_MAX_SIGNAL_BLOCK)
    {
        while (1)
            ;
    }
    pEVSE->status.ulSignalPool[block] &= ~bit;
    
    return 1;
}
static int GetSignalPool(void *pvDev, uint32_t block, uint32_t bit)
{
    EVSE_t *pEVSE;
    
    pEVSE = (EVSE_t *)pvDev;
    if (block >= EVSE_MAX_SIGNAL_BLOCK)
    {
        while (1)
            ;
    }
    if ((pEVSE->status.ulSignalPool[block] & bit) == bit)
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
/*                               设置充电桩信息到配置文件                    */
/*---------------------------------------------------------------------------*/

static ErrorCode_t SetEVSECfg(void *pvEVSE, char *jnItemString, void *pvCfgParam, uint8_t type);

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
static ErrorCode_t SetEVSECfg(void *pvEVSE, char *jnItemString, void *pvCfgParam, uint8_t type)
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
                cJSON_ReplaceItemInObject(jsEVSECfgObj, jnItemString, cJSON_CreateString((char *)pvCfgParam));
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
/*--------------------------------------------------------------------------*/
/*                              从文件获取充电桩信息						*/
/*--------------------------------------------------------------------------*/
/** @brief 设备唯一序列号,和长度
 */

/** @brief 充电设备类型
 *         1：直流设备
 *         2：交流设备
 *         3：交直流一体设备
 *         4：无线设备
 *         5：其他
 *
 */
static ErrorCode_t GetEVSECfg(void *pvEVSE, void *pvCfgObj)
{
    EVSE_t *pEVSE;
    cJSON *jsCfgObj = NULL;
    ErrorCode_t errcode = ERR_NO;

    pEVSE = (EVSE_t *)pvEVSE;
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
    
    //cfgobj_get_string(jsCfgObj, pEVSE->info.strSN, "%s", jnEVSESN);
    cfgobj_get_string(jsCfgObj, pEVSE->info.strID, "%s", jnEVSEID);
    cfgobj_get_uint8(jsCfgObj, &pEVSE->info.ucType, "%s", jnEVSEType);
    cfgobj_get_uint8(jsCfgObj, &pEVSE->info.ucPhaseLine, "%s", jnPhaseLine);
    cfgobj_get_uint8(jsCfgObj, &pEVSE->info.ucTotalCON, "%s", jnTotalCON);
    cfgobj_get_double(jsCfgObj, &pEVSE->info.dLng, "%s", jnLng);
    cfgobj_get_double(jsCfgObj, &pEVSE->info.dLat, "%s", jnLat);
    cfgobj_get_uint8(jsCfgObj, &pEVSE->info.ucServiceFeeType, "%s", jnServiceFeeType);
    cfgobj_get_double(jsCfgObj, &pEVSE->info.dServiceFee, "%s", jnServiceFee);
    cfgobj_get_double(jsCfgObj, &pEVSE->info.dDefSegFee, "%s", jnDefSegFee);
    extern void Get_ChipID(int contype, int total_con);
    Get_ChipID(EVSE_CONFIG_CONType, pEVSE->info.ucTotalCON);
    sprintf(pEVSE->info.strSN, "%s", g_strChipID);
    
    cJSON_Delete(jsCfgObj);
    return errcode;
}
	
/*--------------------------------------------------------------------------*/
/*                              从驱动获取充电桩状态						*/
/*--------------------------------------------------------------------------*/

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
    tmpScramState = ~((uint8_t)(read_pca9554_2() >> 2)) & 0x01;
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
#ifndef DEBUG_DIAG_DUMMY
    pEVSE->status.dKnockAngle = 35;
#else
    pEVSE->status.dKnockAngle = get_angle_max();
#endif
    if (pEVSE->status.dKnockAngle > 90)
    {
        tmpKnockState = 1;
        errcode =  ERR_GSENSOR_FAULT;
    }
    else if (pEVSE->status.dKnockAngle <= 35)
    {
        tmpKnockState = 0;
    }
    else
    {
        tmpKnockState = 1;            
    }
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

#ifndef DEBUG_DIAG_DUMMY1
    tmpOffState = 0;
#else
//    if (Get_Power_Status == 1)
//    {
//        errcode =  ERR_POWEROFF_DECT_FAULT;
//    }
    //if (Get_Power_Status == 0)
    {
        if (Get_Power_Status == 0) //检测间隔10mS
        {
            tmpOffState = 1;
            printf_safe("Power Off!!!!\n");
            printf_safe("Power Off!!!!\n");
            printf_safe("Power Off!!!!\n");
            printf_safe("Power Off!!!!\n");
            printf_safe("Power Off!!!!\n");
        }
        else if(Get_Power_Status == 1)
        {
            tmpOffState = 0;
        }
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
    tmpArresterState = ((uint8_t)(read_pca9554_2() >> 3)) & 0x01;
#endif

    /*********************/

    pEVSE->status.ulArresterState = tmpArresterState;
    return errcode;
}

/** @brief A输入温度
 *
 * @param pvEVSE void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  
 *
 */
static ErrorCode_t GetAC_A_Temp_in(void *pvEVSE)
{
	EVSE_t *pEVSE;
	double tmpACTemp;
	ErrorCode_t errcode;

	pEVSE = (EVSE_t *)pvEVSE;
	tmpACTemp = -60;
	errcode = ERR_NO;

	    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
    tmpACTemp = 25;
#else
    tmpACTemp = (double)get_dc_massage(TEMP_L_IN); 
	if (tmpACTemp > 200 || tmpACTemp < -60)
	{
		errcode = ERR_EVSE_AC_A_TEMP_DECT_FAULT;
	}
#endif

	pEVSE->status.dAC_A_Temp_IN = tmpACTemp;

	return errcode;
}
/** @brief B输入温度
 *
 * @param pvEVSE void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  
 *
 */
static ErrorCode_t GetAC_B_Temp_in(void *pvEVSE)
{
	EVSE_t *pEVSE;
	double tmpACTemp;
	ErrorCode_t errcode;

	pEVSE = (EVSE_t *)pvEVSE;
	tmpACTemp = -60;
	errcode = ERR_NO;

	    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
    tmpACTemp = 25;
#else
	tmpACTemp = 0;
#endif

	pEVSE->status.dAC_B_Temp_IN = tmpACTemp;

	return errcode;
}
/** @brief C输入
 *
 * @param pvEVSE void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  
 *
 */
static ErrorCode_t GetAC_C_Temp_in(void *pvEVSE)
{
	EVSE_t *pEVSE;
	double tmpACTemp;
	ErrorCode_t errcode;

	pEVSE = (EVSE_t *)pvEVSE;
	tmpACTemp = -60;
	errcode = ERR_NO;

	    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
    tmpACTemp = 33;
#else
	tmpACTemp = 0;
#endif

	pEVSE->status.dAC_C_Temp_IN = tmpACTemp;

	return errcode;
}

/** @brief N输入温度
 *
 * @param pvEVSE void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  
 *
 */
static ErrorCode_t GetAC_N_Temp_in(void *pvEVSE)
{
	EVSE_t *pEVSE;
	double tmpACTemp;
	ErrorCode_t errcode;

	pEVSE = (EVSE_t *)pvEVSE;
	tmpACTemp = -60;
	errcode = ERR_NO;

	    /** 实现代码  */
#ifdef DEBUG_DIAG_DUMMY
    tmpACTemp = 25;
#else
    tmpACTemp = (double)get_dc_massage(TEMP_N_IN); 
	if (tmpACTemp > 200 || tmpACTemp < -60)
	{
		errcode = ERR_EVSE_AC_N_TEMP_DECT_FAULT;
	}
#endif

	pEVSE->status.dAC_N_Temp_IN = tmpACTemp;

	return errcode;
}

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));

    memset(pEVSE, 0, sizeof(EVSE_t));

    pEVSE->info.GetEVSECfg = GetEVSECfg;
    pEVSE->info.SetEVSECfg = SetEVSECfg;

    pEVSE->status.GetArresterState = GetArresterState;
    pEVSE->status.GetKnockState    = GetKnockState;
    pEVSE->status.GetPEState       = GetPEState;
    pEVSE->status.GetPowerOffState = GetPowerOffState;
    pEVSE->status.GetScramState    = GetScramState;
	pEVSE->status.GetAC_A_Temp_in  = GetAC_A_Temp_in;
	pEVSE->status.GetAC_B_Temp_in  = GetAC_B_Temp_in;
	pEVSE->status.GetAC_C_Temp_in  = GetAC_C_Temp_in;
	pEVSE->status.GetAC_N_Temp_in  = GetAC_N_Temp_in;

    return pEVSE;
}

static void CONInit(void)
{
    int i;
    static CON_t *pCON[defMaxCON];  //在堆中定义

	uint8_t str[17] = "2000000000000002";
    for (i = 0; i < defMaxCON; i++)
    {
        pCON[i] = NULL;
    }
    
    pListCON = UserListCreate();
//    double upp = 260;
//    double low = 176;
	double temp = 32;
    for(i = 0; i < pEVSE->info.ucTotalCON; i++)
    {
        pCON[i] = CONCreate(i);

        THROW_ERROR(i, pCON[i]->info.GetCONCfg(pCON[i], NULL), ERR_LEVEL_WARNING, "CONInit GetCONCfg");
//        pCON[i]->info .SetCONCfg (pCON[i], jnVolatageUpperLimits, &upp, ParamTypeDouble);
//        pCON[i]->info .SetCONCfg (pCON[i], jnVolatageLowerLimits, &low, ParamTypeDouble);
        //pCON[i]->info .SetCONCfg (pCON[i], jnRatedCurrent, &temp, ParamTypeDouble);
	    //pCON[i]->info.SetCONCfg(pCON[i], jnQRCode, str, ParamTypeString);

        pListCON->Add(pListCON, pCON[i]);
    }
}
void EVSEinit(void)
{
    char str[17] = "3000000000001018";
    char key[] = "0123456789abcder";
    uint8_t tmp = 2;
    double dtmp = 41;
    pEVSE = EVSECreate();
#if 0
//    tmp = 2;
//    cfg_set_uint8(pathEVSECfg, &tmp, "%s", jnTotalCON); 
//    tmp = 3;
//    cfg_set_uint8(pathEVSECfg, &tmp, "%s", jnPhaseLine); 
//    tmp = 1;
//    cfg_set_uint8(pathNetCfg, &tmp, "%s", jnNetAdapter);//网络类型 
//    ifconfig.info.ucAdapterSel = 1;
//    xSysconf.xModule.use_gprs = 2;
//    cfg_set_uint8(pathNetCfg, &ifconfig.info.ucAdapterSel, "%s", jnNetAdapter);
//    cfg_set_uint8(pathSysCfg, &xSysconf.xModule.use_gprs, "%s", jnSysUSE_GPRSModem);
    
//    dtmp = 41;
//    cfg_set_double(pathEVSECfg, &dtmp, "%s:%d.%s", jnCONArray, 0, jnRatedPower);
//    cfg_set_double(pathEVSECfg, &dtmp, "%s:%d.%s", jnCONArray, 1, jnRatedPower);
//    
//    cfg_set_string(pathEVSECfg, str, "%s", jnEVSEID);    
//    cfg_set_string(pathEVSECfg, str, "%s:0.%s", jnCONArray, jnQRCode);    
//    cfg_set_string(pathProtoCfg, key, "%s", jnProtoKey);    
//    cfg_set_uint8(pathEVSECfg, &tmp, "%s", jnTotalCON);    
    
//    char tmpip[] = "v.daizc.com";
//    uint16_t tmport = 6999;
//    cfg_set_string(pathProtoCfg, tmpip, "%s", jnProtoServerIP);
//    cfg_set_uint16(pathProtoCfg, &tmport, "%s", jnProtoServerPort);
#endif
    THROW_ERROR(defDevID_File, pEVSE->info.GetEVSECfg(pEVSE, NULL), ERR_LEVEL_WARNING, "EVSEinit GetEVSECfg");
    CONInit();
    
    meter = meter_init(pEVSE->info.ucPhaseLine);

    pRFIDDev = RFIDDevCreate(RFID_UARTx, RFID_UART_BAND, RFID_UART_DATA, RFID_UART_PARI, RFID_UART_STOP);

    pechProto = EchProtocolCreate();
    THROW_ERROR(defDevID_File, pechProto->info.GetProtoCfg(pechProto, NULL), ERR_LEVEL_WARNING, "EVSEinit GetProtoCfg");
    THROW_ERROR(defDevID_File, pechProto->info.ftp.GetFtpCfg((void *)&(pechProto->info.ftp), NULL), ERR_LEVEL_WARNING, "EVSEinit GetFtpCfg");
}
