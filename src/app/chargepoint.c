/**
* @file chargepoint.c
* @brief ������С��絥Ԫ��Ҫ�����������
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
#include <string.h>
#include "evse_globals.h"
#include "chargepoint.h"


/*---------------------------------------------------------------------------/
/                               ���ļ���ȡ���ӿ���Ϣ
/---------------------------------------------------------------------------*/


static ErrorCode_t GetConnectorType(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint8_t tmpType;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpType = defConnectorTypeB;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.ucConnectorType = tmpType;

    return  errcode;
}
static ErrorCode_t GetVolatageUpperLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpVoltUpLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpVoltUpLim = 250;//(V)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dVolatageUpperLimits = tmpVoltUpLim;

    return  errcode;
}
static ErrorCode_t GetVolatageLowerLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpVoltLowLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpVoltLowLim = 190;//(V)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dVolatageLowerLimits = tmpVoltLowLim;

    return  errcode;
}
static ErrorCode_t GetACTempUpperLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpACTempUpperLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpACTempUpperLim = 105;//(��)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dACTempUpperLimits = tmpACTempUpperLim;

    return  errcode;
}
static ErrorCode_t GetACTempLowerLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpACTempLowerLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpACTempLowerLim = -40;//(��)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dACTempLowerLimits = tmpACTempLowerLim;

    return  errcode;
}
static ErrorCode_t GetConnectorTempUpperLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpConnectorTempUpperLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpConnectorTempUpperLim = 105;//(��)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dConnectorTempUpperLimits = tmpConnectorTempUpperLim;

    return  errcode;
}
static ErrorCode_t GetConnectorTempLowerLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpConnectorTempLowerLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpConnectorTempLowerLim = -40;//(��)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dConnectorTempLowerLimits = tmpConnectorTempLowerLim;

    return  errcode;
}
static ErrorCode_t GetRatedCurrent(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpCurr;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpCurr = 32;//(A)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dRatedCurrent = tmpCurr;

    return  errcode;
}
static ErrorCode_t GetRatedPower(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpPow;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpPow = 7;//(kW)
    errcode = ERR_NO;

    /** @todo (rgw#1#): ���ļ���ȡ */

    //...

    /*********************/

    pPoint->info.dRatedPower = tmpPow;

    return  errcode;
}

/*---------------------------------------------------------------------------/
/                               ��������ȡ���ӿ�״̬
/---------------------------------------------------------------------------*/


/** @note (rgw#1#): ע�ⲻͬID��Ӳ���Ĳ�ͬ���� */

/** @brief ��ȡ����ѹ����⾫�� +/-0.1V
 *
 * @param pPoint ChargePoint_t*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_METER_FAULT
 *
 */
static ErrorCode_t GetChargingVoltage(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpVolt;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpVolt = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ��ȡ���ܱ��ѹ */

    //...
    if(ucPointID == 0)
    {
        tmpVolt = 220;
    }
    if(ucPointID == 1)
    {
        tmpVolt = 110;
    }

    /*********************/

    pPoint->status.dChargingVoltage = tmpVolt;

    return errcode;
}

/** @brief ��ȡ����������⾫��+/-0.1A
 *
 * @param pPoint ChargePoint_t*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_METER_FAULT
 *
 */
static ErrorCode_t GetChargingCurrent(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpCurr;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpCurr = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ��ȡ���ܱ���� */

    //...
    if(ucPointID == 0)
    {
        tmpCurr = 32;
    }
    if(ucPointID == 1)
    {
        tmpCurr = 34;
    }

    /*********************/

    pPoint->status.dChargingCurrent = tmpCurr;

    return errcode;
}

/** @brief ��ȡ��ԴƵ��
 *
 * @param pPoint ChargePoint_t*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_METER_FAULT
 *
 */
static ErrorCode_t GetChargingFrequence(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpFreq;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpFreq = 50;
    errcode = ERR_NO;

    /** @todo (rgw#1#): �ӵ���ȡ */

    //...

    /*********************/

    pPoint->status.dChargingFrequence = tmpFreq;

    return errcode;
}

/** @brief ��ȡCP״̬
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CP_FAULT
 *
 */
static ErrorCode_t GetCPState(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpCPState;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpCPState = CP_12V;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.xCPState = tmpCPState;

    return errcode;
}
/** @brief ����S1����
 *
 * @param pvPoint void*
 * @param cmd uint8_t   ���ݿ��ؿ������SWITCH_ON /SWITCH_OFF
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetCPSwitch(void *pvPoint, uint8_t cmd)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    return errcode;
}
/** @brief ����PWMռ�ձ� �����뿴18487.1-2015 P22
 *
 * @param pvPoint void*
 * @param ucLoadPercent uint8_t ���ذٷֱ�
 *                              ����100%ʱ��PWM=53
                                ����50%ʱ��PWM= 27
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetLoadPercent(void *pvPoint, uint8_t ucLoadPercent)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint8_t tmpCPPWM;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpCPPWM = 53;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //PWM

    /*********************/

    pPoint->status.ucLoadPercent = ucLoadPercent;

    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CC_FAULT
 *
 */
