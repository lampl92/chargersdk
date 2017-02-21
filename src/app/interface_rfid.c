/**
* @file interface_card.c
* @brief RFIDÉäÆµ¶Á¿¨Æ÷½Ó¿Ú
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "interface.h"

static ErrorCode_t MT626GetUID(void *pvmt626)
{
    RFIDDev_t *pmt626;
    MT626CMD_t *pmt626cmd;
    uint32_t ulRecvdOptLen;
    int state;
    ErrorCode_t errcode;

    pmt626 = (RFIDDev_t *)pvmt626;
    pmt626cmd = ((MT626COM_t *)(pmt626->com))->pMT626CMD[MT626_READ_UID_CMD];
    state = MT_STATE_N;
    errcode = ERR_NO;

    state = TransToMT626(pmt626->com, MT626_READ_UID_CMD, NULL, 0);
    if(state == MT_STATE_Y)
    {
        ulRecvdOptLen = pmt626cmd->uiRecvdOptLen;
        memmove(pmt626->status.ucUID, pmt626cmd->ucRecvdOptData, ulRecvdOptLen);
        memset(pmt626cmd->ucRecvdOptData, 0, ulRecvdOptLen);
        pmt626->status.ucFoundCard = 1;
    }
    else if(state == MT_COM_FAIL)
    {
        errcode = ERR_RFID_FAULT;
    }
    return errcode;
}

RFIDDev_t *RFIDDevCreate(void)
{
    RFIDDev_t *pRFID;

    pRFID = (RFIDDev_t *)malloc(sizeof(RFIDDev_t));
    pRFID->status.ucFoundCard = 0;
    memset(pRFID->status.ucUID, 0 , 4);
    pRFID->com = (void *)MT626COMCreate();
    pRFID->status.GetUID = MT626GetUID;

    return pRFID;
}
