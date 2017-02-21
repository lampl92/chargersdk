/**
* @file interface_remote.c
* @brief 通讯层，云平台调用，
* @author rgw
* @version v1.0
* @date 2017-02-06
*/

#include <time.h>
#include "includes.h"
#include "bsp.h"

ErrorCode_t GetRemoteTime(struct tm *pTimeBlock)
{
    struct tm tmpTimeBlock;
    ErrorCode_t errcode;

    errcode = ERR_NO;

    /** @todo (rgw#1#): 获取服务器时间 */


    *pTimeBlock = tmpTimeBlock;

    return errcode;
}

ErrorCode_t OpenRemote()
{

}

ErrorCode_t CloseRemote()
{

}

ErrorCode_t RecvRemote()
{}
ErrorCode_t SendRemote()
{}

