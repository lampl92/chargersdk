#include <stdlib.h>
#include <string.h>
#include "RFIDReader_mt626.h"
#include "bsp_uart.h"

extern UART_HandleTypeDef RFID_UARTx_Handler;
extern Queue *pRfidRecvQue;
extern HAL_StatusTypeDef HAL_UART_Transmit(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size, uint32_t Timeout);
extern void vTaskDelay( const TickType_t xTicksToDelay );


static int MT626DelayMS(uint32_t ms)
{
    vTaskDelay(ms);
    return 1;
}

static uint8_t verifBCC(uint8_t *data, uint32_t len)
{
    uint8_t out;
    uint32_t i;
    for(i = 0; i < len; i++)
    {
        out ^= data[i];
    }
    return out;
}
static MT_RESULT sendCommand(void *pObj, uint8_t ucSendID, uint32_t ucSendLength)
{
    const uint32_t ucTimeOutMS = 1000, uiTryTimes = 3;
    uint32_t ucFailedCounts;
    uint8_t *pucSendBuffer;
    MT626COM_t *pMT626COMObj;
    HAL_StatusTypeDef hal_res;

    ucFailedCounts = 0;
    pucSendBuffer = pMT626COMObj ->pucSendBuffer;
    do
    {
        hal_res = HAL_UART_Transmit(&RFID_UARTx_Handler, pucSendBuffer, ucSendLength, 0xFFFF);
        if(hal_res != HAL_OK)
        {
            ucFailedCounts++;
            MT626DelayMS(ucTimeOutMS);
        }
    }
    while(hal_res != HAL_OK && ucFailedCounts < uiTryTimes);
    if(hal_res != HAL_OK || ucFailedCounts == uiTryTimes)
    {
        return MT_COM_FAIL;
    }
    else
    {
        return MT_SUCCEED;
    }

}
#define MT626_FIND_CMD                  0   //#0  寻卡
#define MT626_READ_UID_CMD              1   //#1  获取UID       
#define MT626_AUTH_KEYA_CMD             2   //#3  验证KeyA
#define MT626_AUTH_KEYB_CMD             3   //#4  验证KeyB
#define MT626_READ_CMD                  4   //#5  读扇区块数据
#define MT626_WRITE_CMD                 5   //#6  写扇区块数据
#define MT626_CHANGE_KEY_CMD            6   //#7  更改密码
#define MT626_INC_CMD                   7   //#8  增值
#define MT626_DEC_CMD                   8   //#9  减值
#define MT626_INIT_CMD                  9   //#10 初始化

static MT_RESULT recvReadEx(void *pObj, uint32_t *pucRecvdLen)
{
    uint8_t ch;
    uint32_t i;
    MT626COM_t *pMT626COM;

    pMT626COM = (MT626COM_t *)pObj;
    ch = 0;
    i = 0;

    while(readRecvQue(pRfidRecvQue, &ch, 1) == 0)
    {
        pMT626COM->pucRecvBuffer[i] = ch;
        i++;
    }
    *pucRecvdLen = i;
    return MT_SUCCEED;
}

