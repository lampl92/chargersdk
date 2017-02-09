/**
* @file interface_monitor.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "interface.h"


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

/** @brief 获得急停状态
 *
 * @param void
 * @return uint32_t 0 无急停
 *                  1 有急停
 *
 */
/** @note (rgw#1#): 暂时以查询方式获取急停状态 */
uint32_t GetScramState(void)
{
    uint32_t tmpScramState;
    tmpScramState = 0;

    /** @todo (rgw#1#): 实现代码 */

    return tmpScramState;
}

/** @brief 从传感器获取撞机状态
 *
 * @param void
 * @return uint32_t 0 无撞机
 *                  1 有撞击
 *
 */
uint32_t GetKnockState(void)
{
    uint32_t tmpKnockState;
    tmpKnockState = 0;

    /** @todo (rgw#1#): 实现代码 */

    return  tmpKnockState;
}

/** @brief 获取保护接地连续性状态
 *
 * @param void
 * @return uint32_t 0 正常
 *                  1 异常
 *
 */
uint32_t GetPEState(void)
{
    uint32_t tmpPEState;
    tmpPEState = 0;

    /** @todo (rgw#1#): 实现代码 */

    return tmpPEState;
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

/** @brief 获取掉电状态
 *
 * @param void
 * @return uint32_t 0 正常
 *                  1 掉电
 *
 */
uint32_t GetPowerOffState(void)
{
    return 0;
}

/** @brief 获取避雷器状态
 *
 * @param void
 * @return uint32_t 0 正常
 *                  1 异常
 *
 */
uint32_t GetArresterState(void)
{
    return 0;
}
