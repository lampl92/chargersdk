/**
* @file RFIDReader_mt626.c
* @brief MT626 com driver
* @author rgw
* @version v1.0
* @date 2016-12-14
*/
#include "RFIDReader_mt626.h"
#include "bsp_uart.h"
#include "retarget.h"
#include "evse_debug.h"
#include <stdlib.h>
#include <string.h>

/** 移植说明：根据系统实现MT626DelayMS，MT626UartInit, MT626UartDeInit, MT626Write，MT626Read 函数*/
#include "cmsis_os.h"

/** @brief 延时函数,用于命令传输过程中等待
 *
 * @param ms uint32_t 延时时间,单位:毫秒
 * @return void
 *
 */
static void MT626DelayMS(uint32_t ms)
{
    osDelay(ms);
}

/** @brief
 *
 * @param data uint8_t* 要发送的数据
 * @param len uint32_t 要发送的长度
 * @return uint32_t 已发送的长度
 *
 */     
#include "evse_globals.h"
static uint32_t MT626Write(uint8_t *data, uint32_t len)
{
    return uart_write_fast(pRFIDDev->uart_handle, data, len);
}
/** @brief
 *
 * @param data uint8_t* 读数据缓存
 * @param pRecvdLen uint8_t*  读取的数据长度
 * @return void
 *
 */
static void MT626Read(uint8_t *data, uint32_t *pRecvdLen)
{
    *pRecvdLen = uart_read_wait(pRFIDDev->uart_handle, data, MT626_RECVBUFF_MAX, 10);
}

/** @brief 异或计算
 *
 * @param data uint8_t* 传递待计算数组指针
 * @param len uint32_t  待计算数组长度
 * @return uint8_t      返回异或值
 *
 */
static uint8_t verifBCC(uint8_t *data, uint32_t len)
{
    uint8_t out = 0;
    uint32_t i;
    for(i = 0; i < len; i++)
    {
        out ^= data[i];
    }
    return out;
}

/** @brief 发送命令函数
 *
 * @param pObj void*            传递待发送命令的实例
 * @param ucSendID uint8_t      发送命令号
 * @param ucSendLength uint32_t 发送命令长度
 * @return MT_RESULT            返回通讯状态
 *
 */
static MT_RESULT sendCommand(void *pObj, uint8_t ucSendID, uint32_t ucSendLength)
{
    const uint32_t ucTimeOutMS = 1000, uiTryTimes = 3;
    uint32_t ucFailedCounts;
    MT626COM_t *pMT626COMObj;
    uint8_t *pucSendBuffer;
    uint32_t btw;

    ucFailedCounts = 0;
    pMT626COMObj = (MT626COM_t *)pObj;
    pucSendBuffer = pMT626COMObj ->pucSendBuffer;

    do
    {
        btw =  MT626Write(pucSendBuffer, ucSendLength);
        if(btw != ucSendLength)
        {
            ucFailedCounts++;
            MT626DelayMS(ucTimeOutMS);
        }
    }
    while(btw != ucSendLength && ucFailedCounts < uiTryTimes);
    if(btw != ucSendLength || ucFailedCounts == uiTryTimes)
    {
        return MT_COM_FAIL;
    }
    else
    {
        return MT_SUCCEED;
    }

}

/** @brief 对接收到的数据分析是否正确
 *
 * @param pObj void*            MT626通讯实例
 * @param ucSendID uint8_t      命令号,定义详见头文件
 * @param pulRecvdLen uint32_t* 返回接收到命令的长度
 * @return MT_RESULT            返回通讯状态
 *
 */