static ErrorCode_t GetCCState(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpCCState;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpCCState = CC_NO;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.xCCState = tmpCCState;

    return errcode;
}

/** @brief ��ȡ��ǹ״̬��Ӧͬʱ������1��CC���ͼ���4��CP��
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_PLUG_FAULT
 *
 */
static ErrorCode_t GetPlugState(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint32_t tmpPlugState; //    PLUG,  UNPLUG,
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpPlugState = UNPLUG;
    errcode = ERR_NO;

    /** ����ǹ״̬�����ӿ�  */

    if(pPoint->info.ucConnectorType == defConnectorTypeB)
    {
        THROW_ERROR(ucPointID, GetCCState(pvPoint), ERR_LEVEL_CRITICAL);
        THROW_ERROR(ucPointID, GetCPState(pvPoint), ERR_LEVEL_CRITICAL);
        if(pPoint->status.xCCState == CC_PE && pPoint->status.xCPState != CP_12V && pPoint->status.xCPState != CP_ERR)
        {
            tmpPlugState = PLUG;
        }
    }
    else if(pPoint->info.ucConnectorType == defConnectorTypeC)
    {
        THROW_ERROR(ucPointID, GetCPState(pvPoint), ERR_LEVEL_CRITICAL);
        if(pPoint->status.xCPState != CP_12V && pPoint->status.xCPState != CP_ERR)
        {
            tmpPlugState = PLUG;
        }
    }

    /*********************/

    pPoint->status.xPlugState = tmpPlugState;

    return errcode;
}

/** @brief B������ǹ��״̬
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_B_LOCK_FAULT
 *
 */
static ErrorCode_t GetBTypeConnectorLock(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpLockState; //lock unlock
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpLockState = LOCK;/** @fixme (rgw#1#): �����������Ҫ�޸�Ϊunlock */
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/


    pPoint->status.xBTypeConnectorLockState = tmpLockState;

    return errcode;
}

/** @brief
 *
 * @param pvPoint void*
 * @param cmd uint8_t   ���ؿ��ƣ�SWITCH_ON /SWITCH_OFF
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_CANT_LOCK
 *
 */
static ErrorCode_t SetBTypeConnectorLock(void *pvPoint, uint8_t cmd)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpLockState;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpLockState = UNLOCK;
    errcode = ERR_NO;

    if(pPoint->status.xBTypeConnectorLockState == UNLOCK)
    {
        /** @todo (rgw#1#): ִ����ֹ���� */

        //...

        /*********************/

    }
    THROW_ERROR(pPoint->info.ucChargePointID, GetBTypeConnectorLock(pPoint), ERR_LEVEL_WARNING);

    return errcode;
}

/** @brief ��ȡL�����¶�
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_ACLTEMP_DECT_FAULT
 *
 */
