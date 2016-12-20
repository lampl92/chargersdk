#ifndef __RFIDREADER_MT626_H
#define __RFIDREADER_MT626_H

#include "stm32f4xx.h"

#define MT626_CMD_MAX                   10  //命令数量
#define MT626_SENDBUFF_MAX              100 //发送缓冲长度
#define MT626_RECVBUFF_MAX              100 //接收缓冲长度


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


#define MT_STATE_Y                      0x59    //'Y' 成功
#define MT_STATE_N                      0x4E    //'N' 寻卡失败/获取UID失败
#define MT_STATE_AUTH_ERR               0x33    //'3' 验证密码错误
#define MT_STATE_SECTOR_ERR             0x31    //'1' 操作扇区号错误(不是验证密码后的扇区)
#define MT_STATE_DATA_ERR               0x34    //'4' 读/写数据失败,更改密码失败,增/减/初始化失败


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

//typedef union _byteword
//{
//	uint8_t	 byte[2];
//	uint16_t word;
//} BYTEWORD, *PBYTEWORD;

typedef    int (*pMT626_MAKE_PROC)  (void *pObj, uint8_t ucSendID, uint8_t *pOptionData, uint32_t uiOptionLen, uint32_t *pucSendLength);
typedef    int (*pMT626_ANALY_PROC) (void *pObj, uint8_t ucSendID, uint32_t uiRecvLen);

typedef struct _MT626CMD
{
    uint8_t     ucParam;
    uint16_t    usLenght;
    uint8_t     ucRecvdOptData[16];//最长16个数据
    uint32_t    uiRecvdOptLen;

    pMT626_MAKE_PROC  makeProc;
    pMT626_ANALY_PROC analyProc;

} MT626CMD_t;

typedef struct _MT626COM
{
    MT626CMD_t *pMT626CMD[MT626_CMD_MAX];
    uint8_t *pucSendBuffer;
    uint8_t *pucRecvBuffer;
    MT_RESULT (*sendCommand)(void *pObj, uint8_t ucSendID, uint32_t ucSendLenght);
    MT_RESULT (*recvResponse)(void *pObj, uint8_t ucSendID, uint32_t *pucRecvdLen);
    void (*deleteCOM)(void *pObj);
} MT626COM_t;

int TransToMT626(void *pObj, uint8_t ucSendID, uint8_t *pucOptionData, uint32_t uiOptionLen);
MT626COM_t *MT626COMCreate(void);

#endif
