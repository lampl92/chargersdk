/**
* @file ech_protocol.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#include "utils.h"
#include "libEcharge/ech_protocol.h"
#include "lwip/sockets.h"
#include "enc_dec.h"

#define defProtocolTimeout      10000

static uint16_t verifCheck(uint8_t *data, uint32_t len)
{
    uint16_t out = 0;
    uint32_t i;
    for(i = 0; i < len; i++)
    {
        out += data[i];
    }
    return out;
}
static int sendCommand(void *pObj, uint16_t ucSendID)
{
    const uint32_t ucTimeOutMS = 1000, uiTryTimes = 3;
    uint32_t ucFailedCounts;
    echProtocol_t *pProto;
    uint8_t *pucSendBuffer;
    EventBits_t uxBitsLwip;

    ucFailedCounts = 0;
    pProto = (echProtocol_t *)pObj;
    pucSendBuffer = pProto ->pucSendBuffer;

    do
    {
        uxBitsLwip = xEventGroupSync(xHandleEventLwIP,
                                     defEventBitTCPClientSendReq,
                                     defEventBitTCPClientSendOK,
                                     defProtocolTimeout
                                    );
        if((uxBitsLwip & defEventBitTCPClientSendOK) != defEventBitTCPClientSendOK)
        {
            //����ʧ��
            ucFailedCounts++;
            vTaskDelay(ucTimeOutMS);
        }
    }
    while((uxBitsLwip & defEventBitTCPClientSendOK) != defEventBitTCPClientSendOK && ucFailedCounts < uiTryTimes);
    if((uxBitsLwip & defEventBitTCPClientSendOK) != defEventBitTCPClientSendOK || ucFailedCounts == uiTryTimes)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
static int makeStdRegBodyCtx(uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    uint32_t ulMsgBodyCtxLen_dec;
    uint8_t i;

    ul2uc ultmpNetSeq;

    ulMsgBodyCtxLen_dec = 0;

    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    for(i = 0; i < strlen(pechProto->info.strUserName); i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pechProto->info.strUserName[i];
    }
    for(i = 0; i < strlen(pechProto->info.strUserPwd); i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pechProto->info.strUserPwd[i];
    }
    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec;
    return 0;
}
static int makeStdHeartBodyCtx(uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    uint32_t ulMsgBodyCtxLen_dec;
    uint8_t i;

    ul2uc ultmpNetSeq;

    ulMsgBodyCtxLen_dec = 0;

    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    return 0;
}
static int makeStdCmd(void *pObj, uint16_t usSendID)
{
    uint8_t *pucSendBuffer;
    uint8_t ucMsgHead[14];
    uint8_t ucMsgBodyCtx_dec[256];
    uint8_t ucMsgBodyCtx_enc[256];
    uint32_t ulMsgBodyCtxLen_dec;
    uint32_t ulMsgBodyCtxLen_enc;
    uint32_t ulMsgHeadLen;
    uint16_t usCheck;
    echCMD_t *pCMD;
    uint8_t i;

    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;

    pCMD = ((echProtocol_t *)pObj)->pCMD[usSendID];
    pucSendBuffer = ((echProtocol_t *)pObj)->pucSendBuffer;
    ulMsgHeadLen = 0;
    ulMsgBodyCtxLen_dec = 0;

    switch(usSendID)
    {
    case ECH_CMDID_REGISTER:
        makeStdRegBodyCtx(ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
        break;
    case ECH_CMDID_HEARTBEAT:
        makeStdHeartBodyCtx(ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
        break;
    default:
        break;
    }

    ulMsgBodyCtxLen_enc = aes_encrypt(ucMsgBodyCtx_dec,
                                      ulMsgBodyCtxLen_dec,
                                      pechProto->info.strKey,
                                      ucMsgBodyCtx_enc);

    //Э��汾
    ucMsgHead[ulMsgHeadLen++] = pechProto->info.ucProtoVer;
    //��Ϣ������
    ucMsgHead[ulMsgHeadLen++] = 0x00;
    //������
    ustmpNetSeq.usVal = htons(pCMD->CMDType.usSendCmd);
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];
    //Ԥ���ֶ�
    ultmpNetSeq.ulVal = 0;
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //��Ϣ�峤��
    ultmpNetSeq.ulVal = htonl((pEVSE->info.ucIDLength << 1) + ulMsgBodyCtxLen_enc); //ucIDLength����ΪHex���ȣ�Ӧ*2��ʾASCII ID�ĳ���
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //��ϢУ����
    ustmpNetSeq.usVal = htons(verifCheck(ucMsgHead, ulMsgHeadLen));
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];

    for(i = 0; i < ulMsgHeadLen; i++)
    {
        pucSendBuffer[i] = ucMsgHead[i];
    }
    for(i = 0; i < pEVSE->info.ucIDLength; i++)  //�˴�����ulMsgHeadLen��ʾpucSendBufferλ�ã�����֮��ulMsgHeadLen��ʾ�ľ���ԭulMsgHeadLen + ׮�ų���
    {
        sprintf(&(pucSendBuffer[ulMsgHeadLen]), "%02d", pEVSE->info.ucID[i]);
        ulMsgHeadLen = ulMsgHeadLen + 2;
    }
    for(i = 0; i < ulMsgBodyCtxLen_enc; i++)
    {
        pucSendBuffer[ulMsgHeadLen + i] = ucMsgBodyCtx_enc[i];
    }

    ((echProtocol_t *)pObj)->ulSendLength = ulMsgHeadLen + ulMsgBodyCtxLen_enc; //�˴�ulMsgHeadLen�Ѿ�������׮�ų���

    return 0;
}

static int analyStdRes(void *pObj, uint8_t ucSendID, uint32_t ulRecvLen)
{


}
int TransToServer(void *pObj, uint16_t usSendID, )
{
    const uint32_t ucTimeOutMS = 10000, uiTryTimes = 3;
    uint32_t ucFailedCounts;
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    uint32_t ulRcvdLength;
    int res ;

    ucFailedCounts = 0;

    pProto = (echProtocol_t *)pObj;
    pCMD = pProto->pCMD[ucSendID];

    pCMD->makeProc(pProto, ucSendID);
    do
    {
        res = pProto ->sendCommand(pProto, ucSendID);
        if(res != 1)
        {
            return 0;
        }
        MT626DelayMS(100);/** @todo (zshare#1#): ��Ϊ�ȴ�read �¼� */
        res = pProto ->recvResponse(pProto, ucSendID, &ulRcvdLength);
        if(res != MT_SUCCEED)
        {
            ucFailedCounts++;
            MT626DelayMS(ucTimeOutMS);
        }
    }
    while(res != MT_SUCCEED && ucFailedCounts < uiTryTimes);
    if(res != MT_SUCCEED || ucFailedCounts == uiTryTimes)
    {
        return MT_COM_FAIL;
    }

    return pCMD->analyProc(pProto, ucSendID, ulRcvdLength);
    pechProto->pCMD[ECH_CMDID_REGISTER]->makeProc(pechProto, ECH_CMDID_REGISTER, &data_len);
}
static void deleteProto(void *pObj)
{
    int i;
    echProtocol_t *pProto;

    pProto = (echProtocol_t *)pObj;
    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        if(pProto->pCMD[i] != NULL)
        {
            free(pProto->pCMD[i]);
            pProto->pCMD[i] = NULL;
        }
    }
    free(pProto ->pucRecvBuffer);
    free(pProto ->pucSendBuffer);
    free(pProto);
    pProto = NULL;
}
static echCMD_t *EchCMDCreate(uint16_t usSendCmd, uint16_t usRecvCmd, pECH_MAKE_PROC makeProc, pECH_ANALY_PROC analyProc)
{
    echCMD_t *pECHCMD = (echCMD_t *)malloc(sizeof(echCMD_t));
    if(pECHCMD == NULL)
    {
        return NULL;
    }
    pECHCMD->CMDType.usSendCmd = usSendCmd;
    pECHCMD->CMDType.usRecvCmd = usRecvCmd;

    pECHCMD->makeProc = makeProc;
    pECHCMD->analyProc = analyProc;

    return pECHCMD;
}

echProtocol_t *EchProtocolCreate(void)
{
    int i;
    echProtocol_t *pProto = (echProtocol_t *)malloc(sizeof(echProtocol_t));
    if(pProto == NULL)
    {
        return NULL;
    }
    strcpy(pProto->info.strServerIP, "124.207.112.70");
    pProto->info.usServerPort = 8051;
    strcpy(pProto->info.strUserName, "esaasusr");
    strcpy(pProto->info.strUserPwd, "esaaspasswrd");
    strcpy(pProto->info.strKey, "0123456789abcdeg");
    memset(pProto->info.strNewKey, 0, 17);
    pProto->info.tNewKeyChangeTime = 0;
    pProto->info.ucProtoVer = 0x68;
    pProto->info.ucHeartBeatCyc = 20;
    pProto->info.ucResetAct = 0;

    pProto->info.ulPowerFee_sharp = 0;//������ ϵ��0.0001
    pProto->info.ulPowerFee_peak = 0;//��
    pProto->info.ulPowerFee_shoulder = 0;//ƽ
    pProto->info.ulPowerFee_off_peak = 0;//��

    pProto->info.ulServiceFee_sharp = 0;//ϵ��0.0001
    pProto->info.ulServiceFee_peak = 0;
    pProto->info.ulServiceFee_shoulder = 0;
    pProto->info.ulServiceFee_off_peak = 0;

    pProto->info.ucStatusUploadCyc = 120;  //״̬�����ϱ��������ȷ����
    pProto->info.ucRTDataUploadCyc = 10;  //ʵʱ�����ϱ����  10s


    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        pProto->pCMD[i] = NULL;
    }
    pProto->pCMD[ECH_CMDID_REGISTER]    = EchCMDCreate(0x01, 0x02, makeStdCmd, analyStdRes);
    pProto->pCMD[ECH_CMDID_HEARTBEAT]   = EchCMDCreate(0x03, 0x04, makeStdCmd, analyStdRes);


//    pProto ->recvResponse = recvResponse;
    pProto ->sendCommand = sendCommand;
    pProto ->deleteProtocol = deleteProto;

    pProto->pucSendBuffer = (uint8_t *)malloc(REMOTE_SENDBUFF_MAX * sizeof(uint8_t));
    memset(pProto->pucSendBuffer, 0, MT626_SENDBUFF_MAX);
    pProto->pucRecvBuffer = (uint8_t *)malloc(REMOTE_RECVBUFF_MAX * sizeof(uint8_t));
    memset(pProto->pucRecvBuffer, 0, MT626_RECVBUFF_MAX);

    pProto->ulSendLength = 0;
    pProto->ulRecvLength = 0;

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses
