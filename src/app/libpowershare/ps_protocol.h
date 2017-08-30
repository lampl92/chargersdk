/**
* @file ps_protocol.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-10
*/
#ifndef  __PS_PROTOCOL_H
#define  __PS_PROTOCOL_H

#define PS_CMD_MAX          4           //命令个数

/* 命令帧元素 */
#define PS_CMD_STX          0x7C
#define PS_CMD_ETX          0x7C

/*命令字SendID*/
#define PS_REGISTER_CMD     1    //注册登陆
#define PS_LOCALSTART_CMD   2    //电桩端开启充电
#define PS_LOCALSTOP_CMD    3    //电桩请求停止充电
#define PS_HEARTBEAT_CMD    4    //心跳

typedef    int (*pPS_MAKE_PROC)  (void *pObj, uint8_t ucSendID, uint8_t *pOptionData, uint32_t uiOptionLen, uint32_t *pucSendLength);
typedef    int (*pPS_ANALY_PROC) (void *pObj, uint8_t ucSendID, uint32_t uiRecvLen);

typedef struct _PSCMD
{
    uint8_t     ucCMD;
    uint8_t     ucRecvdOptData[16];//最长16个数据
    uint32_t    uiRecvdOptLen;

    pPS_MAKE_PROC  makeProc;
    pPS_ANALY_PROC analyProc;

} PSCMD_t;

typedef struct _PSProtocol
{
    PSCMD_t *pPSCMD[PS_CMD_MAX];
    uint8_t *pucSendBuffer;
    uint8_t *pucRecvBuffer;
    MT_RESULT (*sendCommand)(void *pObj, uint8_t ucSendID, uint32_t ucSendLenght);
    MT_RESULT (*recvResponse)(void *pObj, uint8_t ucSendID, uint32_t *pucRecvdLen);
    void (*deleteProtocol)(void *pObj);
} PSProtocol_t;

#endif