static MT_RESULT recvResponse(void *pObj, uint8_t ucSendID, uint32_t *pulRecvdLen)
{
    uint8_t *pucRecvBuffer;
    uint8_t ucRecvdBCC, ucCalcBCC;
    MT_RESULT res;
    int i;

    ucRecvdBCC = 0;

    pucRecvBuffer = ((MT626COM_t *)pObj)->pucRecvBuffer;

    MT626Read(pucRecvBuffer, pulRecvdLen);
    if(*pulRecvdLen == 0)
    {
        return MT_COM_FAIL;
    }
    switch(ucSendID)
    {
    case MT626_FIND_CMD:                        //#0  寻卡
        if(*pulRecvdLen == 8)
        {
            res = MT_SUCCEED;
        }
        else
        {
            res = MT_COM_FAIL;
        }
        break;
    case MT626_READ_UID_CMD:                    //#1  获取UID
        if(*pulRecvdLen == 12)
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
        if(*pulRecvdLen == 9)
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
        if(*pulRecvdLen == 26)
        {
            res = MT_SUCCEED;
        }
        else if(*pulRecvdLen == 10)
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
        if(*pulRecvdLen == 10)
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
        ucRecvdBCC = pucRecvBuffer[*pulRecvdLen - 1];
        ucCalcBCC = verifBCC(pucRecvBuffer, *pulRecvdLen - 1);
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

/** @brief 制作命令
 *
 * @param pObj void*                MT626通讯实例
 * @param ucSendID uint8_t          命令号,定义详见头文件
 * @param pucOptionData uint8_t*    发送命令可选数据
 * @param ulOptionLen uint32_t      发送命令可选数据长度
 * @param pucSendLength uint32_t*   返回制作的命令长度的指针
 * @return int                      无
 *
 */
static int makeStdCmd(void *pObj, uint8_t ucSendID, uint8_t *pucOptionData, uint32_t ulOptionLen, uint32_t *pucSendLength)
{
    uint8_t *pucSendBuffer;
    uint8_t ucOffset, bcc;
    MT626CMD_t *pMT626CMDObj;
    int i;

    pMT626CMDObj = ((MT626COM_t *)pObj)->pMT626CMD[ucSendID];
    pucSendBuffer = ((MT626COM_t *)pObj)->pucSendBuffer;
    ucOffset = 0;
    pucSendBuffer[ucOffset++] = MT626_CMD_STX;
    pucSendBuffer[ucOffset++] = (uint8_t)(pMT626CMDObj -> usLenght >> 8);
    pucSendBuffer[ucOffset++] = (uint8_t)(pMT626CMDObj -> usLenght);
    pucSendBuffer[ucOffset++] = MT626_CMD_TYPE;
    pucSendBuffer[ucOffset++] = pMT626CMDObj ->ucParam;
    if(ulOptionLen > 0 && pucOptionData != NULL)
    {
        for(i = 0; i < ulOptionLen; i++)
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

/** @brief 解析寻卡\验证KeyA\验证KeyB\修改KeyB\增值\减值\初始化命令
 *
 * @param pObj void*            MT626通讯实例
 * @param ucSendID uint8_t      命令号,定义详见头文件
 * @param ulRecvLen uint32_t    需要解析的命令长度
 * @return int                  命令状态
 *
 */
static int analyStdRes(void *pObj, uint8_t ucSendID, uint32_t ulRecvLen)
{
    uint8_t *pucRecvdCMD;
    uint8_t ucState;

    pucRecvdCMD = ((MT626COM_t *)pObj)->pucRecvBuffer;
    ucState = MT_STATE_N;

    switch(ucSendID)
    {
    case MT626_FIND_CMD:
        ucState = pucRecvdCMD[5];
        break;

    case MT626_AUTH_KEYA_CMD:
    case MT626_AUTH_KEYB_CMD:
    case MT626_CHANGE_KEY_CMD:
        ucState = pucRecvdCMD[6];
        break;

    case MT626_INC_CMD:
    case MT626_DEC_CMD:
    case MT626_INIT_CMD:
        ucState = pucRecvdCMD[7];
        break;
    default:
        break;
    }

    return ucState;
}

/** @brief 解析读写命令
 *
 * @param pObj void*            MT626通讯实例
 * @param ucSendID uint8_t      命令号,定义详见头文件
 * @param ulRecvLen uint32_t    需要解析的命令长度
 * @return int                  命令状态
 *
 */
static int analyRWRes(void *pObj, uint8_t ucSendID, uint32_t ulRecvLen)
{
    MT626COM_t *pMT626COMObj;
    MT626CMD_t *pMT626CMDObj;
    uint8_t *pucRecvdCMD;
    uint8_t ucState;
    uint32_t i;

    pMT626COMObj = (MT626COM_t *)pObj;
    pMT626CMDObj = pMT626COMObj->pMT626CMD[ucSendID];
    pucRecvdCMD = pMT626COMObj->pucRecvBuffer;

    switch(ucSendID)
    {
    case MT626_READ_UID_CMD:
        ucState = pucRecvdCMD[5];
        pMT626CMDObj->ulRecvdOptLen = 4;
        break;
    case MT626_READ_CMD:
    case MT626_WRITE_CMD:
        ucState = pucRecvdCMD[7];
        if(ucState == MT_STATE_Y)
        {
            pMT626CMDObj->ulRecvdOptLen = 16;
        }
        else // 读写扇区错误
        {
            return ucState;
        }
        break;
    default:
        return ucState;
    }
    for(i = 0; i < pMT626CMDObj->ulRecvdOptLen; i++)
    {
        pMT626CMDObj ->ucRecvdOptData[i] = pucRecvdCMD[(ulRecvLen - 2 - pMT626CMDObj->ulRecvdOptLen) + i]; //[ulRecvLen -2 - pMT626CMDObj->ulRecvdOptLen]为数据区起始下标
    }

    return ucState;
}

/** @brief MT626通讯函数
 *
 * @param pObj void*                MT626通讯实例指针
 * @param ucSendID uint8_t          命令号,定义详见头文件
 * @param pucOptionData uint8_t*    发送命令可选数据
 * @param ulOptionLen uint32_t      发送命令可选数据长度
 * @return int                      命令状态
 *
 */
int TransToMT626(void *pObj, uint8_t ucSendID, uint8_t *pucOptionData, uint32_t ulOptionLen)
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

    pMT626CMDObj ->makeProc(pMT626COMObj, ucSendID, pucOptionData, ulOptionLen, &ucSendLength);
    do
    {
        res = pMT626COMObj ->sendCommand(pMT626COMObj, ucSendID, ucSendLength);
        if(res != MT_SUCCEED)
        {
            return MT_COM_FAIL;
        }
        MT626DelayMS(100);
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

/** @brief 构造MT626命令
 *
 * @param ucParam uint8_t               命令参数
 * @param usLenght uint16_t             命令长度(命令字+命令参数+数据包)
 * @param makeProc pMT626_MAKE_PROC     制作命令函数指针
 * @param analyProc pMT626_ANALY_PROC   解析命令函数指针
 * @return MT626CMD_t*                  命令实例指针
 *
 */
static MT626CMD_t *MT626CMDCreate(uint8_t ucParam, uint16_t usLenght, pMT626_MAKE_PROC makeProc, pMT626_ANALY_PROC analyProc)
{
    MT626CMD_t *pMT626CMD = (MT626CMD_t *)malloc(sizeof(MT626CMD_t));
    if(pMT626CMD == NULL)
    {
        return NULL;
    }
    pMT626CMD->ucParam = ucParam;
    pMT626CMD->usLenght = usLenght;
    pMT626CMD->makeProc = makeProc;
    pMT626CMD->analyProc = analyProc;

    return pMT626CMD;
}

/** @brief 析构MT626通讯实例
 *
 * @param pObj void* MT626通讯实例
 * @return void
 *
 */
static void deleteCOM(void *pObj)
{
    int i;
    MT626COM_t *pMT626COMObj;

    pMT626COMObj = (MT626COM_t *)pObj;
    for(i = 0; i < MT626_CMD_MAX; i++)
    {
        if(pMT626COMObj->pMT626CMD[i] != NULL)
        {
            free(pMT626COMObj->pMT626CMD[i]);
            pMT626COMObj->pMT626CMD[i] = NULL;
        }
    }
    free(pMT626COMObj ->pucRecvBuffer);
    free(pMT626COMObj ->pucSendBuffer);
    free(pMT626COMObj);
    pMT626COMObj = NULL;
}

/** @brief 构造MT626通讯实例
 *
 * @param void
 * @return MT626COM_t* 返回MT626通讯实例指针
 *
 */
MT626COM_t *MT626COMCreate(void)
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
    pMT626->pMT626CMD[MT626_FIND_CMD]       = MT626CMDCreate(0x30, 0x02, makeStdCmd, analyStdRes);
    pMT626->pMT626CMD[MT626_READ_UID_CMD]   = MT626CMDCreate(0x31, 0x02, makeStdCmd, analyRWRes);
    pMT626->pMT626CMD[MT626_AUTH_KEYA_CMD]  = MT626CMDCreate(0x32, 0x09, makeStdCmd, analyStdRes);
    pMT626->pMT626CMD[MT626_AUTH_KEYB_CMD]  = MT626CMDCreate(0x39, 0x09, makeStdCmd, analyStdRes);
    pMT626->pMT626CMD[MT626_READ_CMD]       = MT626CMDCreate(0x33, 0x04, makeStdCmd, analyRWRes);
    pMT626->pMT626CMD[MT626_WRITE_CMD]      = MT626CMDCreate(0x34, 0x14, makeStdCmd, analyRWRes);
    pMT626->pMT626CMD[MT626_CHANGE_KEY_CMD] = MT626CMDCreate(0x35, 0x09, makeStdCmd, analyStdRes);
    pMT626->pMT626CMD[MT626_INC_CMD]        = MT626CMDCreate(0x37, 0x08, makeStdCmd, analyStdRes);
    pMT626->pMT626CMD[MT626_DEC_CMD]        = MT626CMDCreate(0x38, 0x08, makeStdCmd, analyStdRes);
    pMT626->pMT626CMD[MT626_INIT_CMD]       = MT626CMDCreate(0x36, 0x08, makeStdCmd, analyStdRes);

    pMT626 ->recvResponse = recvResponse;
    pMT626 ->sendCommand = sendCommand;
    pMT626 ->deleteCOM = deleteCOM;

    pMT626->pucSendBuffer = (uint8_t *)malloc(MT626_SENDBUFF_MAX * sizeof(uint8_t));
    memset(pMT626->pucSendBuffer, 0, MT626_SENDBUFF_MAX);
    pMT626->pucRecvBuffer = (uint8_t *)malloc(MT626_RECVBUFF_MAX * sizeof(uint8_t));
    memset(pMT626->pucRecvBuffer, 0, MT626_RECVBUFF_MAX);

    return pMT626;
}
#if 0
void testmt626(void)
{
    MT626COM_t *pmt626com;
    int state, i;
    uint8_t *precvdData;
    uint8_t ucSendData[16] = {  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                                1, 2, 3, 4, 5, 6
                             };
    uint32_t ulRecvdOptLen;

    pmt626com = MT626COMCreate();
    precvdData = NULL;
    ulRecvdOptLen = 0;

    while(1)
    {

        //eg. 无发送数据,无返回数据
        state = TransToMT626(pmt626com, MT626_FIND_CMD, NULL, 0);
        if(state == MT_STATE_Y)
        {
            // do something...
            printf_safe("Find card.\n ");
        }
        else if(state == MT_STATE_N)
        {
            printf_safe("No card.\n");
            // do something...
        }

        //eg. 无发送数据,有返回数据
        state = TransToMT626(pmt626com, MT626_READ_UID_CMD, NULL, 0);
        if(state == MT_STATE_Y)
        {
            // do something...
            ulRecvdOptLen = pmt626com->pMT626CMD[MT626_READ_UID_CMD]->uiRecvdOptLen;
            precvdData = (uint8_t *)malloc(ulRecvdOptLen * sizeof(uint8_t));
            for(i = 0; i < ulRecvdOptLen; i++)
            {
                precvdData[i] = pmt626com->pMT626CMD[MT626_READ_UID_CMD]->ucRecvdOptData[i];
            }
            // use precvdData to do sth...
            printf_safe("UID = ");
            for(i = 0; i < ulRecvdOptLen; i++)
            {
                printf_safe("%x",precvdData[i]);
            }
            printf_safe("\n");
            // then...
            free(precvdData);
        }
        else if(state == MT_STATE_N)
        {
            //do something...
        }
#if 0
        //eg. 有发送数据,有返回数据
        state = TransToMT626(pmt626com, MT626_WRITE_CMD, ucSendData, sizeof(ucSendData));
        if(state == MT_STATE_Y)
        {
            // do something...
            ulRecvdOptLen = pmt626com->pMT626CMD[MT626_WRITE_CMD]->uiRecvdOptLen;
            precvdData = (uint8_t *)malloc(ulRecvdOptLen * sizeof(uint8_t));
            for(i = 0; i < ulRecvdOptLen; i++)
            {
                precvdData[i] = pmt626com->pMT626CMD[MT626_WRITE_CMD]->ucRecvdOptData[i];
            }
            // use precvdData to do sth...
            // then...
            free(precvdData);
        }
        else if(state == MT_STATE_N)
        {
            //do something...
        }
#endif
        vTaskDelay(500); //任务调度
    }
    pmt626com->deleteCOM(pmt626com);
}
#endif
