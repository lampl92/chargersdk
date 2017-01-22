/**
* @file taskcard.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
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

        for(i = 0; i < uiRecvdOptLen; i++)
        {
            precvdData[i] = pmt626com->pMT626CMD[MT626_READ_UID_CMD]->ucRecvdOptData[i];
        }
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
            printf("UID = ");
            for(i = 0; i < 4; i++)
            {
                printf("%x", cardUID[i]);
            }
            printf("\n");
        }
        else
        {
            printf("No card.\n");
        }

        //printf("%s\n", TASKNAME_EVSECard);
        vTaskDelay(500);
    }
}
