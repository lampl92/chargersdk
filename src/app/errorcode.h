/**
* @file errorcode.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __ERRORCODE_H
#define  __ERRORCODE_H

#include	"ff.h"

#define THROW_ERROR(_dev, _errcode,_errlevel,_msg)   do{                                           \
                                                    ErrorCode_t _macro_errcode = _errcode;         \
                                                    if(_macro_errcode != ERR_NO)                  \
                                                    {                                       \
                                                        ThrowErrorCode(_dev, _macro_errcode,_errlevel, _msg);  \
                                                    }                                       \
                                                }while(0);
typedef enum _ErrorCode
{
    ERR_NO,			//No Error
    ERR_SCRAM_FAULT,		//急停发生故障
    ERR_GSENSOR_FAULT,		//重力加速度传感器发生故障
    ERR_PE_FAULT,		//保护接地连续性传感器发生故障
    ERR_POWEROFF_DECT_FAULT,	//掉电状态检测器发生故障
    ERR_ARRESTER_FAULT,		//避雷器发生故障

    ERR_CON_METER_FAULT,        //电表通信故障
    ERR_CON_CP_FAULT,           //检测CP故障
    ERR_CON_CC_FAULT,           //检测CC故障
    ERR_CON_PLUG_FAULT,         //插枪状态检测器发生故障
    ERR_CON_B_LOCK_FAULT,       //B型连接枪锁状态检测器发生故障
    ERR_CON_CANT_LOCK,          //B型连接枪锁无法锁止
    ERR_CON_ACLTEMP_DECT_FAULT, //L进线温度检测发生故障
    ERR_CON_ACNTEMP_DECT_FAULT, //N进线温度检测发生故障
    ERR_CON_BTEMP1_DECT_FAULT,  //B型连接温度检测点1发生故障
    ERR_CON_BTEMP2_DECT_FAULT,  //B型连接温度检测点2发生故障
    ERR_CON_STARTCHARGE,        //开始充电错误
    ERR_CON_STOPCHARGE,         //结束充电错误
    ERR_RELAY_PASTE,            //  "充电点 触点粘连",

    ERR_FILE_RW,		//文件读写错误
    ERR_FILE_NO,		//无目标文件
    ERR_FILE_PARSE,		//"参数解析错误",
    ERR_FILE_PARAM,		//参数内容错误
    ERR_SET_PARAM,		//设置参数范围错误
    ERR_SET_SERIALIZATION,	//JSON序列化错误
    ERR_OTHER,			//其他错误
    ERR_RFID_FAULT,		//读卡器获取UID错误
    ERR_GPRS_FAULT,     //GPRS模块故障

    ERR_UART_PE,		//串口奇偶校验错误
    ERR_UART_NE,		//串口噪声错误
    ERR_UART_FE,		//串口帧错误
    ERR_UART_ORE,		//串口超载
    ERR_UART_DMA,		//串口DMA传输错误


    ERR_NONET,			//网络通信故障
    ERR_NET_TIMEOUT,		//网络通信超时
    ERR_MEMORY,			//Malloc错误

    ERR_REMOTE_NODATA,  //无数据
    ERR_REMOTE_REGEDIT,  //注册失败
    ERR_REMOTE_ORDERSN,  //订单号不相等
    ERR_REMOTE_PARAM,    //协议参数与本机无法兼容
    ERR_WHITE_LIST,
    ERR_BLACK_LIST

} ErrorCode_t;

typedef enum _ErrorLevel
{
    ERR_LEVEL_OK,
    ERR_LEVEL_TIPS,
    ERR_LEVEL_WARNING,
    ERR_LEVEL_CRITICAL,
    ERR_LEVEL_CLEAR
} ErrorLevel_t;

typedef struct _ErrorPackage
{
    uint32_t		ulDevID;
    ErrorCode_t		code;
    ErrorLevel_t	level;
    uint8_t		msg[64];
} ErrorPackage_t;

//充电枪ID从小到大定义，其他设备从大小定义
//DevID 0~? 充电枪ID
#define defDevID_System		248
#define defDevID_Cloud		249
#define defDevID_File		250
#define devDevID_Meter      251
#define defDevID_GPRS       252
#define defDevID_RFID		253
#define defDevID_NetMoudle	254
#define defDevID_EVSE		255	//0xff


extern const uint8_t	*strErrorCode[];

void	ThrowErrorCode(uint32_t ulDevID, ErrorCode_t errcode, ErrorLevel_t errlevel, uint8_t *msg);
void	ThrowFSCode (FRESULT rc, uint8_t *path, uint8_t *info);

#endif
