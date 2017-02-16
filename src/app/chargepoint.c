/**
* @file chargepoint.c
* @brief ������С��絥Ԫ��Ҫ�����������
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
#include "chargepoint.h"

/** @note (rgw#1#): ע�ⲻͬID��Ӳ���Ĳ�ͬ���� */

/** @brief ��ȡ����ѹ����⾫�� +/-0.1V
 *
 * @param pPoint ChargePoint_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetChargingVoltage(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    double tmpVolt;
    ErrorCode_t errcode;

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
ErrorCode_t GetChargingCurrent(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;

    double tmpCurr;
    ErrorCode_t errcode;

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
ErrorCode_t GetChargingFrequence(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    double tmpFreq;
    ErrorCode_t errcode;

    tmpFreq = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#): �ӵ���ȡ */

    pPoint->state.dChargingFrequence = tmpFreq;

    return errcode;
}

ErrorCode_t GetCPState(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    ChargePointStateType tmpCPState;
    ErrorCode_t errcode;

    tmpCPState = CP_12V;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.xCPState = tmpCPState;

    return errcode;
}
ErrorCode_t GetCCState(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    ChargePointStateType tmpCCState;
    ErrorCode_t errcode;

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
ErrorCode_t GetPlugState(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    uint32_t tmpPlugState;
    ErrorCode_t errcode;

    tmpPlugState = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.xPlugState = tmpPlugState;

    return errcode;
}

ErrorCode_t GetBTypeConnectorLock(ChargePoint_t *pPoint) //lock unlock
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    ChargePointStateType tmpLockState;
    ErrorCode_t errcode;

    tmpLockState = LOCK;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.xBTypeConnectorLockState = tmpLockState;

    return errcode;
}

ErrorCode_t SetBTypeConnectorLock(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    ChargePointStateType tmpLockState;
    ErrorCode_t errcode;

    tmpLockState = UNLOCK;
    errcode = ERR_NO;

    if(pPoint->state.xBTypeConnectorLockState == UNLOCK)
    {
        /** @todo (rgw#1#): ִ����ֹ���� */
    }
    THROW_ERROR(GetBTypeConnectorLock(pPoint), ERR_LEVEL_WARNING);

    return errcode;
}

ErrorCode_t GetACLTemp(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    double  tmpACLTemp;
    ErrorCode_t errcode;

    tmpACLTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.dACLTemp = tmpACLTemp;

    return errcode;
}
ErrorCode_t GetACNTemp(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    double tmpACNTemp;
    ErrorCode_t errcode;

    tmpACNTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.dACNTemp = tmpACNTemp;

    return errcode;
}
ErrorCode_t GetBTypeConnectorTemp1(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    double tmpTemp;
    ErrorCode_t errcode;

    tmpTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.dBTypeConnectorTemp1 = tmpTemp;

    return errcode;
}
ErrorCode_t GetBTypeConnectorTemp2(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    double tmpTemp;
    ErrorCode_t errcode;

    tmpTemp = 0;
    errcode = ERR_NO;

    /** @todo (rgw#1#):  */

    pPoint->state.dBTypeConnectorTemp2 = tmpTemp;

    return errcode;
}
ErrorCode_t StartCharge(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    ErrorCode_t errcode;
    ChargePointStateType tmpChargeStatus;

    errcode = ERR_NO;
    /** @todo (rgw#1#): ��������̵��������ؼ̵���״̬ */

    return errcode;
}
ErrorCode_t StopCharge(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    ErrorCode_t errcode;
    ChargePointStateType tmpChargeStatus;

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
    pChargePoint->info.ulCurrent = 0;
    pChargePoint->info.dPower = 0;



    pChargePoint->state.xHandleEventGroupStartCharge = xEventGroupCreate();
    pChargePoint->state.xHandleEventGroupStopCharge = xEventGroupCreate();

    return pChargePoint;
}
