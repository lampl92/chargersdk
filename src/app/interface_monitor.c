/**
* @file interface_monitor.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "interface.h"


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
