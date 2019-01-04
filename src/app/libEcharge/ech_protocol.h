/**
* @file ech_protocol.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-10
*/
#ifndef  __ECH_PROTOCOL_H
#define  __ECH_PROTOCOL_H

#include "gdsl_list.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"
#include "errorcode.h"
#include "order.h"
#include "libEcharge/ech_ftp.h"

#define ECH_UNUSED_ARG(x) (void)x

#define REMOTE_SENDBUFF_MAX              1500 //发送缓冲长度
#define REMOTE_RECVDOPTDATA              1500

typedef struct
{
    uint8_t ucPeriodCont; //时段个数
    uint8_t ucStart[defOrderPeriodMax];//单位小时：如8表示从8点开始
    uint8_t ucEnd[defOrderPeriodMax];//单位小时，如9点表示到9点，但不包括9点
} EchSegTime_t;

typedef struct _echProtoInfo
{
    char  strServerIP[64 + 1]; //可以是域名，也可以是IP地址
    uint16_t usServerPort;
    char  strUserName[8 + 1];
    char  strUserPwd[12 + 1];
    char  strKey[16 + 1];
    char  strNewKey[16 + 1];
    time_t   tNewKeyChangeTime;

    uint32_t ulOptSN;           //重启命令操作序列号
    uint8_t  ucProtoVer;        //协议版本号
    uint32_t ulHeartBeatCyc_ms; //心跳周期 精确到秒
    uint8_t  ucResetAct;        //重启前进行置位，每次启动如果该位置1，则发送重启成功命令，然后清零。

    double dSegEnergyFee[defOrderSegMax];  //分段费率
    double dSegServFee[defOrderSegMax];
    EchSegTime_t SegTime[defOrderSegMax];

    uint32_t ulStatusCyc_ms;    //状态数据上报间隔，精确到秒
    uint32_t ulRTDataCyc_ms;    //实时数据上报间隔  10s

    EchFtpCfg_t ftp;

    ErrorCode_t (*GetProtoCfg)(void *pvProto, void *pvCfgObj);
    int (*BnWIsListCfg)(char *path, char *strID);
    int (*BnWGetListSizeCfg)(char *path, uint16_t *size);
    int (*BnWGetListCfg)(char *path, uint16_t idx, char *strID);
    int (*BnWAddListCfg)(char *path, char *strID);
    int (*BnWDeleteListCfg)(char *path, char *strID);
    int (*BnWFlushListCfg)(char *path);

} echProtoInfo_t;

typedef struct _echProtoStatus
{
    uint8_t fault[6];
    uint8_t warning[6];
    uint8_t protect[6];
}echProtoStatus_t;
//操作序列号

/* 命令帧元素 */

/*********** 增加一条命令*************/
//step.1 添加命令字       ECH_CMDID_XXXX
//step.2 修改命令个数     ECH_CMD_MAX
//step.3 编写makeCmdxxxx(),anlyCmdxxxx()
//step.4 注册命令         在EchProtocolCreate中
//step.6 编写RemoteXXXX()和RemoteXXXXRes()  在interface_remote.c中
//step.7 在taskremote.c中编写step.6的调用函数。
//step.8 回顾一下注册命令是否是step.3编写的函数


