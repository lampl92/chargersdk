/**
* @file chargepoint.c
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
#include "chargepoint.h"



/** @brief 获取充电电压，检测精度 +/-0.1V
 *
 * @param void
 * @return double 有符号型，返回实际电压
 *
 */
double GetChargingVoltage(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
    /** @todo (rgw#1#): 获取电能表电压 */
    double tmpVolt;
    tmpVolt = 0;
    return tmpVolt;
}

/** @brief 获取充电电流，检测精度+/-0.1A
 *
 * @param void
 * @return double 有符号型，返回实际电流
 *
 */
double GetChargingCurrent(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
    /** @todo (rgw#1#): 获取电能表电流 */
    double tmpCurr;
    tmpCurr = 0;
    return tmpCurr;
}

/** @brief 获取电源频率
 *
 * @param void
 * @return double
 *
 */
double GetChargingFrequence(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
/** @todo (rgw#1#): 从电表获取 */
    double tmpFreq;
    tmpFreq = 0;
    return tmpFreq;
}

/** @note (rgw#1#): 注意不同ID对硬件的不同操作 */

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

/** @brief 获取插枪状态，应同时检测检测点1（CC）和检测点4（CP）
 *
 * @param pPoint ChargePoint_t*
 * @return uint32_t 0 无插枪
 *                  1 有插枪
 *
 */
uint32_t GetPlugState(ChargePoint_t *pPoint)
{
    uint8_t ucPointID = pPoint->ucChargePointID;
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
    /** @todo (rgw#1#): 执行锁止动作 */
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
    /** @todo (rgw#1#): 操作输出继电器，返回继电器状态 */

    return xChargeStatus;

}
ChargePointStateType StopCharge(ChargePoint_t *pPoint)
{
    ChargePointStateType xChargeStatus;
    /** @todo (rgw#1#): 操作输出继电器，返回继电器状态 */
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

    pChargePoint->status.xHandleEventGroupStartCharge = xEventGroupCreate();
    pChargePoint->status.xHandleEventGroupStopCharge = xEventGroupCreate();
}
