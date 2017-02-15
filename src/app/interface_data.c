/**
* @file interface_data.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "errorcode.h"
/** @brief
 *
 * @param void
 * @return uint8_t 枪的数量，2代表两把枪
 *
 */
ErrorCode_t GetTotalChargePoint(uint8_t *getVal)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    *getVal = 2;

    return errcode;
}
/** @brief 从文件中获取电压上限
 *
 * @param id uint8_t 枪号
 * @return uint32_t
 *
 */
ErrorCode_t GetVolatageUpperLimits(uint8_t id, uint32_t *getVal)
{

}
/** @brief 向文件写入电压上限
 *
 * @param id uint8_t
 * @param setVal uint32_t
 * @return ErrorCode_t
 *
 */
ErrorCode_t SetVolatageUpperLimits(uint8_t id, uint32_t setVal)
{

}

ErrorCode_t GetVolatageLowerLimits(uint8_t id)
{

}
