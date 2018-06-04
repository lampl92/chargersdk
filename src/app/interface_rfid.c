/**
* @file interface_card.c
* @brief RFID射频读卡器接口
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include "includes.h"
#include "bsp.h"
#include "interface.h"
#include "user_app.h"

static ErrorCode_t RFIDGetUID(void *pvRfid)
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
        Buzzer_control(1);
        //vTaskDelay(200);
        bsp_DelayMS(200);
        Buzzer_control(0);
        ulRecvdOptLen = pmt626cmd->ulRecvdOptLen;
        memset(pRfid->status.ucCardID, 0, defCardIDLength);
        memmove(pRfid->status.ucCardID, pmt626cmd->ucRecvdOptData, ulRecvdOptLen);
        memset(pmt626cmd->ucRecvdOptData, 0, ulRecvdOptLen);
        xEventGroupSetBits(pRfid->xHandleEventGroupRFID, defEventBitGotIDtoRFID);
        pEVSE->status.ulSignalFault &= ~defSignalEVSE_Fault_RFID;
    }
    else if(state == MT_STATE_N)
    {
        pEVSE->status.ulSignalFault &= ~defSignalEVSE_Fault_RFID;
        //xEventGroupClearBits(pRfid->xHandleEventGroupRFID, defEventBitGotIDtoRFID);//清除在其他流程中误刷卡
    }
    else if(state == MT_COM_FAIL)
    {
        pEVSE->status.ulSignalFault |= defSignalEVSE_Fault_RFID;
        errcode = ERR_RFID_FAULT;
    }

    xSemaphoreGive(pRfid->xHandleMutexRFID);

    return errcode;
}

RFIDDev_t *RFIDDevCreate(char *uart_name, uint32_t band, int data_bit, char parity, int stop_bit)
{
    RFIDDev_t *pRFID;

    pRFID = (RFIDDev_t *)malloc(sizeof(RFIDDev_t));
    memset(pRFID, 0, sizeof(RFIDDev_t));
    
	pRFID->status.ulHoldMaxTime_s = 60;
    pRFID->com = (void *)MT626COMCreate();
    pRFID->status.GetCardID = RFIDGetUID;
    pRFID->xHandleMutexRFID = xSemaphoreCreateMutex();
    pRFID->xHandleEventGroupRFID = xEventGroupCreate();
    pRFID->state = STATE_RFID_NOID;
    pRFID->uart_handle = uart_open(uart_name, band, data_bit, parity, stop_bit);

    OrderInit(&(pRFID->order));

    return pRFID;
}
