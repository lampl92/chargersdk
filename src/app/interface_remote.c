/**
* @file interface_remote.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/

#include <time.h>


ErrorCode_t GetRemoteTime(struct tm *pTimeBlock)
{
    struct tm tmpTimeBlock;
    ErrorCode_t errcode;

    errcode = ERR_NO;

/** @todo (rgw#1#): 获取服务器时间 */

    *pTimeBlock = tmpTimeBlock;

    return errcode;
}