static MT_RESULT recvResponse(void *pObj, uint8_t ucSendID, uint32_t *pucRecvdLen)
{
    uint8_t *pucRecvBuffer;
    uint8_t ucRecvdBCC, ucCalcBCC;
    MT_RESULT res;

    ucRecvdBCC = 0;

    pucRecvBuffer = ((MT626COM_t *)pObj)->pucRecvBuffer;

    recvReadEx(pObj, pucRecvdLen);
    if(*pucRecvdLen == 0)
    {
        return MT_COM_FAIL;
    }

    switch(ucSendID)
    {
        case MT626_FIND_CMD:                        //#0  寻卡
            if(*pucRecvdLen == 8)
            {
                res = MT_SUCCEED;
            }
            else
            {
                res = MT_COM_FAIL;
            }
            break;
        case MT626_READ_UID_CMD:                    //#1  获取UID
            if(*pucRecvdLen == 12)
            {
                res = MT_SUCCEED;
            }
            else
            {
                res = MT_COM_FAIL;
            }
            break;
        case MT626_AUTH_KEYA_CMD:                   //#3  验证KeyA
        case MT626_AUTH_KEYB_CMD:                   //#4  验证KeyB
        case MT626_CHANGE_KEY_CMD:                  //#7  更改密码
            if(*pucRecvdLen == 9)
            {
                res = MT_SUCCEED;
            }
            else
            {
                res = MT_COM_FAIL;
            }
            break;
        case MT626_READ_CMD:                        //#5  读扇区块数据
        case MT626_WRITE_CMD:                       //#6  写扇区块数据
            if(*pucRecvdLen == 26)
            {
                res = MT_SUCCEED;
            }
            else if(*pucRecvdLen == 10)
            {
                res = MT_SUCCEED;
            }
            else
            {
                res = MT_COM_FAIL;
            }
            break;

        case MT626_INC_CMD:                         //#8  增值
        case MT626_DEC_CMD:                         //#9  减值
        case MT626_INIT_CMD:                        //#10 初始化
            if(*pucRecvdLen == 10)
            {
                res = MT_SUCCEED;
            }
            else
            {
                res = MT_COM_FAIL;
            }
            break;
    }
    if(res == MT_SUCCEED)
    {
        ucRecvdBCC = pucRecvBuffer[*pucRecvdLen - 1];
        ucCalcBCC = verifBCC(pucRecvBuffer, *pucRecvdLen - 1);
        if(ucRecvdBCC == ucCalcBCC)
        {
            return MT_SUCCEED;
        }
        else
        {
            return MT_COM_FAIL;
        }
    }
    else
    {
        return MT_COM_FAIL;
    }

}

static int makeStCmd(void *pObj, uint8_t ucSendID, uint8_t *pOptionData, uint32_t ucOptionLen, uint32_t *pucSendLength)
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
//static int analyFindRes(void *pObj, uint8_t ucSendID, uint8_t ucRecvLen)
//{
//    return 0;
//}
//static int analyReadUIDRes(void *pObj, uint8_t ucSendID, uint8_t ucRecvLen)
//{
//    return 0;
//}

//static int analyKeyRes(void *pObj, uint8_t ucSendID, uint8_t ucRecvLen)
//{
//    return 0;
//}
////r,w
//static int analyRWDataRes(void *pObj, uint8_t ucSendID, uint8_t ucRecvLen)
//{
//    return 0;
//}
//Inc,dec,err,init
static int analyStRes(void *pObj, uint8_t ucSendID, uint32_t ucRecvLen)
{
    
    return 0;
}



int TransToMT626(void *pObj, uint8_t ucSendID, uint8_t *pucOptionData, uint32_t ucOptionLen)
{
    const uint32_t ucTimeOutMS = 5000, uiTryTimes = 3;
    uint32_t ucFailedCounts;
    MT626COM_t *pMT626COMObj;
    MT626CMD_t *pMT626CMDObj;
    uint32_t ucSendLength, ucRcvdLength;
    MT_RESULT res ;

    ucFailedCounts = 0;

    pMT626COMObj = (MT626COM_t *)pObj;
    pMT626CMDObj = pMT626COMObj->pMT626CMD[ucSendID];

    pMT626CMDObj ->makeProc(pMT626COMObj, ucSendID, pucOptionData, ucOptionLen, &ucSendLength);
    do
    {
        res = pMT626COMObj ->sendCommand(pMT626COMObj, ucSendID, ucSendLength);
        if(res != MT_SUCCEED)
        {
            return MT_COM_FAIL;
        }

        res = pMT626COMObj ->recvResponse(pMT626COMObj, ucSendID, &ucRcvdLength);
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

    return pMT626CMDObj->analyProc(pMT626COMObj, ucSendID, ucRcvdLength);

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
    if(pMT626 == NULL)
    {
        return NULL;
    }

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
