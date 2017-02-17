/**
* @file chargepoint.c
* @brief ������С��絥Ԫ��Ҫ�����������
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
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
    tmpType = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

    pPoint->info.ucConnectorType = tmpType;

    return  errcode;
}
static ErrorCode_t GetVolatageUpperLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint32_t tmpVoltUpLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpVoltUpLim = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

    pPoint->info.ulVolatageUpperLimits = tmpVoltUpLim;

    return  errcode;
}
static ErrorCode_t GetVolatageLowerLimits(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint32_t tmpVoltLowLim;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpVoltLowLim = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

    pPoint->info.ulVolatageLowerLimits = tmpVoltLowLim;

    return  errcode;
}
static ErrorCode_t GetRatedCurrent(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint32_t tmpCurr;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpCurr = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

    pPoint->info.ulRatedCurrent = tmpCurr;

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
    tmpPow = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#): ���ļ���ȡ */

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

    pPoint->state.dChargingVoltage = tmpVolt;

    return errcode;
}

/** @brief ��ȡ����������⾫��+/-0.1A
 *
 * @param pPoint ChargePoint_t*
 * @return ErrorCode_t
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

    pPoint->state.dChargingCurrent = tmpCurr;

    return errcode;
}

/** @brief ��ȡ��ԴƵ��
 *
 * @param pPoint ChargePoint_t*
 * @return ErrorCode_t
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
    tmpFreq = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#): �ӵ���ȡ */

    pPoint->state.dChargingFrequence = tmpFreq;

    return errcode;
}

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

    pPoint->state.xCPState = tmpCPState;

    return errcode;
}
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

    pPoint->state.xCCState = tmpCCState;

    return errcode;
}

/** @brief ��ȡ��ǹ״̬��Ӧͬʱ������1��CC���ͼ���4��CP��
 *
 * @param pPoint ChargePoint_t*
 * @return uint32_t 0 �޲�ǹ
 *                  1 �в�ǹ
 *
 */
static ErrorCode_t GetPlugState(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    uint32_t tmpPlugState;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpPlugState = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.xPlugState = tmpPlugState;

    return errcode;
}

static ErrorCode_t GetBTypeConnectorLock(void *pvPoint) //lock unlock
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpLockState;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpLockState = LOCK;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.xBTypeConnectorLockState = tmpLockState;

    return errcode;
}

static ErrorCode_t SetBTypeConnectorLock(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpLockState;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    tmpLockState = UNLOCK;
    errcode = ERR_NO;

    if(pPoint->state.xBTypeConnectorLockState == UNLOCK)
    {
        /** @todo (rgw#1#): ִ����ֹ���� */
    }
    THROW_ERROR(GetBTypeConnectorLock(pPoint), ERR_LEVEL_WARNING);

    return errcode;
}

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

    pPoint->state.dACLTemp = tmpACLTemp;

    return errcode;
}
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

    pPoint->state.dACNTemp = tmpACNTemp;

    return errcode;
}
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

    pPoint->state.dBTypeConnectorTemp1 = tmpTemp;

    return errcode;
}
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

    pPoint->state.dBTypeConnectorTemp2 = tmpTemp;

    return errcode;
}
static ErrorCode_t StartCharge(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpChargeStatus;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    errcode = ERR_NO;
    /** @todo (rgw#1#): ��������̵��������ؼ̵���״̬ */

    return errcode;
}
static ErrorCode_t StopCharge(void *pvPoint)
{
    ChargePoint_t *pPoint;
    uint8_t ucPointID;
    ChargePointStateType tmpChargeStatus;
    ErrorCode_t errcode;

    pPoint = (ChargePoint_t *)pvPoint;
    ucPointID = pPoint->info.ucChargePointID;
    errcode = ERR_NO;

    /** @todo (rgw#1#): ��������̵��������ؼ̵���״̬ */
    return errcode;
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
    pChargePoint->info.ulVolatageUpperLimits = 0;
    pChargePoint->info.ulVolatageLowerLimits = 0;
    pChargePoint->info.ulRatedCurrent = 0;
    pChargePoint->info.dRatedPower = 0;

    pChargePoint->info.GetConnectorType = GetConnectorType;
    pChargePoint->info.GetVolatageUpperLimits = GetVolatageUpperLimits;
    pChargePoint->info.GetVolatageLowerLimits = GetVolatageLowerLimits;
    pChargePoint->info.GetRatedCurrent = GetRatedCurrent;
    pChargePoint->info.GetRatedPower = GetRatedPower;

    pChargePoint->state.dACLTemp = 0;
    pChargePoint->state.dACNTemp = 0;
    pChargePoint->state.dBTypeConnectorTemp1 = 0;
    pChargePoint->state.dBTypeConnectorTemp2 = 0;
    pChargePoint->state.dChargingCurrent = 0;
    pChargePoint->state.dChargingFrequence = 0;
    pChargePoint->state.dChargingVoltage = 0;
    pChargePoint->state.xBTypeConnectorLockState = 0;
    pChargePoint->state.xCCState = 0;
    pChargePoint->state.xCPState = 0;
    pChargePoint->state.xPlugState = 0;
    pChargePoint->state.xHandleEventGroupStartCharge = xEventGroupCreate();
    pChargePoint->state.xHandleEventGroupStopCharge = xEventGroupCreate();

    pChargePoint->state.GetChargingVoltage = GetChargingVoltage;
    pChargePoint->state.GetChargingCurrent = GetChargingCurrent;
    pChargePoint->state.GetChargingFrequence = GetChargingFrequence;

    pChargePoint->state.GetCPState = GetCPState;
    pChargePoint->state.GetCCState = GetCCState;
    pChargePoint->state.GetPlugState = GetPlugState;
    pChargePoint->state.GetBTypeConnectorLock = GetBTypeConnectorLock;
    pChargePoint->state.SetBTypeConnectorLock = SetBTypeConnectorLock;
    pChargePoint->state.GetACLTemp = GetACLTemp;
    pChargePoint->state.GetACNTemp = GetACNTemp;
    pChargePoint->state.GetBTypeConnectorTemp1 = GetBTypeConnectorTemp1;
    pChargePoint->state.GetBTypeConnectorTemp2 = GetBTypeConnectorTemp2;
    pChargePoint->state.StartCharge = StartCharge;
    pChargePoint->state.StopCharge = StopCharge;

    return pChargePoint;
}
