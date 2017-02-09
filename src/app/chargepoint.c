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

static ChargePointStateType GetCPState(uint8_t  ucChargePointID)
{
    ChargePointStateType xCPState;
    xCPState = CP_12V;

    /** @todo (rgw#1#):  */
    return xCPState;
}
static ChargePointStateType GetCCState(uint8_t  ucChargePointID)
{
    ChargePointStateType xCCState;
    xCCState = CC_NO;

    /** @todo (rgw#1#):  */
    return xCCState;
}

static ChargePointStateType GetBTypeConnectorLock(uint8_t  ucChargePointID) //lock unlock
{
    ChargePointStateType xLockState;
    xLockState = LOCK;
    /** @todo (rgw#1#):  */

    return xLockState;
}

static ChargePointStateType SetBTypeConnectorLock(uint8_t  ucChargePointID)
{
    ChargePointStateType xLockState;
    xLockState = UNLOCK;
    /** @todo (rgw#1#): ִ����ֹ���� */
    xLockState = GetBTypeConnectorLock(ucChargePointID);
    return xLockState;
}

static double GetACLTemp(uint8_t  ucChargePointID)
{
    double  tmpACLTemp;
    tmpACLTemp = 0;
    /** @todo (rgw#1#):  */

    return tmpACLTemp;
}
static double GetACNTemp(uint8_t  ucChargePointID)
{
    /** @todo (rgw#1#):  */
    return 0;
}
static double GetBTypeConnectorTemp1(uint8_t  ucChargePointID)
{
    /** @todo (rgw#1#):  */
    return 0;
}
static double GetBTypeConnectorTemp2(uint8_t  ucChargePointID)
{
    /** @todo (rgw#1#):  */
    return 0;
}
static ChargePointStateType StartCharge(uint8_t  ucChargePointID)
{
    ChargePointStateType xChargeStatus;
    /** @todo (rgw#1#): ��������̵��������ؼ̵���״̬ */

    return xChargeStatus;

}
static ChargePointStateType StopCharge(uint8_t  ucChargePointID)
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
    pChargePoint->ucChargePointID = ucChargePointID;

    pChargePoint->xHandleEventGroupStartCharge = xEventGroupCreate();
    pChargePoint->xHandleEventGroupStopCharge = xEventGroupCreate();
    pChargePoint->GetCPState = GetCPState;
    pChargePoint->GetCCState = GetCCState;
    pChargePoint->GetBTypeConnectorLock = GetBTypeConnectorLock;
    pChargePoint->SetBTypeConnectorLock = SetBTypeConnectorLock;
    pChargePoint->GetACLTemp = GetACLTemp;
    pChargePoint->GetACNTemp = GetACNTemp;
    pChargePoint->GetBTypeConnectorTemp1 = GetBTypeConnectorTemp1;
    pChargePoint->GetBTypeConnectorTemp2 = GetBTypeConnectorTemp2;
}
