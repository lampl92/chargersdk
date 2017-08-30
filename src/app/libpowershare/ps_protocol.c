/**
* @file ps_protocol.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#include "polarssl/base64.h"
#include "ps_protocol.h"

static int makeStdCmd(void *pObj, uint8_t ucSendID, uint8_t *pucOptionData, uint32_t uiOptionLen, uint32_t *pucSendLength)
{
    uint8_t *pucSendBuffer;
    uint8_t ucOffset;
    PSCMD_t *pPSCMDObj;
    uint8_t i;

    pPSCMDObj = ((PSProtocol_t *)pObj)->pPSCMD[ucSendID];
    pucSendBuffer = ((MT626COM_t *)pObj)->pucSendBuffer;
    ucOffset = 0;
    pucSendBuffer[ucOffset++] = PS_CMD_STX;
    pucSendBuffer[ucOffset++] = (uint8_t)(pMT626CMDObj -> usLenght >> 8);
    pucSendBuffer[ucOffset++] = (uint8_t)(pMT626CMDObj -> usLenght);
    pucSendBuffer[ucOffset++] = MT626_CMD_TYPE;
    pucSendBuffer[ucOffset++] = pMT626CMDObj ->ucParam;
    if(uiOptionLen > 0 && pucOptionData != NULL)
    {
        for(i = 0; i < uiOptionLen; i++)
        {
            pucSendBuffer[ucOffset++] = pucOptionData[i];
        }
    }
    pucSendBuffer[ucOffset++] = MT626_CMD_ETX;
    bcc = verifBCC(pucSendBuffer, ucOffset);
    pucSendBuffer[ucOffset++] = bcc;

    *pucSendLength = ucOffset;

    return 0;
}

static PSCMD_t *PSCMDCreate(uint8_t ucCMD, pPS_MAKE_PROC makeProc, pPS_ANALY_PROC analyProc)
{
    PSCMD_t *pPSCMD = (PSCMD_t *)malloc(sizeof(PSCMD_t));
    if(pPSCMD == NULL)
    {
        return NULL;
    }
    pPSCMD->uCMD = ucCMD;
    pPSCMD->makeProc = makeProc;
    pPSCMD->analyProc = analyProc;

    return pPSCMD;
}

PSProtocol_t *PSProtocolCreate(void)
{
    int i;
    PSProtocol_t *pProto = (PSProtocol_t *)malloc(sizeof(PSProtocol_t));
    if(pProto == NULL)
    {
        return NULL;
    }

    for(i = 0; i < PS_CMD_MAX; i++)
    {
        pProto->pPSCMD[i] = NULL;
    }
    pProto->pPSCMD[PS_REGISTER_CMD]     = MT626CMDCreate(0x92, makeStdCmd, analyStdRes);
    pProto->pPSCMD[PS_LOCALSTART_CMD]   = MT626CMDCreate(0x02, makeStdCmd, analyRWRes);
    pProto->pPSCMD[PS_LOCALSTOP_CMD]    = MT626CMDCreate(0x04, makeStdCmd, analyStdRes);
    pProto->pPSCMD[PS_HEARTBEAT_CMD]    = MT626CMDCreate(0x98, makeStdCmd, analyStdRes);

    pProto ->recvResponse = recvResponse;
    pProto ->sendCommand = sendCommand;
    pProto ->deleteCOM = deleteCOM;

    pProto->pucSendBuffer = (uint8_t *)malloc(MT626_SENDBUFF_MAX * sizeof(uint8_t));
    memset(pProto->pucSendBuffer, 0, MT626_SENDBUFF_MAX);
    pProto->pucRecvBuffer = (uint8_t *)malloc(MT626_RECVBUFF_MAX * sizeof(uint8_t));
    memset(pProto->pucRecvBuffer, 0, MT626_RECVBUFF_MAX);

    return pProto;
}



//encode/decode protocol requests
//encode/decode protocol responses
