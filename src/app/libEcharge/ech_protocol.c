/**
* @file ech_protocol.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#include "utils.h"
#include "interface.h"
#include "enc_dec.h"
#include "libEcharge/ech_protocol_proc.h"

#define defProtocolTimeout      10


static uint16_t echVerifCheck(uint8_t ver, uint8_t atrri, uint16_t cmd, uint32_t len)
{
    return (uint16_t)(ver + atrri + cmd + len);
}
static int sendCommand(void *pPObj, void *pEObj, void *pCObj, uint16_t usSendID, uint32_t timeout, uint8_t trycountmax)
{
    echProtocol_t *pProto;
    echCmdElem_t echSendCmdElem;
    uint8_t pucSendBuffer[REMOTE_SENDBUFF_MAX];
    uint32_t ulSendLength;

    pProto = (echProtocol_t *)pPObj;
    ulSendLength = 0;

    pProto->pCMD[usSendID]->makeProc(pPObj, pEObj, pCObj, pucSendBuffer, &ulSendLength);

    echSendCmdElem.timestamp = time(NULL);
    echSendCmdElem.timeout_s = timeout;
    echSendCmdElem.cmd = pProto->pCMD[usSendID]->CMDType;
    echSendCmdElem.cmd_id = usSendID;
    echSendCmdElem.len = ulSendLength;
    echSendCmdElem.pbuff = pucSendBuffer;
    echSendCmdElem.status = 0;//0��ʾδ����  1��ʾ�ѷ���
    echSendCmdElem.trycount = 0;
    echSendCmdElem.trycountmax = trycountmax;

    pProto->pCMD[usSendID]->uiRecvdOptLen = 0;
    memset(pProto->pCMD[usSendID]->ucRecvdOptData, 0, REMOTE_RECVDOPTDATA);
    gdsl_list_insert_tail(pProto->plechSendCmd, (void *)&echSendCmdElem);
}
static int makeStdCmd(void *pPObj,
                      void *pEObj,
                      uint16_t usSendID,
                      uint8_t *pucMsgBodyCtx_dec,
                      uint32_t ulMsgBodyCtxLen_dec,
                      uint8_t *pucSendBuffer,
                      uint32_t *pulSendLength)
{
    echProtocol_t *pProto;
    echCMD_t *pCMD;
    EVSE_t *pE;
    uint8_t ucMsgHead[64];
    uint32_t ulMsgHeadLen;
    uint8_t ucMsgBodyCtx_enc[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_enc;
    uint16_t usCheck;
    uint8_t i;

    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    pCMD = pProto->pCMD[usSendID];
    pE = (EVSE_t *)pEObj;
    ulMsgHeadLen = 0;

    ulMsgBodyCtxLen_enc = aes_encrypt(pucMsgBodyCtx_dec,
                                      ulMsgBodyCtxLen_dec,
                                      pProto->info.strKey,
                                      ucMsgBodyCtx_enc);

    //Э��汾
    ucMsgHead[ulMsgHeadLen++] = pProto->info.ucProtoVer;
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
    ultmpNetSeq.ulVal = htonl((pE->info.ucIDLength << 1) + ulMsgBodyCtxLen_enc); //ucIDLength����ΪHex���ȣ�Ӧ*2��ʾASCII ID�ĳ���
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //��ϢУ����
    ustmpNetSeq.usVal = htons(echVerifCheck(pProto->info.ucProtoVer,
                                            0,
                                            pCMD->CMDType.usSendCmd,
                                            (pE->info.ucIDLength << 1) + ulMsgBodyCtxLen_enc));
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ustmpNetSeq.ucVal[1];

    for(i = 0; i < ulMsgHeadLen; i++)
    {
        pucSendBuffer[i] = ucMsgHead[i];
    }
    for(i = 0; i < pE->info.ucIDLength; i++)  //�˴�����ulMsgHeadLen��ʾpucSendBufferλ�ã�����֮��ulMsgHeadLen��ʾ�ľ���ԭulMsgHeadLen + ׮�ų���
    {
        sprintf(&(pucSendBuffer[ulMsgHeadLen]), "%02x", pE->info.ucID[i]);
        ulMsgHeadLen = ulMsgHeadLen + 2;
    }
    for(i = 0; i < ulMsgBodyCtxLen_enc; i++)
    {
        pucSendBuffer[ulMsgHeadLen + i] = ucMsgBodyCtx_enc[i];
    }

    *pulSendLength = ulMsgHeadLen + ulMsgBodyCtxLen_enc; //�˴�ulMsgHeadLen�Ѿ�������׮�ų���

    return 1;
}
static int makeCmdRegBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint32_t ulMsgBodyCtxLen_dec;
    uint8_t i;
    ul2uc ultmpNetSeq;

    pProto = (echProtocol_t *)pPObj;
    ulMsgBodyCtxLen_dec = 0;

    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    for(i = 0; i < strlen(pProto->info.strUserName); i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.strUserName[i];
    }
    for(i = 0; i < strlen(pProto->info.strUserPwd); i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pProto->info.strUserPwd[i];
    }
    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec;

    return 1;
}
static int makeCmdReg(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdRegBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REGISTER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);

    return 1;
}

static int makeCmdHeartBodyCtx(uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    uint32_t ulMsgBodyCtxLen_dec;

    ul2uc ultmpNetSeq;

    ulMsgBodyCtxLen_dec = 0;

    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdHeart(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdHeartBodyCtx(ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_HEARTBEAT, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}

static int makeCmdStatusBodyCtx(void *pEObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    EVSE_t *pEVSE;
    CON_t *pCON;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    uint8_t errcode;
    EventBits_t uxBits;

    pCON = (CON_t *)pCObj;
    pEVSE = (EVSE_t *)pEObj;

    ulMsgBodyCtxLen_dec = 0;
    errcode = 0;

    //���׮�ӿ�   0��Ĭ�� 1��A 2��B
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //ԤԼ״̬ 1����ԤԼ  2:��ԤԼ
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    //��λ״̬ 1������   2��ռ��   3��δ֪
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    //�ӿ�����״̬  1 ���У� 2,������ 3 δ֪
    if(pCON->status.xPlugState == UNPLUG)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    }
    else if(pCON->status.xPlugState == PLUG)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
    }
    else
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    }
    //�ӿڹ���״̬ 1����� 2:���� 3������ 4�������� 5��δ֪
    switch(pCON->state)
    {
    case STATE_CON_CHARGING:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
        break;
    case STATE_CON_IDLE:
    case STATE_CON_PLUGED:
    case STATE_CON_PRECONTRACT:
    case STATE_CON_PRECONTRACT_LOSEPLUG:
    case STATE_CON_STARTCHARGE: //�����״̬��û��ʼ���
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
        break;
    case STATE_CON_ERROR:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
        break;
    case STATE_CON_STOPCHARGE:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 4;
        break;
    default:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 5;
        break;
    }
    //�����ѹxxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C ���ѹ
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //�������xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C �����
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //�̵���״̬ 1����2��
    if(pCON->status.ucRelayLState == SWITCH_ON &&
            pCON->status.ucRelayNState == SWITCH_ON)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    }
    else if(pCON->status.ucRelayLState == SWITCH_OFF &&
            pCON->status.ucRelayNState == SWITCH_OFF)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 2;
    }
    //ϵͳ�������״̬ 1������ 2���ػ�
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //׮ʵʱ�¶� xx.xxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dACLTemp * 1000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //�й����� �޹����� xx.xxxx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //���ܱ��й����� xx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //����޹����� xx.xx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //SOC 1~100
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //������
    if(pEVSE->status.ulScramState == 1)
    {
        errcode |= 1 << 0; //Bit0 ��ͣ����
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventException);
    if((uxBits & defEventBitExceptionMeter) == defEventBitExceptionMeter)
    {
        errcode |= 1 << 1; //Bit1 ������
    }
    if((uxBits & defEventBitExceptionRelayPaste) == defEventBitExceptionRelayPaste)
    {
        errcode |= 1 << 2; //Bit2 �Ӵ�������
    }
    if((uxBits & defEventBitExceptionRFID) == defEventBitExceptionRFID)
    {
        errcode |= 1 << 3; //Bit3 ����������
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
    if((uxBits & defEventBitCONACTempOK) != defEventBitCONACTempOK)
    {
        errcode |= 1 << 4; //Bit4 �ڲ����¹���
    }
    //errcode |= 1 << 5 //Bit5 ����������
    if(pEVSE->status.ulPEState == 1)
    {
        errcode |= 1 << 6; //Bit5 ��Ե����
    }
    if(pEVSE->status.ulPowerOffState == 1)
    {
        errcode |= 1 << 7;  //Bit7 ����(�ڴ˶���Ϊͣ�����)
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = errcode;
    //���׮��ǰʱ��
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdStatus(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdStatusBodyCtx(pEObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_STATUS, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}

static int makeCmdRemoteCtrlBodyCtx(void *pPObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    uint8_t *pbuff;
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pbuff = pProto->pCMD[ECH_CMDID_REMOTE_CTRL]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    for(i = 0; i < 14; i++)
    {
        //[0...3] ����ID
        //[4...11] ������ˮ��
        //[12] ���׮�ӿ�
        //[13] ���׮����
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[i]; //����
    }
    //[14] ��ͣ���
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[14];//�ڷ�������֮ǰ��ֵ��
    //[15] ʧ��ԭ��
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[15];//�ڷ�������֮ǰ��ֵ��
    //[16] SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdRemoteCtrl(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdRemoteCtrlBodyCtx(pPObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_REMOTE_CTRL, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdRTDataBodyCtx(void *pPObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8];
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_RTDATA]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0...7] ������ˮ��
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[8] ׮�ӿ�
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[9...12] ��ǰ����ܵ��� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[13...16] ����� xxx.xx
    //[17...20] �����
    //[21...24] ƽ����
    //[25...28] �ȵ���
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[29...32] ��ǰ����� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[32...36] ��ǰ����ѽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServiceFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[37,38] ��ǰ���ʱ��
    ustmpNetSeq.usVal = htons(  time(NULL) - pCON->order.tStartTime  );
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[39] ���׮״̬
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[39];//ע���޸�����ҲҪ�޸�interface_remote.c��Ӧλ��
    //[40] ֹͣԭ��
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[40];
    //[41] ��ǰSOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[42,43] ʣ����ʱ��
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[44...47] �����ѹ xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[48...51] ������� xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[52...55] ���������¶�
    //[56...59] ���������¶�
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[60...63] ��ǰʱ���
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[64...80] ����VIN��
    for(i = 0; i < 17; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdRTData(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdRTDataBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_RTDATA, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
static int makeCmdOrderBodyCtx(void *pPObj, void *pCObj, uint8_t *pucMsgBodyCtx_dec, uint32_t *pulMsgBodyCtxLen_dec)
{
    echProtocol_t *pProto;
    CON_t *pCON;
    uint8_t *pbuff;
    uint8_t ucOrderSN[8];
    uint8_t strCardID[17];
    uint32_t ulMsgBodyCtxLen_dec;
    ul2uc ultmpNetSeq;
    us2uc ustmpNetSeq;
    uint8_t reason;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pCON = (CON_t *)pCObj;
    pbuff = pProto->pCMD[ECH_CMDID_ORDER]->ucRecvdOptData;
    ulMsgBodyCtxLen_dec = 0;

    //[0] �п� 04 �޿�05
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[0];
    //[1...8] ������ˮ��
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[9] ���׮�ӿ�
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[10...25] ����
    if(pbuff[0] == 4)
    {
        HexToStr(pCON->order.ucCardID, strCardID, 8);
        for(i = 0; i < 16; i++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = strCardID[i];
        }
    }
    else if(pbuff[0] == 5)
    {
        for(i = 0; i < 16; i++)
        {
            pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
        }
    }
    //[26...29] ���ǰ�ܵ���ʾֵ xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dStartPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[30...33] ���������ʾֵ xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)((pCON->order.dStartPower + pCON->order.dTotalPower) * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[34...37] ���γ�����ܽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[38...41] ���γ�������ܽ�� xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServiceFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[42...45] ����       xx.xxxx
    //[46...49] �����ѵ��� xx.xxxx
    //[50...53] �����       xxx.xx
    //[54...57] ������   xxx.xx
    //[58...61] �����ѽ�� xxx.xx
    //[62,63] ����ʱ�� xx
    //[64...85]��
    //[86...107]ƽ
    //[108...129]��
    for(i = 0; i < 22 * 4; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[130...133] ��翪ʼʱ��
    ultmpNetSeq.ulVal = htonl(pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[134,135] ������ʱ��
    ustmpNetSeq.usVal = htons(pCON->order.tStopTime - pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[136] ֹͣ���ԭ��
    switch(pCON->order.ucStopType)
    {
    case defOrderStopType_RFID:
    case defOrderStopType_Remote:
        reason = 1;//�ֶ�ֹͣ
        break;
    case defOrderStopType_Full:
        reason = 3;//����ֹͣ
        break;
    case defOrderStopType_Fee:
        reason = 4;//�ﵽ�����
        break;
    case defOrderStopType_Scram:
    case defOrderStopType_NetLost:
    case defOrderStopType_Poweroff:
    case defOrderStopType_OverCurr:
    case defOrderStopType_Knock:
        reason = 5;//�쳣ֹͣ
        break;
    default:
        reason = 6;//����ԭ��ֹͣ
        break;
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = reason;
    //[137] ��ǰsoc
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[138] ״̬
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[139...142] ������ʱ��
    ultmpNetSeq.ulVal = htonl(pCON->order.tStopTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //��Ҫ���Ǹ�ֵ

    return 0;
}
static int makeCmdOrder(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdOrderBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_ORDER, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
}
#define ECH_ERR_VER     -1
#define ECH_ERR_CHECK   -2
#define ECH_ERR_ID      -3

static int recvResponse(void *pPObj,
                        void *pEObj,
                        uint8_t *pbuff,
                        uint32_t ulRecvdLen,
                        uint8_t trycountmax)
{
    echProtocol_t *pProto;
    EVSE_t *pE;
    echCmdElem_t echRecvCmdElem;
    us2uc ustmpNetSeq;
    ul2uc ultmpNetSeq;
    uint32_t ulMsgBodyLen_enc;
    uint16_t usChecksum;
    uint8_t EVSEID[8];
    uint32_t ulOffset;
    int i;

    pProto = (echProtocol_t *)pPObj;
    pE = (EVSE_t *)pEObj;
    ulOffset = 0;
    while(pbuff[ulOffset] != pProto->info.ucProtoVer)
    {
        ulOffset++;
        if(ulOffset == ulRecvdLen)
        {
            return ECH_ERR_VER;
        }
    }
//    if(pbuff[ulOffset] != pProto->info.ucProtoVer)
//    {
//        return ECH_ERR_VER;
//    }

    //pbuff[1] ����

    //pbuff[2,3] ������
    ustmpNetSeq.ucVal[0] = pbuff[ulOffset + 2];
    ustmpNetSeq.ucVal[1] = pbuff[ulOffset + 3];
    echRecvCmdElem.cmd.usRecvCmd = ntohs(ustmpNetSeq.usVal);

    //pbuff[4...7] Ԥ���ֶ�

    //pbuff[8...11] ��Ϣ�峤��
    ultmpNetSeq.ucVal[0] = pbuff[ulOffset + 8];
    ultmpNetSeq.ucVal[1] = pbuff[ulOffset + 9];
    ultmpNetSeq.ucVal[2] = pbuff[ulOffset + 10];
    ultmpNetSeq.ucVal[3] = pbuff[ulOffset + 11];
    ulMsgBodyLen_enc = ntohl(ultmpNetSeq.ulVal);

    //pbuff[12,13] Checksum
    ustmpNetSeq.ucVal[0] = pbuff[ulOffset + 12];
    ustmpNetSeq.ucVal[1] = pbuff[ulOffset + 13];
    usChecksum = ntohs(ustmpNetSeq.usVal);
    if(usChecksum != echVerifCheck(pProto->info.ucProtoVer, 0, echRecvCmdElem.cmd.usRecvCmd, ulMsgBodyLen_enc))
    {
        return ECH_ERR_CHECK;
    }
    StrToHex(&pbuff[ulOffset + 14], EVSEID, 16);
    if(memcmp(EVSEID, pE->info.ucID, 8) != 0 )
    {
        return ECH_ERR_ID;
    }
    echRecvCmdElem.timestamp = time(NULL);
    switch(echRecvCmdElem.cmd.usRecvCmd)
    {
    case 2://�����ظ����������Ҫtimeout ��λs��
        echRecvCmdElem.cmd_id = ECH_CMDID_REGISTER;
        echRecvCmdElem.timeout_s =  0xffffffff;
        break;
    case 4:
        echRecvCmdElem.cmd_id = ECH_CMDID_HEARTBEAT;
        echRecvCmdElem.timeout_s =  0xffffffff;
        break;
    case 42:
        echRecvCmdElem.cmd_id = ECH_CMDID_STATUS;
        echRecvCmdElem.timeout_s =  30;
        break;
    case 43:
        echRecvCmdElem.cmd_id = ECH_CMDID_REMOTE_CTRL;
        echRecvCmdElem.timeout_s =  30;
        break;
    case 47:
        echRecvCmdElem.cmd_id = ECH_CMDID_ORDER;
        echRecvCmdElem.timeout_s =  30;
        break;
    default:
        break;
    }
    echRecvCmdElem.len = ulMsgBodyLen_enc + 14;
    echRecvCmdElem.pbuff = pbuff;
    echRecvCmdElem.status = 0;
    echRecvCmdElem.trycount = 0;
    echRecvCmdElem.trycountmax = trycountmax;

    gdsl_list_insert_tail(pProto->plechRecvCmd, (void *)&echRecvCmdElem);

    if(ulRecvdLen - ulOffset > echRecvCmdElem.len)
    {
        recvResponse(pPObj, pEObj, &pbuff[ulOffset + echRecvCmdElem.len], ulRecvdLen - ulOffset - echRecvCmdElem.len, 3);
    }
    return 1;
}

/** @brief
 *
 * @param pPObj void*
 * @param usSendID uint16_t ��������ܻ����������⣩SendID��ʾ�յ��Ļظ���Ӧ��SendID
 * @param pbuff uint8_t*
 * @param ulRecvLen uint32_t
 * @return int
 *
 */
