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

#define ECH_UNUSED_ARG(x) (void)x

#define REMOTE_SENDBUFF_MAX              128 //���ͻ��峤��
#define REMOTE_RECVBUFF_MAX              128 //���ջ��峤��
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

    uint8_t ucProtoVer; //Э��汾��
    uint8_t ucHeartBeatCyc; //�������� ��ȷ����
    uint8_t ucResetAct;//����ǰ������λ��ÿ�����������λ��1�����������ɹ����Ȼ�����㡣

    uint32_t ulPowerFee_sharp;//������ ϵ��0.0001
    uint32_t ulPowerFee_peak;//��
    uint32_t ulPowerFee_shoulder;//ƽ
    uint32_t ulPowerFee_off_peak;//��

    uint32_t ulServiceFee_sharp;//ϵ��0.0001
    uint32_t ulServiceFee_peak;
    uint32_t ulServiceFee_shoulder;
    uint32_t ulServiceFee_off_peak;

    uint8_t ucStatusUploadCyc;  //״̬�����ϱ��������ȷ����
    uint8_t ucRTDataUploadCyc;  //ʵʱ�����ϱ����  10s

} echProtoInfo_t;

//�������к�

/* ����֡Ԫ�� */


/*������RecvCMDID*/
#define ECH_CMDID_REGISTER        0//ע���½
#define ECH_CMDID_HEARTBEAT       1//����
#define ECH_CMDID_STATUS           2//״̬

#define ECH_CMD_MAX          3



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
    int (*sendCommand)(void *pPObj, void *pEObj, void *pCObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax);
    int (*recvResponse)(void *pPObj, uint8_t *pbuff, uint32_t ulRecvdLen, uint8_t trycountmax);
    void (*deleteProtocol)(void *pPObj);
} echProtocol_t;

echProtocol_t *EchProtocolCreate(void);

#endif
