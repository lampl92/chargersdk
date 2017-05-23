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

#define REMOTE_SENDBUFF_MAX              128 //发送缓冲长度
#define REMOTE_RECVBUFF_MAX              128 //接收缓冲长度
#define REMOTE_RECVDOPTDATA             128

typedef struct _ECHProtoParam
{
    uint8_t strServerIP[15 + 1];
    uint16_t usServerPort;
    uint8_t strUserName[8 + 1];
    uint8_t strUserPwd[12 + 1];
    uint8_t strKey[16 + 1];
    uint8_t strNewKey[16 + 1];
    time_t tNewKeyChangeTime;

    uint8_t ucProtoVer; //协议版本号
    uint8_t ucHeartBeatCyc; //心跳周期 精确到秒
    uint8_t ucResetAct;//重启前进行置位，每次启动如果该位置1，则发送重启成功命令，然后清零。

    uint32_t ulPowerFee_sharp;//尖峰费率 系数0.0001
    uint32_t ulPowerFee_peak;//峰
    uint32_t ulPowerFee_shoulder;//平
    uint32_t ulPowerFee_off_peak;//谷

    uint32_t ulServiceFee_sharp;//系数0.0001
    uint32_t ulServiceFee_peak;
    uint32_t ulServiceFee_shoulder;
    uint32_t ulServiceFee_off_peak;

    uint8_t ucStatusUploadCyc;  //状态数据上报间隔，精确到秒
    uint8_t ucRTDataUploadCyc;  //实时数据上报间隔  10s

} echProtoInfo_t;

//操作序列号

/* 命令帧元素 */


/*命令子RecvCMDID*/
#define ECH_CMDID_REGISTER        0//注册登陆
#define ECH_CMDID_HEARTBEAT       1//心跳

#define ECH_CMD_MAX          2



typedef    int (*pECH_MAKE_PROC)  (void *pObj, uint16_t usSendID, uint8_t *pucSendBuffer, uint32_t *pulLen);
typedef    int (*pECH_ANALY_PROC) (void *pObj, uint16_t usSendID, uint8_t *pucRecvBuffer, uint32_t ulRecvLen);

typedef struct
{
    uint16_t usSendCmd;
    uint16_t usRecvCmd;
} echCMDType_t;

typedef struct _echCMD
{
    echCMDType_t CMDType;

    uint8_t     ucRecvdOptData[REMOTE_RECVDOPTDATA];
    uint32_t    uiRecvdOptLen;

    pECH_MAKE_PROC  makeProc;
    pECH_ANALY_PROC analyProc;

} echCMD_t;

typedef struct _echProtocol
{
    echProtoInfo_t info;
    echCMD_t *pCMD[ECH_CMD_MAX];
    gdsl_list_t plechSendCmd;
    gdsl_list_t plechRecvCmd;
    int (*sendCommand)(void *pObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax);
    int (*recvResponse)(void *pObj, uint8_t *pbuff, uint32_t ulRecvdLen, uint8_t trycountmax);
    void (*deleteProtocol)(void *pObj);
} echProtocol_t;

echProtocol_t *EchProtocolCreate(void);

#endif
