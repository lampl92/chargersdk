/**
* @file errorcode.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __ERRORCODE_H
#define  __ERRORCODE_H

#define THROW_ERROR(errcode,errlevel)   if(errcode != ERR_NO) {ThrowErrorCode(errcode,errlevel);}

typedef enum _ErrorCode
{
    ERR_NO,                 //No Error
    ERR_SCRAM,              //急停发生故障
    ERR_OTHER,              //其他错误
    ERR_RFID_PE,            //读卡器串口奇偶校验错误
    ERR_RFID_NE,            //读卡器串口噪声错误
    ERR_RFID_FE,            //读卡器串口帧错误
    ERR_RFID_ORE,           //读卡器串口超载
    ERR_RFID_DMA,           //读卡器串口DMA传输错误
    ERR_NETMODULE_PE,       //网络模块串口奇偶校验错误
    ERR_NETMODULE_NE,       //网络模块串口噪声错误
    ERR_NETMODULE_FE,       //网络模块串口帧错误
    ERR_NETMODULE_ORE,      //网络模块串口超载
    ERR_NETMODULE_DMA,      //网络模块串口DMA传输错误
} ErrorCode_t;

typedef enum _ErrorLevel
{
    ERR_LEVEL_TIPS,
    ERR_LEVEL_WARNING,
    ERR_LEVEL_CRITICAL,
    ERR_LEVEL_CLEAR
} ErrorLevel_t;

typedef struct _ErrorPackage
{
    ErrorCode_t code;
    ErrorLevel_t level;
}ErrorPackage_t;

extern const uint8_t *strErrorCode[];

void ThrowErrorCode(ErrorCode_t errcode, ErrorLevel_t errlevel);

#endif
