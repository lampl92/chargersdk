#include <stdlib.h>
#include "RFIDReader_mt626.h"


int makeStCmd(void *pObj, uint8_t ucSendID, uint8_t ucParam,uint16_t ucLenght,uint8_t *pOptionData,uint8_t ucOptionLen)
{
    MT626CMD_t *pMT626IfObj;
    
    pMT626IfObj = (MT626CMD_t *)pObj;
    
}
int analyStRes(void *pObj, uint8_t ucSendID, uint8_t ucParam,uint16_t ucLenght,uint8_t ucState,uint8_t *pRcvData, uint8_t ucRecvLen)
{
    return 0;
}

MT626CMD_t *MT626CMDCreate(uint8_t ucParam,uint16_t usLenght,MT626_MAKE_PROC makeProc,MT626_ANALY_PROC analyProc)
{
    MT626CMD_t *pMT626CMD = (MT626CMD_t *)malloc(sizeof(MT626CMD_t));
    pMT626CMD->ucParam = ucParam;
    pMT626CMD->usLenght = usLenght;
    pMT626CMD->makeProc = makeProc;
    pMT626CMD->analyProc = analyProc;
    return pMT626CMD;
}

/*
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
*/
MT626COM_t *MT626COMCreate()
{
    int i;
    MT626COM_t *pMT626 = (MT626COM_t *)malloc(sizeof(MT626COM_t));
    for(i = 0;i< MT626_CMD_MAX;i++)
    {
        pMT626->pMT626CMD[i] = NULL;
    }
    pMT626->pMT626CMD[MT626_FIND_CMD]       = MT626CMDCreate(0x30, 2, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_READ_UID_CMD]   = MT626CMDCreate(0x31, 2, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_AUTH_KEYA_CMD]  = MT626CMDCreate(0x32, 9, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_AUTH_KEYB_CMD]  = MT626CMDCreate(0x39, 9, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_READ_CMD]       = MT626CMDCreate(0x33, 4, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_WRITE_CMD]      = MT626CMDCreate(0x34, 0x14, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_CHANGE_KEY_CMD] = MT626CMDCreate(0x35, 9, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_INC_CMD]        = MT626CMDCreate(0x37, 8, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_DEC_CMD]        = MT626CMDCreate(0x38, 8, makeStCmd,analyStRes);
    pMT626->pMT626CMD[MT626_INIT_CMD]       = MT626CMDCreate(0x36, 8, makeStCmd,analyStRes);

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
