/**
* @file interface_charge.c
* @brief 充电任务需要的接口函数
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"

/** @brief 获取充电电压，检测精度 +/-0.1V
 *
 * @param void
 * @return double 有符号型，返回实际电压
 *
 */
double GetChargingVoltage(void)
{
    /** @todo (rgw#1#): 目前是获取电能表电压 */
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
double GetChargingCurrent(void)
{
    /** @todo (rgw#1#): 目前是获取电能表电流 */
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
double GetChargingFrequence(void)
{
/** @todo (rgw#1#): 从电表获取 */
    double tmpFreq;
    tmpFreq = 0;
    return tmpFreq;
}
