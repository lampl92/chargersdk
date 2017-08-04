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

#define ECH_UNUSED_ARG(x) (void)x

#define REMOTE_SENDBUFF_MAX              1500 //发送缓冲长度
#define REMOTE_RECVBUFF_MAX              1500 //接收缓冲长度
#define REMOTE_RECVDOPTDATA              1500

typedef struct
{
    uint8_t ucSegCont; //时段个数
    uint8_t ucStart[5];//单位小时：如8表示从8点开始
    uint8_t ucEnd[5];//单位小时，如9点表示到9点，但不包括9点
} EchSegTime_t;

typedef struct _echProtoInfo
{
    uint8_t  strServerIP[64 + 1]; //可以是域名，也可以是IP地址
    uint16_t usServerPort;
    uint8_t  strUserName[8 + 1];
    uint8_t  strUserPwd[12 + 1];
    uint8_t  strKey[16 + 1];
    uint8_t  strNewKey[16 + 1];
    time_t   tNewKeyChangeTime;

    uint32_t ulOptSN;           //重启命令操作序列号
    uint8_t  ucProtoVer;        //协议版本号
    uint32_t ulHeartBeatCyc_ms; //心跳周期 精确到秒
    uint8_t  ucResetAct;        //重启前进行置位，每次启动如果该位置1，则发送重启成功命令，然后清零。

    double dPowerFee_sharp;  //尖峰费率
    double dPowerFee_peak;   //峰
    double dPowerFee_shoulder; //平
    double dPowerFee_off_peak; //谷

    double dServFee_sharp;
    double dServFee_peak;
    double dServFee_shoulder;
    double dServFee_off_peak;

    EchSegTime_t SegTime_sharp;
    EchSegTime_t SegTime_peak;
    EchSegTime_t SegTime_shoulder;
    EchSegTime_t SegTime_off_peak;

    uint32_t ulStatusCyc_ms;    //状态数据上报间隔，精确到秒
    uint32_t ulRTDataCyc_ms;    //实时数据上报间隔  10s

    ErrorCode_t (*GetProtoCfg)(void *pvProto, void *pvCfgObj);
    ErrorCode_t (*SetProtoCfg)(const uint8_t *jnItemString, uint8_t ObjType, const uint8_t *jnSubItemString, uint8_t SubType, void *pvCfgParam);

} echProtoInfo_t;

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
#define ECH_CMDID_REGISTER    0 //注册登陆
#define ECH_CMDID_HEARTBEAT   1 //心跳
#define ECH_CMDID_RESET       2 //重启
#define ECH_CMDID_STATUS      3 //状态
#define ECH_CMDID_REMOTE_CTRL 4 //无卡启停
#define ECH_CMDID_RTDATA      5 //实时数据
#define ECH_CMDID_ORDER       6 //交易记录
#define ECH_CMDID_SET_SUCC    7 //设置成功
#define ECH_CMDID_SET_FAIL    8 //设置失败
#define ECH_CMDID_SET_POWERFEE  9  //平台下发电价设置
#define ECH_CMDID_SET_SERVFEE   10 //平台下发服务费设置
#define ECH_CMDID_SET_CYC       11 //平台下发状态上报时间间隔
#define ECH_CMDID_SET_TIMESEG   12 //平台下发尖峰平谷时间段设置
#define ECH_CMDID_SET_KEY       13 //平台下发密钥变更
#define ECH_CMDID_REQ_POWERFEE  14 //平台查询充电桩当前电费
#define ECH_CMDID_REQ_SERVFEE   15 //平台查询充电桩当前服务费
#define ECH_CMDID_REQ_CYC       16 //平台查询上报时间间隔

/*命令个数*/
#define ECH_CMD_MAX             17

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
    echCMD_t *pCMD[ECH_CMD_MAX];
    gdsl_list_t plechSendCmd;
    gdsl_list_t plechRecvCmd;
    int (*sendCommand)(void *pPObj, void *pEObj, void *pCObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax);
    int (*recvResponse)(void *pPObj, void *pEObj, uint8_t *pbuff, uint32_t ulRecvdLen, uint8_t trycountmax);
    void (*deleteProtocol)(void *pPObj);
} echProtocol_t;

echProtocol_t *EchProtocolCreate(void);

#endif
