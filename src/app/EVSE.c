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


/** @brief 设备唯一序列号
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetEVSESN(EVSE_t *pEVSE, uint8_t *pLength)
{
    uint8_t tmpSN[24];
    uint8_t tmpLength;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    memset(tmpSN, 0, 24);
    tmpLength = 0;

/** @todo (rgw#1#): 从文件获取SN 并获取SN长度*/

    memmove(pEVSE->info.ucSN, tmpSN, *pLength);

    return errcode;
}

/** @brief 平台分配ID
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetEVSEID(EVSE_t *pEVSE, uint8_t *pLenght)
{
    uint8_t tmpID[24];
    uint8_t tmpLength;
    ErrorCode_t errcode;

    errcode = ERR_NO;
    memset(tmpID, 0, 24);
    tmpLength = 0;

/** @todo (rgw#1#): 从文件获取ID 并获取ID长度*/

    memmove(pEVSE->info.ucID, tmpID, *pLength);

    return errcode;
}

/** @brief 充电设备类型
 *         1：直流设备
 *         2：交流设备
 *         3：交直流一体设备
 *         4：无线设备
 *         5：其他
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetEVSEType(EVSE_t *pEVSE)
{
    uint8_t tmpType;
    ErrorCode_t errcode;

    tmpType = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#):  */

    pEVSE->info.ucType = tmpType;

    return errcode;
}

/** @brief 经纬度，保留后六位
 *
 * @param pEVSE EVSE_t*
 * @return ErrorCode_t
 *
 */
ErrorCode_t GetEVSELngLat(EVSE_t *pEVSE)
{
    double tmpLng,tmpLat;
    ErrorCode_t errcode;

    tmpLng = 0;
    tmpLat = 0;
    errcode = ERR_NO;

/** @todo (rgw#1#):  */

    pEVSE->info.dLng = tmpLng;
    pEVSE->info.dLat = tmpLng;

    return errcode;
}

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

    //errcode = ERR_SCRAM;
    errcode = ERR_NO;
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

    memset(pEVSE->info.ucSN, 0, 24);
    memset(pEVSE->info.ucID, 0, 24);
    pEVSE->info.ucType = 2;
    pEVSE->info.dLng = 116.275833;
    pEVSE->info.dLat = 39.831944;

    pEVSE->state.ulArresterState = 0;
    pEVSE->state.ulKnockState = 0;
    pEVSE->state.ulPEState = 0;
    pEVSE->state.ulPowerOffState = 0;
    pEVSE->state.ulScramState = 0;

    return pEVSE;
}

void EVSEinit(void)
{
    static ChargePoint_t *pchargepoint[2];  //在堆中定义
    uint8_t ucTotal;

    THROW_ERROR(GetTotalChargePoint(&ucTotal), ERR_LEVEL_WARNING);
    pEVSE = EVSECreate();
    pListChargePoint = UserListCreate();
    int i;
    for(i = 0; i < ucTotal; i++)
    {
        pchargepoint[i] = ChargePointCreate(i);
        pListChargePoint->Add(pListChargePoint, pchargepoint[i]);
    }
}
