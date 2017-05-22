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

static int makeStdCmd(void *pObj, uint16_t usSendID, uint8_t *pucSendBuffer, uint32_t *pulSendLength);

static uint16_t echVerifCheck(uint8_t ver, uint8_t atrri, uint16_t cmd, uint32_t len)
{
    return (uint16_t)(ver + atrri + cmd + len);
}
static int sendCommand(void *pObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax)
{
    echProtocol_t *pProto;
    echCmdElem_t echSendCmdElem;
    uint8_t pucSendBuffer[REMOTE_SENDBUFF_MAX];
    uint32_t ulSendLength;

    pProto = (echProtocol_t *)pObj;
    ulSendLength = 0;

    makeStdCmd(pProto, usSendID, pucSendBuffer, &ulSendLength);

    echSendCmdElem.timestamp = time(NULL);
    echSendCmdElem.timeout = timeout;
    echSendCmdElem.cmd = pProto->pCMD[usSendID]->CMDType;
    echSendCmdElem.cmd_id = usSendID;
    echSendCmdElem.len = ulSendLength;
    echSendCmdElem.pbuff = pucSendBuffer;
    echSendCmdElem.status = 0;//0表示未发送  1表示已发送
    echSendCmdElem.trycount = 0;
    echSendCmdElem.trycountmax = trycountmax;

    pProto->pCMD[usSendID]->uiRecvdOptLen = 0;
    memset(pProto->pCMD[usSendID]->ucRecvdOptData, 0, pProto->pCMD[usSendID]->uiRecvdOptLen);
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
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    uint8_t i;

    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pObj;
    pCMD = pProto->pCMD[usSendID];
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
    ustmpNetSeq.usVal = htons(echVerifCheck(pProto->info.ucProtoVer,
                                            0,
                                            pCMD->CMDType.usSendCmd,
                                            (pEVSE->info.ucIDLength << 1) + ulMsgBodyCtxLen_enc));
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

#define ECH_ERR_VER     -1
#define ECH_ERR_CHECK   -2
#define ECH_ERR_ID      -3

static int recvResponse(void *pObj, uint8_t *pbuff, uint32_t ulRecvdLen, uint8_t trycountmax)
{
    echProtocol_t *pProto;
    echCmdElem_t echRecvCmdElem;
    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;
    uint32_t ulMsgBodyLen_enc;
    uint16_t usChecksum;
    uint8_t EVSEID[8];
    int i;

    pProto = (echProtocol_t *)pObj;

    if(pbuff[0] != 0x68)
    {
        return ECH_ERR_VER;
    }

    //pbuff[1] 属性

    //pbuff[2,3] 命令字
    ustmpNetSeq.ucVal[0] = pbuff[2];
    ustmpNetSeq.ucVal[1] = pbuff[3];
    echRecvCmdElem.cmd.usRecvCmd = ntohs(ustmpNetSeq.usVal);

    //pbuff[4...7] 预留字段

    //pbuff[8...11] 消息体长度
    ultmpNetSeq.ucVal[0] = pbuff[8];
    ultmpNetSeq.ucVal[1] = pbuff[9];
    ultmpNetSeq.ucVal[2] = pbuff[10];
    ultmpNetSeq.ucVal[3] = pbuff[11];
    ulMsgBodyLen_enc = ntohl(ultmpNetSeq.ulVal);

    //pbuff[12,13] Checksum
    ustmpNetSeq.ucVal[0] = pbuff[12];
    ustmpNetSeq.ucVal[1] = pbuff[13];
    usChecksum = ntohs(ustmpNetSeq.usVal);
    if(usChecksum != echVerifCheck(0x68, 0, echRecvCmdElem.cmd.usRecvCmd, ulMsgBodyLen_enc))
    {
        return ECH_ERR_CHECK;
    }
    StrToHex(&pbuff[14], EVSEID, 16);
    if(memcmp(EVSEID, pEVSE->info.ucID, 8) != 0 )
    {
        return ECH_ERR_ID;
    }
    echRecvCmdElem.timestamp = time(NULL);
    switch(echRecvCmdElem.cmd.usRecvCmd)
    {
    case 2://主机回复的命令，不需要timeout
        echRecvCmdElem.cmd_id = ECH_CMDID_REGISTER;
        echRecvCmdElem.timeout =  0xffffffff;
        break;
    case 4:
        echRecvCmdElem.cmd_id = ECH_CMDID_HEARTBEAT;
        echRecvCmdElem.timeout =  0xffffffff;
        break;
    default:
        break;
    }
    echRecvCmdElem.len = ulRecvdLen;
    echRecvCmdElem.pbuff = pbuff;
    echRecvCmdElem.status = 0;
    echRecvCmdElem.trycount = 0;
    echRecvCmdElem.trycountmax = trycountmax;

    gdsl_list_insert_tail(pProto->plechRecvCmd, (void *)&echRecvCmdElem);
}

/** @brief
 *
 * @param pObj void*
 * @param usSendID uint16_t （这里可能会产生理解问题）SendID表示收到的回复对应的SendID
 * @param pbuff uint8_t*
 * @param ulRecvLen uint32_t
 * @return int
 *
 */
static int analyStdRes(void *pObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    uint8_t *pMsgBodyCtx_enc;
    uint8_t *pMsgBodyCtx_dec;
    uint32_t ulMsgBodyCtxLen_enc;
    ul2uc ultmpNetSeq;
    time_t timestamp;

    pProto = (echProtocol_t *)pObj;
    pMsgBodyCtx_enc = &pbuff[30];         //取出加密部分buff
    ulMsgBodyCtxLen_enc = ulRecvLen - 30; //加密部分长度
    pMsgBodyCtx_dec = (uint8_t *)malloc(ulMsgBodyCtxLen_enc * sizeof(uint8_t));

    aes_decrypt(pMsgBodyCtx_enc, pProto->info.strKey, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);

    memcpy(pProto->pCMD[usSendID]->ucRecvdOptData, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);
    pProto->pCMD[usSendID]->uiRecvdOptLen = ulMsgBodyCtxLen_enc;

    free(pMsgBodyCtx_dec);
}

static int analyRegRes(void *pObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    uint8_t *pMsgBodyCtx_dec;

    pProto = (echProtocol_t *)pObj;
    pMsgBodyCtx_dec = pProto->pCMD[usSendID]->ucRecvdOptData;

    analyStdRes(pObj, usSendID, pbuff, ulRecvLen);

    //xEventGroupSetBits(xHandleEventLwIP, defEventBitCmdRegedit);
    return 1;
}

static int analyHeartRes(void *pObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    uint8_t *pMsgBodyCtx_dec;
    ul2uc ultmpNetSeq;
    time_t timestamp;

    pProto = (echProtocol_t *)pObj;
    pMsgBodyCtx_dec = pProto->pCMD[usSendID]->ucRecvdOptData;

    analyStdRes(pObj, usSendID, pbuff, ulRecvLen);

    ultmpNetSeq.ucVal[0] = pMsgBodyCtx_dec[0];
    ultmpNetSeq.ucVal[1] = pMsgBodyCtx_dec[1];
    ultmpNetSeq.ucVal[2] = pMsgBodyCtx_dec[2];
    ultmpNetSeq.ucVal[3] = pMsgBodyCtx_dec[3];
    timestamp = (time_t)ntohl(ultmpNetSeq.ulVal);
    if(utils_abs(timestamp - time(NULL)) > 30)//大于30s进行校时
    {
        time(&timestamp);
    }
    return 1;
}

/** @brief 复制待插入的元素到新申请的空间
 *
 * @param pechCmd void*
 * @return gdsl_element_t
 *
 */
static gdsl_element_t echCmdListAlloc(gdsl_element_t pechCmd)
{
    echCmdElem_t *copyCmdListElem;

    copyCmdListElem = (echCmdElem_t *)malloc(sizeof(echCmdElem_t));
    if(copyCmdListElem != NULL)
    {
        memcpy(copyCmdListElem, pechCmd, sizeof(echCmdElem_t));
    }
    else
    {
        return NULL;
    }
    copyCmdListElem->pbuff = (uint8_t *)malloc(copyCmdListElem->len * sizeof(uint8_t));
    if(copyCmdListElem->pbuff != NULL)
    {
        memcpy(copyCmdListElem->pbuff, ((echCmdElem_t *)pechCmd)->pbuff, ((echCmdElem_t *)pechCmd)->len);
    }
    else
    {
        return NULL;
    }

    return (gdsl_element_t)copyCmdListElem;
}

static void echCmdListFree (gdsl_element_t e)
{
    free(((echCmdElem_t *)e)->pbuff);
    free (e);
    e = NULL;
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
    pProto->pCMD[ECH_CMDID_REGISTER]    = EchCMDCreate(0x01, 0x02, makeStdCmd, analyRegRes);
    pProto->pCMD[ECH_CMDID_HEARTBEAT]   = EchCMDCreate(0x03, 0x04, makeStdCmd, analyHeartRes);

    pProto ->recvResponse = recvResponse;
    pProto ->sendCommand = sendCommand;
    pProto ->deleteProtocol = deleteProto;

    pProto->plechRecvCmd = gdsl_list_alloc ("RecvCmd", echCmdListAlloc, echCmdListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("SendCmd", echCmdListAlloc, echCmdListFree);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses
