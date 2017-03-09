/**
* @file interface_data.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "interface.h"
#include "cJSON.h"
ErrorCode_t CreateOrderFile(void)
{
    FRESULT res;
    ErrorCode_t errcode;
    FIL fil;
    cJSON *order_root, *order_item;
    //res = f_open(&fil, "order/order.txt", FA_CREATE_NEW | FA_WRITE);
    switch(res)
    {
        case FR_OK:
            errcode = ERR_NO;
            break;
        default:
            errcode = ERR_FILE_RW;
            break;
    }
    return errcode;
}

/** @brief
 *
 * @param pulIndex uint32_t*   返回节点位置
 * @param order mxml_node_t*   要添加的节点
 * @return ErrorCode_t
 *
 */
ErrorCode_t DataAddOrder(uint32_t *pulIndex)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

/** @todo (rgw#1#):  */

//1. 获取最后节点
//2. 获取最后节点Index
//3. 将order的Index设置为+1
//4. 添加到节点最后

/*********************/

    *pulIndex = 1;

    return errcode;
}

/** @brief
 *
 * @param ulIndex uint32_t  索引
 * @param strNode uint8_t*  字段名字，例如"StartDate"
 * @param pvGetVal void*    返回字段内容
 * @return ErrorCode_t
 *
 */
ErrorCode_t DataGetOrder( uint32_t ulIndex, uint8_t *strNode, void *pvGetVal)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

/** @todo (rgw#1#):  */


/*********************/

    return errcode;
}
ErrorCode_t DataSetOrder( uint32_t ulIndex, uint8_t *strNode, void *pvGetVal)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;

    return errcode;
}
ErrorCode_t DataDelOrder(uint32_t ulIndex)
{
    ErrorCode_t errcode;
    errcode = ERR_NO;
    return errcode;
}