static int analyStdRes(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    uint8_t *pMsgBodyCtx_enc;
    uint8_t *pMsgBodyCtx_dec;
    uint32_t ulMsgBodyCtxLen_enc;
    ul2uc ultmpNetSeq;
    time_t timestamp;

    pProto = (echProtocol_t *)pPObj;
    pMsgBodyCtx_enc = pbuff + 30;         //ȡ�����ܲ���buff
    ulMsgBodyCtxLen_enc = ulRecvLen - 30; //���ܲ��ֳ���
    pMsgBodyCtx_dec = (uint8_t *)malloc(ulMsgBodyCtxLen_enc * sizeof(uint8_t));

    aes_decrypt(pMsgBodyCtx_enc, pProto->info.strKey, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);

    memmove(pProto->pCMD[usSendID]->ucRecvdOptData, pMsgBodyCtx_dec, ulMsgBodyCtxLen_enc);
    pProto->pCMD[usSendID]->uiRecvdOptLen = ulMsgBodyCtxLen_enc;

    free(pMsgBodyCtx_dec);

    return 1;
}

static int analyCmdReg(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
//    echProtocol_t *pProto;
//    uint8_t *pMsgBodyCtx_dec;
//
//    pProto = (echProtocol_t *)pPObj;

    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

//    pMsgBodyCtx_dec = pProto->pCMD[usSendID]->ucRecvdOptData;

    return 1;
}

