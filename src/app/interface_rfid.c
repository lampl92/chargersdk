/**
* @file interface_card.c
* @brief RFID��Ƶ�������ӿ�
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "interface.h"

static ErrorCode_t MT626GetUID(void *pvRfid)
{
    RFIDDev_t *pRfid;
    MT626CMD_t *pmt626cmd;
    uint32_t ulRecvdOptLen;
    int state;
    ErrorCode_t errcode;

    pRfid = (RFIDDev_t *)pvRfid;
    pmt626cmd = ((MT626COM_t *)(pRfid->com))->pMT626CMD[MT626_READ_UID_CMD];
    state = MT_STATE_N;
    errcode = ERR_NO;

    xSemaphoreTake(pRfid->xHandleMutexRFID, portMAX_DELAY);

    state = TransToMT626(pRfid->com, MT626_READ_UID_CMD, NULL, 0);
    if(state == MT_STATE_Y)
    {
        ulRecvdOptLen = pmt626cmd->uiRecvdOptLen;
        memmove(pRfid->status.ucUID, pmt626cmd->ucRecvdOptData, ulRecvdOptLen);
        memset(pmt626cmd->ucRecvdOptData, 0, ulRecvdOptLen);
        //pRfid->status.ucFoundCard = 1;
        xEventGroupSetBits(pRfid->xHandleEventGroupRFID, defEventBitGETUID);
    }
    else if(state == MT_STATE_N)
    {
        //pRfid->status.ucFoundCard = 0;
    }
    else if(state == MT_COM_FAIL)
    {
        errcode = ERR_RFID_FAULT;
    }

    xSemaphoreGive(pRfid->xHandleMutexRFID);

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
    pRFID->xHandleMutexRFID = xSemaphoreCreateMutex();
    pRFID->xHandleEventGroupRFID = xEventGroupCreate();

    return pRFID;
}