/*命令字RecvCMDID*/
#define ECH_CMDID_REGISTER       0  //注册登陆
#define ECH_CMDID_HEARTBEAT      1  //心跳
#define ECH_CMDID_RESET          2  //重启
#define ECH_CMDID_STATUS         3  //状态
#define ECH_CMDID_REMOTE_CTRL    4  //无卡启停
#define ECH_CMDID_RTDATA         5  //实时数据
#define ECH_CMDID_ORDER          6  //交易记录
#define ECH_CMDID_SET_SUCC       7  //设置成功
#define ECH_CMDID_SET_FAIL       8  //设置失败
#define ECH_CMDID_SET_ENERGYFEE  9  //平台下发电价设置
#define ECH_CMDID_SET_SERVFEE    10 //平台下发服务费设置
#define ECH_CMDID_SET_CYC        11 //平台下发状态上报时间间隔
#define ECH_CMDID_SET_TIMESEG    12 //平台下发尖峰平谷时间段设置
#define ECH_CMDID_SET_KEY        13 //平台下发密钥变更
#define ECH_CMDID_REQ_ENERGYFEE  14 //平台查询充电桩当前电费
#define ECH_CMDID_REQ_SERVFEE    15 //平台查询充电桩当前服务费
#define ECH_CMDID_REQ_CYC        16 //平台查询上报时间间隔
#define ECH_CMDID_REQ_TIMESEG    17 //平台查询尖峰平谷时间段
#define ECH_CMDID_REQ_KEY        18 //平台查询密钥信息
#define ECH_CMDID_REQ_SOFTVER    19 //平台查询软件版本号
#define ECH_CMDID_SET_QR         20 //平台设置枪口互联互通二维码
#define ECH_CMDID_REQ_QR         21 //平台查询枪口互联互通二维码
#define ECH_CMDID_SET_USRPASS    22 //平台下发用户名密码
#define ECH_CMDID_SET_BLACK      23 //平台下发黑名单卡号
#define ECH_CMDID_SET_WHITE      24 //平台下发白名单卡号
#define ECH_CMDID_REQ_BLACK      25 //平台查询黑名单卡号
#define ECH_CMDID_REQ_WHITE      26 //平台查询白名单卡号
#define ECH_CMDID_ADD_BNW        27 //平台新增黑白名单 BNW = Black andWhite
#define ECH_CMDID_DEL_BNW        28 //平台删除黑白名单
#define ECH_CMDID_CARD_START     29 //充电桩请求有卡启动充电
#define ECH_CMDID_CARD_START_PWD 30 //充电桩带密码请求有卡启动充电
#define ECH_CMDID_CARD_START_RES 31 //充电桩上报有卡充电启动结果
#define ECH_CMDID_CARD_STOP_RES  32 //充电桩上报有卡充电停止充电
#define ECH_CMDID_CARD_RTDATA    33 //充电桩上报有卡实时充电数据
//---运维监控
#define ECH_CMDID_UP_FAULT       34 //充电桩上报故障信息
#define ECH_CMDID_UP_WARNING     35 //充电桩上报告警与保护信息
//---远程升级
#define ECH_CMDID_SET_OTA        36 //平台下发软件升级命令
#define ECH_CMDID_REQ_OTA_DW     37 //平台查询软件下载是否成功
#define ECH_CMDID_OTA_START      38 //充电桩上报进入升级状态
#define ECH_CMDID_OTA_RESULT     39 //充电桩上报升级结果

//---紧急停止
#define ECH_CMDID_EMERGENCY_STOP 40 //平台下发紧急停止充电

//---功率设置
#define ECH_CMDID_SET_POWER      41 //平台下发功率设置
#define ECH_CMDID_REQ_POWER      42 //平台查询功率设置

//---预约
#define ECH_CMDID_SET_APPOINT    43 //平台下发预约设置
#define ECH_CMDID_REQ_APPOINT    44 //平台查询预约设置

#define ECH_CMDID_REQ_ORDER      45//查询交易记录
#define ECH_CMDID_REQ_ICCID      46//查询SIM卡ICCID

/*命令个数*/
#define ECH_CMD_MAX              47

typedef struct
{
    uint32_t UID;
    time_t timestamp;
    uint32_t len;
    uint8_t *pbuff;
    uint8_t status;
} echCmdElem_t;

typedef    int (*pECH_MAKE_PROC)  (void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen);
typedef    int (*pECH_ANALY_PROC) (void *pPObj, uint16_t usSendID, uint8_t *pucRecvBuffer, uint32_t ulRecvLen);

typedef struct
{
    uint16_t usSendCmd;
    uint16_t usRecvCmd;
} echCMDType_t;

typedef struct _echCMD
{
    echCMDType_t CMDType;

    uint32_t ulRecvTimeout_s;

    uint8_t     ucRecvdOptData[REMOTE_RECVDOPTDATA];
    uint32_t    ulRecvdOptLen;
    EventGroupHandle_t xHandleEventCmd;
    SemaphoreHandle_t xMutexCmd;

    gdsl_list_t plRecvCmd;

    pECH_MAKE_PROC  makeProc;
    pECH_ANALY_PROC analyProc;

} echCMD_t;

typedef struct _echProtocol
{
    echProtoInfo_t info;
    echProtoStatus_t status;
    echCMD_t *pCMD[ECH_CMD_MAX];
    gdsl_list_t plechSendCmd;
    gdsl_list_t plechRecvCmd;
    SemaphoreHandle_t xMutexProtoSend;
    SemaphoreHandle_t xMutexProtoRecv;
    int (*sendCommand)(void *pPObj, void *pEObj, void *pCObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax);
    int (*recvResponse)(void *pPObj, void *pEObj, uint8_t *pbuff, uint32_t ulRecvdLen, uint8_t trycountmax);
    void (*deleteProtocol)(void *pPObj);
} echProtocol_t;

echProtocol_t *EchProtocolCreate(void);

#endif
