/**
* @file interface_card.c
* @brief RFIDÉäÆµ¶Á¿¨Æ÷½Ó¿Ú
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#include <string.h>
#include "interface_rfid.h"

void Init_RFIDDev(void *pRFIDDev)
{
    pRFIDDev = (void *)MT626COMCreate();
}
void *CreateRFIDDev(void)
{
    MT626COM_t *pmt626com;
    pmt626com = MT626COMCreate();

    return (void *)pmt626com;
}
int GetUID(void *pRFIDDev, uint8_t *pUID)
{
    MT626COM_t *pmt626com;
    uint8_t *precvdData;
    uint32_t uiRecvdOptLen;
    int state;

    pmt626com = (MT626COM_t *)pRFIDDev;
    precvdData = pUID;
    uiRecvdOptLen = 0;
    state = MT_STATE_N;

    state = TransToMT626(pmt626com, MT626_READ_UID_CMD, NULL, 0);
    if(state == MT_STATE_Y)
    {
        uiRecvdOptLen = pmt626com->pMT626CMD[MT626_READ_UID_CMD]->uiRecvdOptLen;
        memmove(precvdData, pmt626com->pMT626CMD[MT626_READ_UID_CMD]->ucRecvdOptData, uiRecvdOptLen);
        memset(pmt626com->pMT626CMD[MT626_READ_UID_CMD]->ucRecvdOptData,0,uiRecvdOptLen);
    }
    return state;
}