static int analyCmdHeart(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    echProtocol_t *pProto;
    uint8_t *pMsgBodyCtx_dec;
    ul2uc ultmpNetSeq;
    time_t timestamp;

    pProto = (echProtocol_t *)pPObj;
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);
    pMsgBodyCtx_dec = pProto->pCMD[usSendID]->ucRecvdOptData;

    ultmpNetSeq.ucVal[0] = pMsgBodyCtx_dec[0];
    ultmpNetSeq.ucVal[1] = pMsgBodyCtx_dec[1];
    ultmpNetSeq.ucVal[2] = pMsgBodyCtx_dec[2];
    ultmpNetSeq.ucVal[3] = pMsgBodyCtx_dec[3];
    timestamp = (time_t)ntohl(ultmpNetSeq.ulVal);
    if(utils_abs(timestamp - time(NULL)) > 5)//����5s����Уʱ
    {
        time(&timestamp);
    }
    return 1;
}

static int analyCmdStatus(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

    return 1;
}
static int analyCmdRemoteCtrl(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);

    return 1;
}
static int analyCmdRTData(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);
    return 1;
}
static int analyCmdOrder(void *pPObj, uint16_t usSendID, uint8_t *pbuff, uint32_t ulRecvLen)
{
    analyStdRes(pPObj, usSendID, pbuff, ulRecvLen);
    return 1;
}
/** @brief ���ƴ������Ԫ�ص�������Ŀռ�
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
        memmove(copyCmdListElem, pechCmd, sizeof(echCmdElem_t));
    }
    else
    {
        return NULL;
    }
    copyCmdListElem->pbuff = (uint8_t *)malloc(copyCmdListElem->len * sizeof(uint8_t));
    if(copyCmdListElem->pbuff != NULL)
    {
        memmove(copyCmdListElem->pbuff, ((echCmdElem_t *)pechCmd)->pbuff, copyCmdListElem->len);
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
    ((echCmdElem_t *)e)->pbuff = NULL;
    e = NULL;
}

static void deleteProto(void *pPObj)
{
    int i;
    echProtocol_t *pProto;

    pProto = (echProtocol_t *)pPObj;
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
    strcpy(pProto->info.strServerIP, "124.207.112.70");//"123.56.113.123");//
    pProto->info.usServerPort      = 8051;//6677;//
    strcpy(pProto->info.strUserName, "esaasusr");
    strcpy(pProto->info.strUserPwd, "esaaspasswrd");
    strcpy(pProto->info.strKey, "1234567890abcde2");
    memset(pProto->info.strNewKey, 0, 17);
    pProto->info.tNewKeyChangeTime = 0;
    pProto->info.ucProtoVer        = 0x68;
    pProto->info.ulHeartBeatCyc_ms = 5000;
    pProto->info.ucResetAct        = 0;

    pProto->info.ulPowerFee_sharp    = 0; //������ ϵ��0.0001
    pProto->info.ulPowerFee_peak     = 0; //��
    pProto->info.ulPowerFee_shoulder = 0; //ƽ
    pProto->info.ulPowerFee_off_peak = 0; //��

    pProto->info.ulServiceFee_sharp    = 0; //ϵ��0.0001
    pProto->info.ulServiceFee_peak     = 0;
    pProto->info.ulServiceFee_shoulder = 0;
    pProto->info.ulServiceFee_off_peak = 0;

    pProto->info.ulStatusCyc_ms = 5000; //״̬�����ϱ����
    pProto->info.ulRTDataCyc_ms = 10000; //ʵʱ�����ϱ����  10s


    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        pProto->pCMD[i] = NULL;
    }
    //׮����, ƽ̨����
    pProto->pCMD[ECH_CMDID_REGISTER]  = EchCMDCreate(1, 2, makeCmdReg, analyCmdReg);
    pProto->pCMD[ECH_CMDID_HEARTBEAT] = EchCMDCreate(3, 4, makeCmdHeart, analyCmdHeart);
    pProto->pCMD[ECH_CMDID_STATUS]    = EchCMDCreate(41, 42, makeCmdStatus, analyCmdStatus);
    pProto->pCMD[ECH_CMDID_REMOTE_CTRL]    = EchCMDCreate(44, 43, makeCmdRemoteCtrl, analyCmdRemoteCtrl);
    pProto->pCMD[ECH_CMDID_RTDATA]    = EchCMDCreate(45, 0, makeCmdRTData, analyCmdRTData);
    pProto->pCMD[ECH_CMDID_ORDER] = EchCMDCreate(46, 47, makeCmdOrder, analyCmdOrder);

    pProto->recvResponse = recvResponse;
    pProto->sendCommand = sendCommand;
    pProto->deleteProtocol = deleteProto;

    pProto->plechRecvCmd = gdsl_list_alloc ("RecvCmd", echCmdListAlloc, echCmdListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("SendCmd", echCmdListAlloc, echCmdListFree);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses
