/**
* @file taskrfid.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "includes.h"
#include "taskcreate.h"
#include "taskrfid.h"
#include "interface_rfid.h"

void vTaskEVSERFID(void *pvParameters)
{
    uint8_t cardUID[4];
    int state, i;
    void *pRFIDDev;

    pRFIDDev = CreateRFIDDev();

    while(1)
    {
        state = GetUID(pRFIDDev, cardUID);
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
