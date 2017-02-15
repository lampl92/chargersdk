/**
* @file chargepoint.c
* @brief ������С��絥Ԫ��Ҫ�����������
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
#include "chargepoint.h"



/** @brief ��ȡ����ѹ����⾫�� +/-0.1V
 *
 * @param void
 * @return double �з����ͣ�����ʵ�ʵ�ѹ
 *
 */
double GetChargingVoltage(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    /** @todo (rgw#1#): ��ȡ���ܱ��ѹ */
    double tmpVolt;
    tmpVolt = 0;
    return tmpVolt;
}

/** @brief ��ȡ����������⾫��+/-0.1A
 *
 * @param void
 * @return double �з����ͣ�����ʵ�ʵ���
 *
 */
double GetChargingCurrent(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    /** @todo (rgw#1#): ��ȡ���ܱ���� */
    double tmpCurr;
    tmpCurr = 0;
    return tmpCurr;
}

/** @brief ��ȡ��ԴƵ��
 *
 * @param void
 * @return double
 *
 */
double GetChargingFrequence(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
/** @todo (rgw#1#): �ӵ���ȡ */
    double tmpFreq;
    tmpFreq = 0;
    return tmpFreq;
}

/** @note (rgw#1#): ע�ⲻͬID��Ӳ���Ĳ�ͬ���� */

ChargePointStateType GetCPState(ChargePoint_t *pPoint)
{
    ChargePointStateType xCPState;
    xCPState = CP_12V;

    /** @todo (rgw#1#):  */
    return xCPState;
}
ChargePointStateType GetCCState(ChargePoint_t *pPoint)
{
    ChargePointStateType xCCState;
    xCCState = CC_NO;

    /** @todo (rgw#1#):  */
    return xCCState;
}

/** @brief ��ȡ��ǹ״̬��Ӧͬʱ������1��CC���ͼ���4��CP��
 *
 * @param pPoint ChargePoint_t*
 * @return uint32_t 0 �޲�ǹ
 *                  1 �в�ǹ
 *
 */
uint32_t GetPlugState(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->info.ucChargePointID;
    uint32_t tmpPlugState;
    tmpPlugState = 0;

    return tmpPlugState;
}

ChargePointStateType GetBTypeConnectorLock(ChargePoint_t *pPoint) //lock unlock
{
    ChargePointStateType xLockState;
    xLockState = LOCK;
    /** @todo (rgw#1#):  */

    return xLockState;
}

ChargePointStateType SetBTypeConnectorLock(ChargePoint_t *pPoint)
{
    ChargePointStateType xLockState;
    xLockState = UNLOCK;
    /** @todo (rgw#1#): ִ����ֹ���� */
    xLockState = GetBTypeConnectorLock(pPoint);
    return xLockState;
}

double GetACLTemp(ChargePoint_t *pPoint)
{
    double  tmpACLTemp;
    tmpACLTemp = 0;
    /** @todo (rgw#1#):  */

    return tmpACLTemp;
}
double GetACNTemp(ChargePoint_t *pPoint)
{
    /** @todo (rgw#1#):  */
    return 0;
}
double GetBTypeConnectorTemp1(ChargePoint_t *pPoint)
{
    /** @todo (rgw#1#):  */
    return 0;
}
double GetBTypeConnectorTemp2(ChargePoint_t *pPoint)
{
    /** @todo (rgw#1#):  */
    return 0;
}
ChargePointStateType StartCharge(ChargePoint_t *pPoint)
{
    ChargePointStateType xChargeStatus;
    /** @todo (rgw#1#): ��������̵��������ؼ̵���״̬ */

    return xChargeStatus;

}
ChargePointStateType StopCharge(ChargePoint_t *pPoint)
{
    ChargePointStateType xChargeStatus;
    /** @todo (rgw#1#): ��������̵��������ؼ̵���״̬ */
    return xChargeStatus;
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

    pChargePoint->state.xHandleEventGroupStartCharge = xEventGroupCreate();
    pChargePoint->state.xHandleEventGroupStopCharge = xEventGroupCreate();
}
