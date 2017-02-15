/**
* @file EVSE.c
* @brief EVSE初始化
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "globals.h"
#include "userlib_list.h"
#include "interface.h"

/** @brief 获得急停状态
 *          0 无急停
 *          1 有急停
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetScramState(EVSE_t *pEVSE)
{
    uint32_t tmpScramState;
    ErrorCode_t errcode;

    errcode = ERR_SCRAM;
    tmpScramState = 0;

    /** @todo (rgw#1#): 实现代码 */

    pEVSE->state.ulScramState = tmpScramState;

    return errcode;
}

/** @brief 从传感器获取撞机状态
 *          0 无撞机
 *          1 有撞击
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetKnockState(EVSE_t *pEVSE)
{
    uint32_t tmpKnockState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpKnockState = 0;

    /** @todo (rgw#1#): 实现代码 */

    pEVSE->state.ulKnockState = tmpKnockState;

    return  errcode;
}

/** @brief 获取保护接地连续性状态
 *          0 正常
 *          1 异常
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetPEState(EVSE_t *pEVSE)
{
    uint32_t tmpPEState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpPEState = 0;

    /** @todo (rgw#1#): 实现代码 */

    pEVSE->state.ulPEState = tmpPEState;

    return errcode;
}

/** @brief 获取掉电状态
 *          0 正常
 *          1 掉电
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetPowerOffState(EVSE_t *pEVSE)
{
    uint32_t tmpOffState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpOffState = 0;

    /** @todo (rgw#1#): 实现代码 */

    pEVSE->state.ulPowerOffState = tmpOffState;

    return errcode;
}

/** @brief 获取避雷器状态
 *          0 正常
 *          1 异常
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetArresterState(EVSE_t *pEVSE)
{
    uint32_t tmpArresterState;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    tmpArresterState = 0;
    pEVSE->state.ulArresterState = tmpArresterState;
    return errcode;
}

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));

    return pEVSE;
}

void EVSEinit(void)
{
    static ChargePoint_t *pchargepoint[2];  //在堆中定义
    uint8_t ucTotal;

    THROW_ERROR(GetTotalChargePoint(&ucTotal));
    pEVSE = EVSECreate();
    pListChargePoint = UserListCreate();
    int i;
    for(i = 0; i < ucTotal; i++)
    {
        pchargepoint[i] = ChargePointCreate(i);
        pListChargePoint->Add(pListChargePoint, pchargepoint[i]);
    }
}
