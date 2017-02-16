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
const uint8_t *strErrorCode[100] =
{
    "No Error",
    "急停检测故障",
    "其他错误",
    "读卡器串口奇偶校验错误",
    "读卡器串口噪声错误",
    "读卡器串口帧错误",
    "读卡器串口超载",
    "读卡器串口DMA传输错误"
    "网络模块串口奇偶校验错误",
    "网络模块串口噪声错误",
    "网络模块串口帧错误",
    "网络模块串口超载",
    "网络模块串口DMA传输错误"
};

void ThrowErrorCode(ErrorCode_t errcode, ErrorLevel_t errlevel)
{
    ErrorPackage_t package;

    package.code = errcode;
    package.level = errlevel;

    xQueueSend(xHandleQueueErrorPackage, (void *)&package, 0);
}
