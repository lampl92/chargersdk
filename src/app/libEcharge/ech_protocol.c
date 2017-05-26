/**
* @file ech_protocol.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#include "utils.h"
#include "interface.h"
#include "lwip/sockets.h"
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
    echSendCmdElem.status = 0;//0表示未发送  1表示已发送
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

    //协议版本
    ucMsgHead[ulMsgHeadLen++] = pProto->info.ucProtoVer;
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
    ultmpNetSeq.ulVal = htonl((pE->info.ucIDLength << 1) + ulMsgBodyCtxLen_enc); //ucIDLength长度为Hex长度，应*2表示ASCII ID的长度
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[0];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[1];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[2];
    ucMsgHead[ulMsgHeadLen++] = ultmpNetSeq.ucVal[3];
    //消息校验码
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
    for(i = 0; i < pE->info.ucIDLength; i++)  //此处借用ulMsgHeadLen表示pucSendBuffer位置，运行之后ulMsgHeadLen表示的就是原ulMsgHeadLen + 桩号长度
    {
        sprintf(&(pucSendBuffer[ulMsgHeadLen]), "%02d", pE->info.ucID[i]);
        ulMsgHeadLen = ulMsgHeadLen + 2;
    }
    for(i = 0; i < ulMsgBodyCtxLen_enc; i++)
    {
        pucSendBuffer[ulMsgHeadLen + i] = ucMsgBodyCtx_enc[i];
    }

    *pulSendLength = ulMsgHeadLen + ulMsgBodyCtxLen_enc; //此处ulMsgHeadLen已经包含了桩号长度

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

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

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

    //充电桩接口   0：默认 1：A 2：B
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //预约状态 1：无预约  2:有预约
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
    //车位状态 1：空闲   2：占用   3：未知
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 3;
    //接口连接状态  1：充电 2:待机 3：故障 4：充电结束 5：未知
    switch(pCON->state)
    {
    case STATE_CON_CHARGING:
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 1;
        break;
    case STATE_CON_IDLE:
    case STATE_CON_PLUGED:
    case STATE_CON_PRECONTRACT:
    case STATE_CON_PRECONTRACT_LOSEPLUG:
    case STATE_CON_STARTCHARGE: //在这个状态还没开始充电
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
    //输出电压xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C 相电压
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
    //输出电流xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 10));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //A B C 相电流
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
    //继电器状态 1开，2关
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
    //系统风机工作状态 1：开机 2：关机
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //桩实时温度 xx.xxx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dACLTemp * 1000));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //有功功率 无功功率 xx.xxxx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //电能表有功电能 xx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //电表无功电能 xx.xx
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //SOC 1~100
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //故障码
    if(pEVSE->status.ulScramState == 1)
    {
        errcode |= 1 << 0; //Bit0 急停故障
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventException);
    if((uxBits & defEventBitExceptionMeter) == defEventBitExceptionMeter)
    {
        errcode |= 1 << 1; //Bit1 电表故障
    }
    if((uxBits & defEventBitExceptionRelayPaste) == defEventBitExceptionRelayPaste)
    {
        errcode |= 1 << 2; //Bit2 接触器故障
    }
    if((uxBits & defEventBitExceptionRFID) == defEventBitExceptionRFID)
    {
        errcode |= 1 << 3; //Bit3 读卡器故障
    }
    uxBits = xEventGroupGetBits(pCON->status.xHandleEventCharge);
    if((uxBits & defEventBitCONACTempOK) != defEventBitCONACTempOK)
    {
        errcode |= 1 << 4; //Bit4 内部过温故障
    }
    //errcode |= 1 << 5 //Bit5 连接器故障
    if(pEVSE->status.ulPEState == 1)
    {
        errcode |= 1 << 6; //Bit5 绝缘故障
    }
    if(pEVSE->status.ulPowerOffState == 1)
    {
        errcode |= 1 << 7;  //Bit7 其他(在此定义为停电故障)
    }
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = errcode;
    //充电桩当前时间
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

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
        //[0...3] 操作ID
        //[4...11] 交易流水号
        //[12] 充电桩接口
        //[13] 充电桩操作
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[i]; //不变
    }
    //[14] 启停结果
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[14];//在发送命令之前赋值的
    //[15] 失败原因
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[15];//在发送命令之前赋值的
    //[16] SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

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

    //[0...7] 交易流水号
    StrToHex(pCON->order.strOrderSN, ucOrderSN, strlen(pCON->order.strOrderSN));
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ucOrderSN[i];
    }
    //[8] 桩接口
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pCON->info.ucCONID + 1;
    //[9...12] 当前充电总电量 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPower * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[13...16] 尖电量 xxx.xx
    //[17...20] 峰电量
    //[21...24] 平电量
    //[25...28] 谷电量
    for(i = 0; i < 16; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[29...32] 当前充电金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalPowerFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[32...36] 当前服务费金额 xxx.xx
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->order.dTotalServiceFee * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[37,38] 当前充电时间
    ustmpNetSeq.usVal = htons(time(NULL) - pCON->order.tStartTime);
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ustmpNetSeq.ucVal[1];
    //[39] 充电桩状态
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[39];//注意修改这里也要修改interface_remote.c对应位置
    //[40] 停止原因
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = pbuff[40];
    //[41] 当前SOC
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[42,43] 剩余充电时间
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    //[44...47] 输出电压 xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingVoltage * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[48...51] 输出电流 xxx.x
    ultmpNetSeq.ulVal = htonl((uint32_t)(pCON->status.dChargingCurrent * 100));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[52...55] 电池组最低温度
    //[56...59] 电池组最高温度
    for(i = 0; i < 8; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }
    //[60...63] 当前时间戳
    ultmpNetSeq.ulVal = htonl(time(NULL));
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[0];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[1];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[2];
    pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = ultmpNetSeq.ucVal[3];
    //[64...80] 车辆VIN号
    for(i = 0; i < 17; i++)
    {
        pucMsgBodyCtx_dec[ulMsgBodyCtxLen_dec++] = 0;
    }

    *pulMsgBodyCtxLen_dec = ulMsgBodyCtxLen_dec; //不要忘记赋值

    return 0;
}
static int makeCmdRTData(void *pPObj, void *pEObj, void *pCObj, uint8_t *pucSendBuffer, uint32_t *pulSendLen)
{
    uint8_t ucMsgBodyCtx_dec[REMOTE_SENDBUFF_MAX];
    uint32_t ulMsgBodyCtxLen_dec;

    makeCmdRTDataBodyCtx(pPObj, pCObj, ucMsgBodyCtx_dec, &ulMsgBodyCtxLen_dec);
    makeStdCmd(pPObj, pEObj, ECH_CMDID_RTDATA, ucMsgBodyCtx_dec, ulMsgBodyCtxLen_dec, pucSendBuffer, pulSendLen);
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
    int i;

    pProto = (echProtocol_t *)pPObj;
    pE = (EVSE_t *)pEObj;

    if(pbuff[0] != pProto->info.ucProtoVer)
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
    if(usChecksum != echVerifCheck(pProto->info.ucProtoVer, 0, echRecvCmdElem.cmd.usRecvCmd, ulMsgBodyLen_enc))
    {
        return ECH_ERR_CHECK;
    }
    StrToHex(&pbuff[14], EVSEID, 16);
    if(memcmp(EVSEID, pE->info.ucID, 8) != 0 )
    {
        return ECH_ERR_ID;
    }
    echRecvCmdElem.timestamp = time(NULL);
    switch(echRecvCmdElem.cmd.usRecvCmd)
    {
    case 2://主机回复的命令，不需要timeout 单位s。
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
 * @param pPObj void*
 * @param usSendID uint16_t （这里可能会产生理解问题）SendID表示收到的回复对应的SendID
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
    pMsgBodyCtx_enc = pbuff + 30;         //取出加密部分buff
    ulMsgBodyCtxLen_enc = ulRecvLen - 30; //加密部分长度
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
    if(utils_abs(timestamp - time(NULL)) > 5)//大于5s进行校时
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
    strcpy(pProto->info.strServerIP, "124.207.112.70");
    pProto->info.usServerPort      = 8051;
    strcpy(pProto->info.strUserName, "esaasusr");
    strcpy(pProto->info.strUserPwd, "esaaspasswrd");
    strcpy(pProto->info.strKey, "0123456789abcdeg");
    memset(pProto->info.strNewKey, 0, 17);
    pProto->info.tNewKeyChangeTime = 0;
    pProto->info.ucProtoVer        = 0x68;
    pProto->info.ulHeartBeatCyc_ms = 5000;
    pProto->info.ucResetAct        = 0;

    pProto->info.ulPowerFee_sharp    = 0; //尖峰费率 系数0.0001
    pProto->info.ulPowerFee_peak     = 0; //峰
    pProto->info.ulPowerFee_shoulder = 0; //平
    pProto->info.ulPowerFee_off_peak = 0; //谷

    pProto->info.ulServiceFee_sharp    = 0; //系数0.0001
    pProto->info.ulServiceFee_peak     = 0;
    pProto->info.ulServiceFee_shoulder = 0;
    pProto->info.ulServiceFee_off_peak = 0;

    pProto->info.ulStatusCyc_ms = 5000; //状态数据上报间隔
    pProto->info.ulRTDataCyc_ms = 10000; //实时数据上报间隔  10s


    for(i = 0; i < ECH_CMD_MAX; i++)
    {
        pProto->pCMD[i] = NULL;
    }
    //桩命令, 平台命令
    pProto->pCMD[ECH_CMDID_REGISTER]  = EchCMDCreate(1, 2, makeCmdReg, analyCmdReg);
    pProto->pCMD[ECH_CMDID_HEARTBEAT] = EchCMDCreate(3, 4, makeCmdHeart, analyCmdHeart);
    pProto->pCMD[ECH_CMDID_STATUS]    = EchCMDCreate(41, 42, makeCmdStatus, analyCmdStatus);
    pProto->pCMD[ECH_CMDID_REMOTE_CTRL]    = EchCMDCreate(44, 43, makeCmdRemoteCtrl, analyCmdRemoteCtrl);
    pProto->pCMD[ECH_CMDID_RTDATA]    = EchCMDCreate(45, 0, makeCmdRTData, analyCmdRTData);

    pProto->recvResponse = recvResponse;
    pProto->sendCommand = sendCommand;
    pProto->deleteProtocol = deleteProto;

    pProto->plechRecvCmd = gdsl_list_alloc ("RecvCmd", echCmdListAlloc, echCmdListFree);
    pProto->plechSendCmd = gdsl_list_alloc ("SendCmd", echCmdListAlloc, echCmdListFree);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses
