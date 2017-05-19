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
#include "ech_protocol_proc.h"

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
static int sendCommand(void *pObj, uint16_t usSendID, uint32_t timeout, uint8_t trycount)
{
    echProtocol_t *pProto;
    echCmdElem_t echSendCmdElem;
    uint8_t pucSendBuffer[REMOTE_SENDBUFF_MAX];
    uint32_t ulSendLength;

    pProto = (echProtocol_t *)pObj;
    ulSendLength = 0;

    makeStdCmd(pProto, usSendID, pucSendBuffer, ulSendLength);

    echSendCmdElem.timestamp = time(NULL);
    echSendCmdElem.timeout = timeout;
    echSendCmdElem.cmd = pProto->pCMD[usSendID]->CMDType;
    echSendCmdElem.len = ulSendLength;
    echSendCmdElem.pbuff = pucSendBuffer;
    echSendCmdElem.status = 0;//0表示未发送  1表示已发送
    echSendCmdElem.trycount = trycount;
    echSendCmdElem.trycountmax = 3;//当一个常量用了，需要修改次数时再定义

    gdsl_list_insert_tail(pProto->plechSendCmd, (void *)&echSendCmdElem);
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
static int makeStdCmd(void *pObj, uint16_t usSendID, uint8_t *pucSendBuffer, uint32_t *pulSendLength)
{
    uint8_t ucMsgHead[14];
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint8_t ucMsgBodyCtx_enc[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;
    uint32_t ulMsgBodyCtxLen_enc;
    uint32_t ulMsgHeadLen;
    uint16_t usCheck;
    echCMD_t *pCMD;
    uint8_t i;

    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;

    pCMD = ((echProtocol_t *)pObj)->pCMD[usSendID];
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

    //协议版本
    ucMsgHead[ulMsgHeadLen++] = pechProto->info.ucProtoVer;
    //消息包属性
    ucMsgHead[ulMsgHeadLen++] = 0x00;
    //命令字
    ustmpNetSeq.usVal = htons(pCMD->CMDType.usSendCmd);
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];
    //预留字段
    ultmpNetSeq.ulVal = 0;
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //消息体长度
    ultmpNetSeq.ulVal = htonl((pEVSE->info.ucIDLength << 1) + ulMsgBodyCtxLen_enc); //ucIDLength长度为Hex长度，应*2表示ASCII ID的长度
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //消息校验码
    ustmpNetSeq.usVal = htons(verifCheck(ucMsgHead, ulMsgHeadLen));
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];

    for(i = 0; i < ulMsgHeadLen; i++)
    {
        pucSendBuffer[i] = ucMsgHead[i];
    }
    for(i = 0; i < pEVSE->info.ucIDLength; i++)  //此处借用ulMsgHeadLen表示pucSendBuffer位置，运行之后ulMsgHeadLen表示的就是原ulMsgHeadLen + 桩号长度
    {
        sprintf(&(pucSendBuffer[ulMsgHeadLen]), "%02d", pEVSE->info.ucID[i]);
        ulMsgHeadLen = ulMsgHeadLen + 2;
    }
    for(i = 0; i < ulMsgBodyCtxLen_enc; i++)
    {
        pucSendBuffer[ulMsgHeadLen + i] = ucMsgBodyCtx_enc[i];
    }

    *pulSendLength = ulMsgHeadLen + ulMsgBodyCtxLen_enc; //此处ulMsgHeadLen已经包含了桩号长度

    return 0;
}

static int analyStdRes(void *pObj, uint8_t ucSendID, uint32_t ulRecvLen)
{


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
//    free(pProto->pucRecvBuffer);
//    free(pProto->pucSendBuffer);
    gdsl_list_free(pProto->plechSendCmd);
    gdsl_list_free(pProto->plechRecvCmd);
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
    pProto->info.usServerPort      = 8051;
    strcpy(pProto->info.strUserName, "esaasusr");
    strcpy(pProto->info.strUserPwd, "esaaspasswrd");
    strcpy(pProto->info.strKey, "0123456789abcdeg");
    memset(pProto->info.strNewKey, 0, 17);
    pProto->info.tNewKeyChangeTime = 0;
    pProto->info.ucProtoVer        = 0x68;
    pProto->info.ucHeartBeatCyc    = 20;
    pProto->info.ucResetAct        = 0;

    pProto->info.ulPowerFee_sharp    = 0; //尖峰费率 系数0.0001
    pProto->info.ulPowerFee_peak     = 0; //峰
    pProto->info.ulPowerFee_shoulder = 0; //平
    pProto->info.ulPowerFee_off_peak = 0; //谷

    pProto->info.ulServiceFee_sharp    = 0; //系数0.0001
    pProto->info.ulServiceFee_peak     = 0;
    pProto->info.ulServiceFee_shoulder = 0;
    pProto->info.ulServiceFee_off_peak = 0;

    pProto->info.ucStatusUploadCyc = 120; //状态数据上报间隔，精确到秒
    pProto->info.ucRTDataUploadCyc = 10; //实时数据上报间隔  10s


    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        pProto->pCMD[i] = NULL;
    }
    pProto->pCMD[ECH_CMDID_REGISTER]    = EchCMDCreate(0x01, 0x02, makeStdCmd, analyStdRes);
    pProto->pCMD[ECH_CMDID_HEARTBEAT]   = EchCMDCreate(0x03, 0x04, makeStdCmd, analyStdRes);


//    pProto ->recvResponse = recvResponse;
    pProto ->sendCommand = sendCommand;
    pProto ->deleteProtocol = deleteProto;

///** @todo (rgw#1#): 调通后去掉这个buff，因为已经存在链表了 */
//    pProto->pucSendBuffer = (uint8_t *)malloc(REMOTE_SENDBUFF_MAX * sizeof(uint8_t));
//    memset(pProto->pucSendBuffer, 0, MT626_SENDBUFF_MAX);
//    pProto->pucRecvBuffer = (uint8_t *)malloc(REMOTE_RECVBUFF_MAX * sizeof(uint8_t));
//    memset(pProto->pucRecvBuffer, 0, MT626_RECVBUFF_MAX);

//    pProto->ulSendLength = 0;
//    pProto->ulRecvLength = 0;

    pProto->plechRecvCmd = gdsl_list_alloc ("RecvCmd", echCmdListAlloc, echCmdListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("SendCmd", echCmdListAlloc, echCmdListFree);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses
