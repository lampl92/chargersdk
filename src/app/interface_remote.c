/**
* @file interface_remote.c
* @brief ͨѶ�㣬��ƽ̨���ã�
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

    /** @todo (rgw#1#): ��ȡ������ʱ�� */


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

