/**
* @file taskcard.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "includes.h"
#include "taskcreate.h"
#include "taskcard.h"
#include "RFIDReader_mt626.h"

int *GetUID(MT626COM_t *pmt626com, uint8_t *pUID)
{
    uint8_t *precvdData;
    uint32_t uiRecvdOptLen;
    int state, i;

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

void vTaskEVSECard(void *pvParameters)
{
    MT626COM_t *pmt626com;
    uint8_t cardUID[4];
    int state, i;

    pmt626com = MT626COMCreate();

    while(1)
    {
        state = GetUID(pmt626com, cardUID);
        if(state == MT_STATE_Y)
        {
            #ifdef DEBUG_RFID
            xprintf("UID = ");
            for(i = 0; i < 4; i++)
            {
                xprintf("%x", cardUID[i]);
            }
            xprintf("\n");
            #endif
        }
        else
        {
            #ifdef DEBUG_RFID
            xprintf("No card.\n");
            #endif
        }

        //xprintf("%s\n", TASKNAME_EVSECard);
        vTaskDelay(500);
    }
}
