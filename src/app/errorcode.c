/**
* @file errorcode.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#include "includes.h"
#include "errorcode.h"

//
//  一定要保证code的 顺序 与头文件定义一致
//
const uint8_t *strErrorCode[100]=
{
    "No Error\n",     //ERR_NO
    "急停发生故障\n",   //ERR_SCRAM
    "其他错误\n"      //ERR_OTHER
};

void ThrowErrorCode(ErrorCode_t errcode)
{
    //printf_safe("%s",strErrorCode[errcode]);
    xQueueSend(xHandleQueueErrorCode, (void *)&errcode, 0);
}