static ErrorCode_t GetACLTemp(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double  tmpACLTemp;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpACLTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.dACLTemp = tmpACLTemp;

    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_ACNTEMP_DECT_FAULT
 *
 */
static ErrorCode_t GetACNTemp(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpACNTemp;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpACNTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.dACNTemp = tmpACNTemp;

    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_BTEMP1_DECT_FAULT
 *
 */
static ErrorCode_t GetBTypeConnectorTemp1(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpTemp;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.dBTypeConnectorTemp1 = tmpTemp;

    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_BTEMP2_DECT_FAULT
 */
static ErrorCode_t GetBTypeConnectorTemp2(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    double tmpTemp;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.dBTypeConnectorTemp2 = tmpTemp;

    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_STARTCHARGE
 *
 */
static ErrorCode_t StartCharge(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpChargeStatus;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    errcode = ERR_NO;
    /** @todo (rgw#1#): ��������̵���������̵���״̬ */

    //...

    /*********************/
    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *                  ERR_NO
 *                  ERR_STOPCHARGE
 *
 */
static ErrorCode_t StopCharge(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpChargeStatus;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ��������̵���������̵���״̬ */

    //...

    /*********************/
    return errcode;
}
/** @brief ��ȡ����̵���״̬
 *
 * @param pvPoint void*
 * @return ErrorCode_t
 *
 */
static ErrorCode_t GetRelayState(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint8_t tmpLStat;
    uint8_t tmpNStat;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpLStat = SWITCH_OFF;
    tmpNStat = SWITCH_OFF;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    //...

    /*********************/

    pPoint->status.ucRelayLState = tmpLStat;
    pPoint->status.ucRelayNState = tmpNStat;

    return errcode;
}
/** @brief
 *
 * @param pvPoint void*
 * @param cmd uint8_t SWITCH_ON SWITCH_OFF
 * @return ErrorCode_t
 *
 */
static ErrorCode_t SetRelay(void *pvPoint, uint8_t cmd)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;

    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    errcode = ERR_NO;


        /** @todo (rgw#1#): ����K1,K2����̵��� */

        if(cmd == SWITCH_OFF)
        {
        //...
        }
        else if(cmd == SWITCH_ON)
        {
        //...
        }

        /*********************/

    THROW_ERROR(pPoint->info.ucChargePointID, GetRelayState(pPoint), ERR_LEVEL_CRITICAL);
    return errcode;
}
ChargePoint_t *ChargePointGetHandle(uint8_t ucChargePointID)
{
    ChargePoint_t *pPoint;

    pPoint =  (ChargePoint_t *)(pListChargePoint->pListPointArray[ucChargePointID]);

    return pPoint;
}
ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID )
{
    ChargePoint_t *pChargePoint;
    pChargePoint = (ChargePoint_t *)malloc(sizeof(ChargePoint_t));
    if(pChargePoint == NULL)
    {
        return NULL;
    }
    pChargePoint->info.ucChargePointID = ucChargePointID;
    pChargePoint->info.ucConnectorType = defConnectorTypeB;
    pChargePoint->info.dVolatageUpperLimits = 0;
    pChargePoint->info.dVolatageLowerLimits = 0;
    pChargePoint->info.dACTempUpperLimits = 0;
    pChargePoint->info.dACTempLowerLimits = 0;
    pChargePoint->info.dConnectorTempUpperLimits = 0;
    pChargePoint->info.dConnectorTempLowerLimits = 0;
    pChargePoint->info.dRatedCurrent = 0;
    pChargePoint->info.dRatedPower = 0;

    pChargePoint->info.GetConnectorType = GetConnectorType;
    pChargePoint->info.GetVolatageUpperLimits = GetVolatageUpperLimits;
    pChargePoint->info.GetVolatageLowerLimits = GetVolatageLowerLimits;
    pChargePoint->info.GetACTempUpperLimits = GetACTempUpperLimits;
    pChargePoint->info.GetACTempLowerLimits = GetACTempLowerLimits;
    pChargePoint->info.GetConnectorTempUpperLimits = GetConnectorTempUpperLimits;
    pChargePoint->info.GetConnectorTempLowerLimits = GetConnectorTempLowerLimits;
    pChargePoint->info.GetRatedCurrent = GetRatedCurrent;
    pChargePoint->info.GetRatedPower = GetRatedPower;

    memset(pChargePoint->status.ucHeldCardUID, 0, defUIDLength);
    pChargePoint->status.dACLTemp = 0;
    pChargePoint->status.dACNTemp = 0;
    pChargePoint->status.dBTypeConnectorTemp1 = 0;
    pChargePoint->status.dBTypeConnectorTemp2 = 0;
    pChargePoint->status.dChargingCurrent = 0;
    pChargePoint->status.dChargingFrequence = 0;
    pChargePoint->status.dChargingVoltage = 0;
    pChargePoint->status.xBTypeConnectorLockState = 0;
    pChargePoint->status.xCCState = 0;
    pChargePoint->status.xCPState = 0;
    pChargePoint->status.ucLoadPercent = 100;//(%)
    pChargePoint->status.xPlugState = 0;
    pChargePoint->status.xHandleEventCharge = xEventGroupCreate();
    pChargePoint->status.xHandleEventException = xEventGroupCreate();
    pChargePoint->status.xHandleTimerVolt = NULL;
    pChargePoint->status.xHandleTimerCurr = NULL;
    pChargePoint->status.xHandleTimerCharge = NULL;
    pChargePoint->status.GetChargingVoltage = GetChargingVoltage;
    pChargePoint->status.GetChargingCurrent = GetChargingCurrent;
    pChargePoint->status.GetChargingFrequence = GetChargingFrequence;
    pChargePoint->status.xVoltStat = STATE_VOLT_OK;
    pChargePoint->status.xCurrStat = STATE_CURR_INIT;

    pChargePoint->status.GetCPState = GetCPState;
    pChargePoint->status.SetCPSwitch = SetCPSwitch;
    pChargePoint->status.SetLoadPercent = SetLoadPercent;
    pChargePoint->status.GetCCState = GetCCState;
    pChargePoint->status.GetPlugState = GetPlugState;
    pChargePoint->status.GetBTypeConnectorLock = GetBTypeConnectorLock;
    pChargePoint->status.SetBTypeConnectorLock = SetBTypeConnectorLock;
    pChargePoint->status.GetACLTemp = GetACLTemp;
    pChargePoint->status.GetACNTemp = GetACNTemp;
    pChargePoint->status.GetBTypeConnectorTemp1 = GetBTypeConnectorTemp1;
    pChargePoint->status.GetBTypeConnectorTemp2 = GetBTypeConnectorTemp2;
    pChargePoint->status.StartCharge = StartCharge;
    pChargePoint->status.StopCharge = StopCharge;
    pChargePoint->status.GetRelayState = GetRelayState;
    pChargePoint->status.SetRelay = SetRelay;

    pChargePoint->state = STATE_POINT_IDLE;
    return pChargePoint;
}
