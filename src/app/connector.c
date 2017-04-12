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

/*---------------------------------------------------------------------------/
/                               从文件获取充电接口信息
/---------------------------------------------------------------------------*/

/** @todo (rgw#1#): 增加枪充电类型CONType */

static ErrorCode_t GetSocketType(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	uint8_t tmpType;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpType = defSocketTypeB;
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...
	tmpType = GET_CC1;

	/*********************/

	pCON->info.ucSocketType = tmpType;

	return  errcode;
}
static ErrorCode_t GetVolatageUpperLimits(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpVoltUpLim;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpVoltUpLim = 250;//(V)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...

	/*********************/

	pCON->info.dVolatageUpperLimits = tmpVoltUpLim;

	return  errcode;
}
static ErrorCode_t GetVolatageLowerLimits(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpVoltLowLim;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpVoltLowLim = 190;//(V)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...


	/*********************/

	pCON->info.dVolatageLowerLimits = tmpVoltLowLim;

	return  errcode;
}
static ErrorCode_t GetACTempUpperLimits(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpACTempUpperLim;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpACTempUpperLim = 105;//(℃)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

    //..

	/*********************/

	pCON->info.dACTempUpperLimits = tmpACTempUpperLim;

	return  errcode;
}
static ErrorCode_t GetACTempLowerLimits(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpACTempLowerLim;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpACTempLowerLim = -40;//(℃)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...
	tmpACTempLowerLim = Sys_samp.DC.TEMP1;

	/*********************/

	pCON->info.dACTempLowerLimits = tmpACTempLowerLim;

	return  errcode;
}
static ErrorCode_t GetSocketTempUpperLimits(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpSocketTempUpperLim;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpSocketTempUpperLim = 105;//(℃)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...

	/*********************/

	pCON->info.dSocketTempUpperLimits = tmpSocketTempUpperLim;

	return  errcode;
}
static ErrorCode_t GetSocketTempLowerLimits(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpSocketTempLowerLim;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpSocketTempLowerLim = -40;//(℃)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...

	/*********************/

	pCON->info.dSocketTempLowerLimits = tmpSocketTempLowerLim;

	return  errcode;
}
static ErrorCode_t GetRatedCurrent(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpCurr;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpCurr = 32;//(A)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...

	/*********************/

	pCON->info.dRatedCurrent = tmpCurr;

	return  errcode;
}
static ErrorCode_t GetRatedPower(void *pvCON)
{
	CON_t *pCON;
	uint8_t ucCONID;
	double tmpPow;
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	tmpPow = 7;//(kW)
	errcode = ERR_NO;

	/** @todo (rgw#1#): 从文件获取 */

	//...

	/*********************/

	pCON->info.dRatedPower = tmpPow;

	return  errcode;
}

/*---------------------------------------------------------------------------/
/                               从驱动获取充电接口状态
/---------------------------------------------------------------------------*/


