/**
* @file RFIDReader_mt626.c
* @brief MT626 com driver
* @author rgw
* @version v1.0
* @date 2016-12-14
*/
#include <stdlib.h>
#include <string.h>
#include "RFIDReader_mt626.h"
#include "bsp_uart.h"


/** ��ֲ˵��������ϵͳʵ��MT626DelayMS��MT626Write ��MT626Read ��������*/


extern void vTaskDelay( const TickType_t xTicksToDelay );


/** @brief ��ʱ����,�������������еȴ�
 *
 * @param ms uint32_t ��ʱʱ��,��λ:����
 * @return void
 *
 */
static void MT626DelayMS(uint32_t ms)
{
    vTaskDelay(ms);
}
/** @brief
 *
 * @param data uint8_t* Ҫ���͵�����
 * @param len uint32_t Ҫ���͵ĳ���
 * @return uint32_t �ѷ��͵ĳ���
 *
 */     
static uint32_t MT626Write(uint8_t *data, uint32_t len)
{
    return uart_write(UART_PORT_RFID, data, len);
}
/** @brief
 *
 * @param data uint8_t* �����ݻ���
 * @param pRecvdLen uint8_t*  ��ȡ�����ݳ���
 * @return void
 *
 */     
static void MT626Read(uint8_t *data, uint8_t *pRecvdLen)
{
    readRecvQueEx(pRfidRecvQue, data, 0, pRecvdLen, 1);
}

/** @brief ������
 *
 * @param data uint8_t* ���ݴ���������ָ��
 * @param len uint32_t  ���������鳤��
 * @return uint8_t      �������ֵ
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

/** @brief ���������
 *
 * @param pObj void*            ���ݴ����������ʵ��
 * @param ucSendID uint8_t      ���������
 * @param ucSendLength uint32_t ���������
 * @return MT_RESULT            ����ͨѶ״̬
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

/** @brief �Խ��յ������ݷ����Ƿ���ȷ
 *
 * @param pObj void*            MT626ͨѶʵ��
 * @param ucSendID uint8_t      �����,�������ͷ�ļ�
 * @param pulRecvdLen uint32_t* ���ؽ��յ�����ĳ���
 * @return MT_RESULT            ����ͨѶ״̬
 *
 */
static MT_RESULT recvResponse(void *pObj, uint8_t ucSendID, uint32_t *pulRecvdLen)
{
    uint8_t *pucRecvBuffer;
    uint8_t ucRecvdBCC, ucCalcBCC;
    MT_RESULT res;

    ucRecvdBCC = 0;

    pucRecvBuffer = ((MT626COM_t *)pObj)->pucRecvBuffer;

    MT626Read(pucRecvBuffer, pulRecvdLen);
    if(*pulRecvdLen == 0)
    {
        return MT_COM_FAIL;
    }

    switch(ucSendID)
    {
    case MT626_FIND_CMD:                        //#0  Ѱ��
        if(*pulRecvdLen == 8)
        {
            res = MT_SUCCEED;
        }
        else
        {
            res = MT_COM_FAIL;
        }
        break;
    case MT626_READ_UID_CMD:                    //#1  ��ȡUID
        if(*pulRecvdLen == 12)
        {
            res = MT_SUCCEED;
        }
        else
        {
            res = MT_COM_FAIL;
        }
        break;
    case MT626_AUTH_KEYA_CMD:                   //#3  ��֤KeyA
    case MT626_AUTH_KEYB_CMD:                   //#4  ��֤KeyB
    case MT626_CHANGE_KEY_CMD:                  //#7  ��������
        if(*pulRecvdLen == 9)
        {
            res = MT_SUCCEED;
        }
        else
        {
            res = MT_COM_FAIL;
        }
        break;
    case MT626_READ_CMD:                        //#5  ������������
    case MT626_WRITE_CMD:                       //#6  д����������
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

    case MT626_INC_CMD:                         //#8  ��ֵ
    case MT626_DEC_CMD:                         //#9  ��ֵ
    case MT626_INIT_CMD:                        //#10 ��ʼ��
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

/** @brief ��������
 *
 * @param pObj void*                MT626ͨѶʵ��
 * @param ucSendID uint8_t          �����,�������ͷ�ļ�
 * @param pucOptionData uint8_t*    ���������ѡ����
 * @param ulOptionLen uint32_t      ���������ѡ���ݳ���
 * @param pucSendLength uint32_t*   ��������������ȵ�ָ��
 * @return int                      ��
 *
 */
static int makeStdCmd(void *pObj, uint8_t ucSendID, uint8_t *pucOptionData, uint32_t ulOptionLen, uint32_t *pucSendLength)
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

/** @brief ����Ѱ��\��֤KeyA\��֤KeyB\�޸�KeyB\��ֵ\��ֵ\��ʼ������
 *
 * @param pObj void*            MT626ͨѶʵ��
 * @param ucSendID uint8_t      �����,�������ͷ�ļ�
 * @param ulRecvLen uint32_t    ��Ҫ�����������
 * @return int                  ����״̬
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

/** @brief ������д����
 *
 * @param pObj void*            MT626ͨѶʵ��
 * @param ucSendID uint8_t      �����,�������ͷ�ļ�
 * @param ulRecvLen uint32_t    ��Ҫ�����������
 * @return int                  ����״̬
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
        pMT626CMDObj->uiRecvdOptLen = 4;
        break;
    case MT626_READ_CMD:
    case MT626_WRITE_CMD:
        ucState = pucRecvdCMD[7];
        if(ucState == MT_STATE_Y)
        {
            pMT626CMDObj->uiRecvdOptLen = 16;
        }
        else // ��д��������
        {
            return ucState;
        }
        break;
    default:
        return ucState;
    }
    for(i = 0; i < pMT626CMDObj->uiRecvdOptLen; i++)
    {
        pMT626CMDObj ->ucRecvdOptData[i] = pucRecvdCMD[(ulRecvLen - 2 - pMT626CMDObj->uiRecvdOptLen) + i]; //[ulRecvLen -2 - pMT626CMDObj->ulRecvdOptLen]Ϊ��������ʼ�±�
    }

    return ucState;
}

/** @brief MT626ͨѶ����
 *
 * @param pObj void*                MT626ͨѶʵ��ָ��
 * @param ucSendID uint8_t          �����,�������ͷ�ļ�
 * @param pucOptionData uint8_t*    ���������ѡ����
 * @param ulOptionLen uint32_t      ���������ѡ���ݳ���
 * @return int                      ����״̬
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

/** @brief ����MT626����
 *
 * @param ucParam uint8_t               �������
 * @param usLenght uint16_t             �����(������+�������+���ݰ�)
 * @param makeProc pMT626_MAKE_PROC     ���������ָ��
 * @param analyProc pMT626_ANALY_PROC   ���������ָ��
 * @return MT626CMD_t*                  ����ʵ��ָ��
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

/** @brief ����MT626ͨѶʵ��
 *
 * @param pObj void* MT626ͨѶʵ��
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

/** @brief ����MT626ͨѶʵ��
 *
 * @param void
 * @return MT626COM_t* ����MT626ͨѶʵ��ָ��
 *
 */
/** @todo (rgw#1#): ����ͨ�Žӿ� */
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

        //eg. �޷�������,�޷�������
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

        //eg. �޷�������,�з�������
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
        //eg. �з�������,�з�������
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
        vTaskDelay(500); //�������
    }
    pmt626com->deleteCOM(pmt626com);
}
#endif
