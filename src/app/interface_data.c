/**
* @file interface_data.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "interface.h"

/** @brief
 *
 * @param void
 * @return uint8_t ǹ��������2��������ǹ
 *
 */
ErrorCode_t GetTotalChargePoint(uint8_t *getVal)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    *getVal = 2;

    return errcode;
}


