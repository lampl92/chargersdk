#ifndef __RFIDREADER_MT626_H
#define __RFIDREADER_MT626_H

#include "stm32f4xx.h"

#define MT626_CMD_MAX 10
#define MT626_SENDBUFF_MAX      100
#define MT626_RECVBUFF_MAX      100


#define MT626_CMD_STX                   0x02
#define MT626_CMD_ETX                   0x03
#define MT626_CMD_TYPE                  0x34

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



typedef enum
{
    MT_SUCCEED,
    MT_NOTAG,
    MT_LOGIN_SUCCEED,
    MT_LOGIN_FAIL,
    MT_READ_FAIL,
    MT_WRITE_FAIL,
    MT_SET_KEY_FAIL,
    MT_NOT_AUTH,
    MT_CARD_HALTED,
    MT_COM_FAIL,
    MT_FAIL
} MT_RESULT;

typedef    int (*pMT626_MAKE_PROC)  (void *pObj, uint8_t ucSendID, uint8_t *pOptionData, uint8_t ucOptionLen, uint8_t *pucSendLength);
typedef    int (*pMT626_ANALY_PROC) (void *pObj, uint8_t ucSendID, uint8_t ucRecvLen);


typedef struct _MT626CMD
{
    uint8_t     ucParam;
    uint16_t    usLenght;

    pMT626_MAKE_PROC  makeProc;
    pMT626_ANALY_PROC analyProc;

} MT626CMD_t;

typedef struct _MT626COM
{
    MT626CMD_t *pMT626CMD[MT626_CMD_MAX];
    uint8_t *pucSendBuffer;
    uint8_t *pucRecvBuffer;
    MT_RESULT (*sendCommand)(void *pObj, uint8_t ucSendID, uint8_t ucSendLenght);
    MT_RESULT (*recvResponse)(void *pObj, uint8_t ucSendID, uint8_t *pucRecvdLen);
    void (*deleteCOM)(void *pObj);
} MT626COM_t;


MT626COM_t *MT626COMCreate(void);

#endif
