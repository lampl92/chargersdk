#include <stdlib.h>
#include <string.h>
#include "RFIDReader_mt626.h"
#include "bsp_uart.h"

extern UART_HandleTypeDef RFID_UARTx_Handler;
extern HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
extern HAL_StatusTypeDef HAL_UART_Receive(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);

static void MT626DelayMS(uint32_t ms)
{
    vTaskDelay(ms);
}
    

static uint8_t verifBCC(uint8_t *data, uint8_t n)
{
    uint8_t out;
    uint8_t i;
    for(i = 0; i < n; i++)
    {
        out ^= data[i];
    }
    return out;
}
static int sendCommand(void *pObj, uint8_t ucSendID, uint8_t ucSendLength)
{
    uint8_t *pucSendBuffer;
    MT626COM_t *pMT626COMObj;

    pucSendBuffer = pMT626COMObj ->pucSendBuffer;

    HAL_UART_Transmit(&RFID_UARTx_Handler, (uint8_t *)&pucSendBuffer, ucSendLength, 0xFFFF);

    return 0;
}
static int recvResponse(uint8_t ucSendID, uint8_t ucState, uint8_t *pRcvData, uint8_t ucRecvLen)
{
    return 0;
}

static int makeStCmd(void *pObj, uint8_t ucSendID, uint8_t *pOptionData, uint8_t ucOptionLen, uint8_t *pucSendLength)
{
    uint8_t *pucSendBuffer;
    uint8_t ucOffset, bcc;
    MT626CMD_t *pMT626CMDObj;
    uint8_t i;

    pMT626CMDObj = ((MT626COM_t *)pObj)->pMT626CMD[ucSendID];
    pucSendBuffer = ((MT626COM_t *)pObj)->pucSendBuffer;
    ucOffset = 0;
    pucSendBuffer[ucOffset++] = MT626_CMD_STX;
    pucSendBuffer[ucOffset++] = (uint8_t)(pMT626CMDObj -> usLenght >> 8);
    pucSendBuffer[ucOffset++] = (uint8_t)(pMT626CMDObj -> usLenght);
    pucSendBuffer[ucOffset++] = MT626_CMD_TYPE;
    pucSendBuffer[ucOffset++] = pMT626CMDObj ->ucParam;
    if(ucOptionLen > 0)
    {
        for(i = 0; i < ucOptionLen; i++)
        {
            pucSendBuffer[ucOffset++] = pOptionData[i];
        }
    }
    pucSendBuffer[ucOffset++] = MT626_CMD_ETX;
    bcc = verifBCC(pucSendBuffer, ucOffset);
    pucSendBuffer[ucOffset++] = bcc;

    *pucSendLength = ucOffset;

    return 0;
}
static int analyStRes(void *pObj, uint8_t ucSendID, uint8_t ucState, uint8_t *pRcvData, uint8_t ucRecvLen)
{
    return 0;
}

int TransToMT626(void *pObj, uint8_t ucSendID, uint8_t *pOptionData, uint8_t ucOptionLen)
{
    MT626COM_t *pMT626COMObj;
    MT626CMD_t *pMT626CMDObj;
    uint8_t ucSendLength;
    int SentResult ;
    pMT626COMObj = (MT626COM_t *)pObj;
    pMT626CMDObj = pMT626COMObj->pMT626CMD[ucSendID];

    pMT626CMDObj ->makeProc(pObj, ucSendID, pOptionData, ucOptionLen, &ucSendLength);

    SentResult = pMT626COMObj ->sendCommand(pObj, ucSendID, ucSendLength);

    if(SentResult == 0)
    {
        //pMT626COMObj ->recvResponse();
    }

}
static MT626CMD_t *MT626CMDCreate(uint8_t ucParam, uint16_t usLenght, pMT626_MAKE_PROC makeProc, pMT626_ANALY_PROC analyProc)
{
    MT626CMD_t *pMT626CMD = (MT626CMD_t *)malloc(sizeof(MT626CMD_t));
    pMT626CMD->ucParam = ucParam;
    pMT626CMD->usLenght = usLenght;
    pMT626CMD->makeProc = makeProc;
    pMT626CMD->analyProc = analyProc;
    return pMT626CMD;
}
void deleteCOM(void *pObj)
{
    int i;
    MT626COM_t *pMT626COMObj;

    pMT626COMObj = (MT626COM_t *)pObj;
    for(i = 0; i < MT626_CMD_MAX; i++)
    {
        free(pMT626COMObj->pMT626CMD[i]);
    }
    free(pMT626COMObj ->pucRecvBuffer);
    free(pMT626COMObj ->pucSendBuffer);
    free(pMT626COMObj);
}
MT626COM_t *MT626COMCreate(void)//后续是否应添加参数, 参数包括UART实例,以初始化串口号与波特率
{
    int i;
    MT626COM_t *pMT626 = (MT626COM_t *)malloc(sizeof(MT626COM_t));

    for(i = 0; i < MT626_CMD_MAX; i++)
    {
        pMT626->pMT626CMD[i] = NULL;
    }
    pMT626->pMT626CMD[MT626_FIND_CMD]       = MT626CMDCreate(0x30, 0x02, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_READ_UID_CMD]   = MT626CMDCreate(0x31, 0x02, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_AUTH_KEYA_CMD]  = MT626CMDCreate(0x32, 0x09, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_AUTH_KEYB_CMD]  = MT626CMDCreate(0x39, 0x09, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_READ_CMD]       = MT626CMDCreate(0x33, 0x04, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_WRITE_CMD]      = MT626CMDCreate(0x34, 0x14, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_CHANGE_KEY_CMD] = MT626CMDCreate(0x35, 0x09, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_INC_CMD]        = MT626CMDCreate(0x37, 0x08, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_DEC_CMD]        = MT626CMDCreate(0x38, 0x08, makeStCmd, analyStRes);
    pMT626->pMT626CMD[MT626_INIT_CMD]       = MT626CMDCreate(0x36, 0x08, makeStCmd, analyStRes);

    pMT626 ->recvResponse = recvResponse;
    pMT626 ->sendCommand = sendCommand;
    pMT626 ->deleteCOM = deleteCOM;

    pMT626->pucSendBuffer = (uint8_t *)malloc(MT626_SENDBUFF_MAX * sizeof(uint8_t));
    memset(pMT626->pucSendBuffer, 0, MT626_SENDBUFF_MAX);
    pMT626->pucRecvBuffer = (uint8_t *)malloc(MT626_RECVBUFF_MAX * sizeof(uint8_t));
    memset(pMT626->pucRecvBuffer, 0, MT626_RECVBUFF_MAX);

    return pMT626;
}
MT_RESULT mt626_find(void)
{
    return MT_SUCCEED;
}

MT_RESULT mt626_ReadUID(uint8_t *pUID)
{

    pUID[0] = 0xAB;
    return MT_SUCCEED;
}
