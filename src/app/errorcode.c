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
//  !!! 一定要保证code的 **顺序** 与头文件定义一致
//
const uint8_t *strErrorCode[100] =
{
    "无故障",
    "急停检测发生故障",
    "重力加速度传感器发生故障",
    "保护接地连续性传感器发生故障",
    "掉电状态检测器发生故障",
    "避雷器发生故障",

    "充电枪 电表通信故障",
    "充电枪 检测CP故障",
    "充电枪 检测CC故障",
    "充电枪 插枪状态检测器发生故障",
    "充电枪 B型连接枪锁状态检测器发生故障",
    "充电枪 B型连接枪锁无法锁止",
    "充电枪 L进线温度检测发生故障",
    "充电枪 N进线温度检测发生故障",
    "充电枪 B型连接温度检测点1发生故障",
    "充电枪 B型连接温度检测点2发生故障",
    "充电枪 开始充电错误",
    "充电枪 结束充电错误",
    "充电枪 触电粘连",

    "文件读写错误",
    "无目标文件",
    "参数解析错误",
    "参数内容错误",
    "设置参数范围错误",//   ERR_SET_PARAM,          //
    "JSON序列化错误",//ERR_SET_SERIALIZATION
    "其他错误",
    "读卡器故障",
    "GPRS模块故障",
    "串口奇偶校验错误",
    "串口噪声错误",
    "串口帧错误",
    "串口超载",
    "串口DMA传输错误",
    "网络通信故障",
    "网络通信超时",
    "内存分配错误",

    "无数据",
    "注册失败",
    "订单号不相等",
    "协议参数与本机无法兼容"
};

void ThrowErrorCode(uint32_t ulDevID, ErrorCode_t errcode, ErrorLevel_t errlevel, uint8_t *msg)
{
    ErrorPackage_t package;

    package.ulDevID = ulDevID;
    package.code = errcode;
    package.level = errlevel;
    strcpy(package.msg, msg);

    xQueueSend(xHandleQueueErrorPackage, (void *)&package, 0);
}

void ThrowFSCode (FRESULT rc, uint8_t *path, uint8_t *info)
{
    const char *str =
        "OK\0" "DISK_ERR\0" "INT_ERR\0" "NOT_READY\0" "NO_FILE\0" "NO_PATH\0"
        "INVALID_NAME\0" "DENIED\0" "EXIST\0" "INVALID_OBJECT\0" "WRITE_PROTECTED\0"
        "INVALID_DRIVE\0" "NOT_ENABLED\0" "NO_FILE_SYSTEM\0" "MKFS_ABORTED\0" "TIMEOUT\0"
        "LOCKED\0" "NOT_ENOUGH_CORE\0" "TOO_MANY_OPEN_FILES\0" "INVALID_PARAMETER\0";
    FRESULT i;
    if(rc != 0)
    {
        for (i = 0; i != rc && *str; i++)
        {
            while (*str++) ;
        }
        printf_safe("%s, %s, rc=%u FR_%s\n", (UINT)rc, str);
    }
}
