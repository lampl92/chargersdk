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
/** @brief ���ļ��л�ȡ��ѹ����
 *
 * @param id uint8_t ǹ��
 * @return uint32_t
 *
 */
ErrorCode_t GetVolatageUpperLimits(uint8_t id, uint32_t *getVal)
{

}
/** @brief ���ļ�д���ѹ����
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