/** @note (rgw#1#): 注意不同ID对硬件的不同操作 */

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

	/** @todo (rgw#1#): 获取电能表电压 */

	//...

	if(ucCONID == 0)
	{
		tmpVolt=get_va();
	}
	if(ucCONID == 1)
	{
		tmpVolt=get_va();
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

	/** @todo (rgw#1#): 获取电能表电流 */

	//...
	if(ucCONID == 0)
	{
		tmpCurr=get_ia();
	}
	if(ucCONID == 1)
	{
		tmpCurr=get_ia();
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

	/** @todo (rgw#1#): 从电表获取 */

	tmpFreq = Electricity_meter[ucCONID].massage.massage_frequency;

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

	/** @todo (rgw#1#): 从电表获取 */

		tmpPower = Electricity_meter[ucCONID].massage.massage_electric_energy;

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

	/** @todo (rgw#1#):  */

	//...

	if(ucCONID==0)
	{
		cp1=get_CP1();
		if((cp1<12.8f)&&(cp1>11.2f))
		{
			 if(TIM2->CCR1!=1000)
            {
                 tmpCPState = CP_12V_PWM;
            }
            else
            {
                 tmpCPState = CP_12V;
            }
		}
		else if((cp1<9.8f)&&(cp1>8.2f))
		{
		    if(TIM2->CCR1!=1000)
            {
                 tmpCPState = CP_9V_PWM;
            }
            else
            {
                 tmpCPState = CP_9V;
            }

		}
		else if((cp1<6.8f)&&(cp1>5.2f))
		{
			if(TIM2->CCR1!=1000)
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
	}
	else if(ucCONID==1)
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

	/** @todo (rgw#1#):  */

	//...
	if(ucCONID==0)
    {
        if(cmd==SWITCH_ON)
	{
		PWM1_535;
	}
	else
	{
		PWM1_1000;
	}
    }
    else if(ucCONID==1)
    {


        if(cmd==SWITCH_ON)
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

	/** @todo (rgw#1#):  */
	if(ucCONID==0)
	{
		TIM2->CCR1=ucLoadPercent*10;
	}
	else if(ucCONID==1)
	{
		TIM4->CCR1=ucLoadPercent*10;
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

	/** @todo (rgw#1#):  */

	//...
	if(ucCONID==0)
	{
	    if(GET_CC1==0)//已经连接CC1点，PE连接正常
        {
        tmpCCState=CC_PE;
        }
        else if(GET_CC1==1)
        {
        tmpCCState=CC_NO;
        }

	}
	else if(ucCONID==1)
	{
            if(GET_CC2==0)//已经连接CC1点，PE连接正常
        {
        tmpCCState=CC_PE;
        }
        else if(GET_CC2==1)
        {
        tmpCCState=CC_NO;
        }
	}
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
		THROW_ERROR(ucCONID, GetCCState(pvCON), ERR_LEVEL_CRITICAL);
		THROW_ERROR(ucCONID, GetCPState(pvCON), ERR_LEVEL_CRITICAL);
		if(pCON->status.xCCState == CC_PE && pCON->status.xCPState != CP_12V && pCON->status.xCPState != CP_ERR)
		{
			tmpPlugState = PLUG;
		}
	}
	else if(pCON->info.ucSocketType == defSocketTypeC)
	{
		THROW_ERROR(ucCONID, GetCPState(pvCON), ERR_LEVEL_CRITICAL);
		if(pCON->status.xCPState != CP_12V && pCON->status.xCPState != CP_ERR)
		{
			tmpPlugState = PLUG;
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
	tmpLockState = LOCK;/** @fixme (rgw#1#): 完成驱动后需要修改为unlock */
	errcode = ERR_NO;

	/** @todo (rgw#1#):  */

	if(ucCONID=0)
	{
		tmpLockState=GET_GUN_STATE_1;
	}
	else if(ucCONID==1)
	{
		tmpLockState=GET_GUN_STATE_2;
	}
	/*********************/


	pCON->status.xBTypeSocketLockState = tmpLockState;

	return errcode;
}

/** @brief
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

	if(ucCONID==0)
	{
		/** @todo (rgw#1#): 执行锁止动作 */

		if(cmd==SWITCH_ON)
		{
			Close_gun_1();
		}
		else
		{
			Open_gun_1();
		}

	}
	else if(ucCONID==1)
	{
		if(cmd==SWITCH_ON)
		{
			Close_gun_2();
		}
		else
		{
			Open_gun_2();
		}

	}
	THROW_ERROR(pCON->info.ucCONID, GetBTypeSocketLock(pCON), ERR_LEVEL_WARNING);

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

	/** @todo (rgw#1#):  */
	if(ucCONID==0)
	{
		tmpACLTemp=Sys_samp.DC.TEMP1;
	}
	else if(ucCONID==1)
	{
		tmpACLTemp=Sys_samp.DC.TEMP2;
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

	/** @todo (rgw#1#):  */

	//...
	if(ucCONID==0)
	{
		tmpACNTemp=Sys_samp.DC.TEMP3;
	}
	else if(ucCONID==1)
	{
		tmpACNTemp=Sys_samp.DC.TEMP4;
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

	/** @todo (rgw#1#):  */

	//...
	if(ucCONID ==0)
	{
		tmpTemp=Sys_samp.DC.TEMP_ARM1;
	}
	else if(ucCONID ==1)
	{
		tmpTemp=Sys_samp.DC.TEMP_ARM2;
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

	/** @todo (rgw#1#):  */

	//...
	if(ucCONID==0)
	{
		tmpTemp=Sys_samp.DC.TEMP_ARM3;
	}
	else if(ucCONID==1)
	{
		tmpTemp=Sys_samp.DC.TEMP_ARM4;
	}

	/*********************/

	pCON->status.dBTypeSocketTemp2 = tmpTemp;

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
	/** @todo (rgw#1#): 操作输出继电器，保存继电器状态 */

	if(ucCONID==0)
	{
		POWER_L_CLOSE();
		POWER_N_CLOSE();
	}
	else if(ucCONID==1)
	{
		;
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
	ErrorCode_t errcode;

	pCON = (CON_t *)pvCON;
	ucCONID = pCON->info.ucCONID;
	errcode = ERR_NO;

	/** @todo (rgw#1#): 操作输出继电器，保存继电器状态 */

	//...
	POWER_L_OPEN();
	POWER_N_OPEN();
	/*********************/
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

	/** @todo (rgw#1#):  */
	read_pca9554_2();
	tmpLStat=(read_pca9554_2()>>6)&&0x01;
	tmpNStat=tmpLStat;
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


	/** @todo (rgw#1#): 操作K1,K2输出继电器 */
	if(ucCONID==0)
	{
		if(cmd == SWITCH_OFF)
		{
			//...
			POWER_L_CLOSE();
			POWER_N_CLOSE();
		}
		else
		{
			POWER_L_OPEN();
			POWER_N_OPEN();
		}
	}
	else if(ucCONID==1)
	{
		;
	}
	/*********************/

	THROW_ERROR(pCON->info.ucCONID, GetRelayState(pCON), ERR_LEVEL_CRITICAL);
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
	if(pCON->order.pChargeSegment != NULL)
	{
		pCON->order.pChargeSegment->Delete(pCON->order.pChargeSegment);
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
	pCON->info.ucCONType = 0;
	pCON->info.ucSocketType = defSocketTypeB;
	pCON->info.dVolatageUpperLimits = 0;
	pCON->info.dVolatageLowerLimits = 0;
	pCON->info.dACTempUpperLimits = 0;
	pCON->info.dACTempLowerLimits = 0;
	pCON->info.dSocketTempUpperLimits = 0;
	pCON->info.dSocketTempLowerLimits = 0;
	pCON->info.dRatedCurrent = 0;
	pCON->info.dRatedPower = 0;

	pCON->info.GetSocketType = GetSocketType;
	pCON->info.GetVolatageUpperLimits = GetVolatageUpperLimits;
	pCON->info.GetVolatageLowerLimits = GetVolatageLowerLimits;
	pCON->info.GetACTempUpperLimits = GetACTempUpperLimits;
	pCON->info.GetACTempLowerLimits = GetACTempLowerLimits;
	pCON->info.GetSocketTempUpperLimits = GetSocketTempUpperLimits;
	pCON->info.GetSocketTempLowerLimits = GetSocketTempLowerLimits;
	pCON->info.GetRatedCurrent = GetRatedCurrent;
	pCON->info.GetRatedPower = GetRatedPower;

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

	pCON->order.pChargeSegment = NULL;

	OrderInit(&(pCON->order));

	return pCON;
}
