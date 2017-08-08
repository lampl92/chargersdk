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

#define REMOTE_SENDBUFF_MAX              1500 //���ͻ��峤��
#define REMOTE_RECVBUFF_MAX              1500 //���ջ��峤��
#define REMOTE_RECVDOPTDATA              1500

typedef struct
{
    uint8_t ucSegCont; //ʱ�θ���
    uint8_t ucStart[5];//��λСʱ����8��ʾ��8�㿪ʼ
    uint8_t ucEnd[5];//��λСʱ����9���ʾ��9�㣬��������9��
} EchSegTime_t;

typedef struct _echProtoInfo
{
    uint8_t  strServerIP[64 + 1]; //������������Ҳ������IP��ַ
    uint16_t usServerPort;
    uint8_t  strUserName[8 + 1];
    uint8_t  strUserPwd[12 + 1];
    uint8_t  strKey[16 + 1];
    uint8_t  strNewKey[16 + 1];
    time_t   tNewKeyChangeTime;

    uint32_t ulOptSN;           //��������������к�
    uint8_t  ucProtoVer;        //Э��汾��
    uint32_t ulHeartBeatCyc_ms; //�������� ��ȷ����
    uint8_t  ucResetAct;        //����ǰ������λ��ÿ�����������λ��1�����������ɹ����Ȼ�����㡣

    double dPowerFee_sharp;  //������
    double dPowerFee_peak;   //��
    double dPowerFee_shoulder; //ƽ
    double dPowerFee_off_peak; //��

    double dServFee_sharp;
    double dServFee_peak;
    double dServFee_shoulder;
    double dServFee_off_peak;

    EchSegTime_t SegTime_sharp;
    EchSegTime_t SegTime_peak;
    EchSegTime_t SegTime_shoulder;
    EchSegTime_t SegTime_off_peak;

    uint32_t ulStatusCyc_ms;    //״̬�����ϱ��������ȷ����
    uint32_t ulRTDataCyc_ms;    //ʵʱ�����ϱ����  10s

    ErrorCode_t (*GetProtoCfg)(void *pvProto, void *pvCfgObj);
    ErrorCode_t (*SetProtoCfg)(const uint8_t *jnItemString, uint8_t ObjType, const uint8_t *jnSubItemString, uint8_t SubType, void *pvCfgParam);
    int (*BnWIsListCfg)(uint8_t *path, uint8_t *strID);
    int (*BnWGetListSizeCfg)(uint8_t *path, uint16_t *size);
    int (*BnWGetListCfg)(uint8_t *path, uint16_t idx, uint8_t *strID);
    int (*BnWAddListCfg)(uint8_t *path, uint8_t *strID);
    int (*BnWDeleteListCfg)(uint8_t *path, uint8_t *strID);
    int (*BnWFlushListCfg)(uint8_t *path);

} echProtoInfo_t;

//�������к�

/* ����֡Ԫ�� */

/*********** ����һ������*************/
//step.1 ���������       ECH_CMDID_XXXX
//step.2 �޸��������     ECH_CMD_MAX
//step.3 ��дmakeCmdxxxx(),anlyCmdxxxx()
//step.4 ע������         ��EchProtocolCreate��
//step.6 ��дRemoteXXXX()��RemoteXXXXRes()  ��interface_remote.c��
//step.7 ��taskremote.c�б�дstep.6�ĵ��ú�����
//step.8 �ع�һ��ע�������Ƿ���step.3��д�ĺ���


/*������RecvCMDID*/
#define ECH_CMDID_REGISTER    0 //ע���½
#define ECH_CMDID_HEARTBEAT   1 //����
#define ECH_CMDID_RESET       2 //����
#define ECH_CMDID_STATUS      3 //״̬
#define ECH_CMDID_REMOTE_CTRL 4 //�޿���ͣ
#define ECH_CMDID_RTDATA      5 //ʵʱ����
#define ECH_CMDID_ORDER       6 //���׼�¼
#define ECH_CMDID_SET_SUCC    7 //���óɹ�
#define ECH_CMDID_SET_FAIL    8 //����ʧ��
#define ECH_CMDID_SET_POWERFEE  9  //ƽ̨�·��������
#define ECH_CMDID_SET_SERVFEE   10 //ƽ̨�·����������
#define ECH_CMDID_SET_CYC       11 //ƽ̨�·�״̬�ϱ�ʱ����
#define ECH_CMDID_SET_TIMESEG   12 //ƽ̨�·����ƽ��ʱ�������
#define ECH_CMDID_SET_KEY       13 //ƽ̨�·���Կ���
#define ECH_CMDID_REQ_POWERFEE  14 //ƽ̨��ѯ���׮��ǰ���
#define ECH_CMDID_REQ_SERVFEE   15 //ƽ̨��ѯ���׮��ǰ�����
#define ECH_CMDID_REQ_CYC       16 //ƽ̨��ѯ�ϱ�ʱ����
#define ECH_CMDID_REQ_TIMESEG   17 //ƽ̨��ѯ���ƽ��ʱ���
#define ECH_CMDID_REQ_KEY       18 //ƽ̨��ѯ��Կ��Ϣ
#define ECH_CMDID_REQ_SOFTVER   19 //ƽ̨��ѯ����汾��
#define ECH_CMDID_SET_QR        20 //ƽ̨����ǹ�ڻ�����ͨ��ά��
#define ECH_CMDID_REQ_QR        21 //ƽ̨��ѯǹ�ڻ�����ͨ��ά��
#define ECH_CMDID_SET_BLACK     22 //ƽ̨�·�����������
#define ECH_CMDID_SET_WHITE     23 //ƽ̨�·�����������
#define ECH_CMDID_REQ_BLACK     24 //ƽ̨��ѯ����������
#define ECH_CMDID_REQ_WHITE     25 //ƽ̨��ѯ����������
#define ECH_CMDID_ADD_BNW       26 //ƽ̨�����ڰ����� BNW = Black and White
#define ECH_CMDID_DEL_BNW       27 //ƽ̨ɾ���ڰ�����


/*�������*/
#define ECH_CMD_MAX             28

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
