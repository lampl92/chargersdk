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
#include "interface.h"

void vTaskEVSERFID(void *pvParameters)
{
    int i;
    EventBits_t uxBits;
    ErrorCode_t errcode;
    ChargePoint_t *pPoint[2];
    uint8_t ucTotal;

    ucTotal = pEVSE->info.ucTotalPoint;
    for(i = 0; i < ucTotal; i++)
    {
        pPoint[i] =  ChargePointGetHandle(i);
    }
    uxBits = 0;
    errcode = ERR_NO;
    while(1)
    {
        uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGETUID, pdTRUE, pdTRUE, portMAX_DELAY);
        if((uxBits & defEventBitGETUID) == defEventBitGETUID)
        {
#ifdef DEBUG_RFID
            printf_safe("im rfid task,find card, :)\n");
            printf_safe("UID = ");
            for(i = 0; i < 4; i++)
            {
                printf_safe("%x", pRFIDDev->status.ucUID[i]);
            }
            printf_safe("\n");
#endif
            for(i = 0; i < ucTotal; i++)
            {
                if(memcmp(pPoint[i]->status.ucHeldCardUID, pRFIDDev->status.ucUID, defUIDLength) == 0)
                {
                    //´Ë¿¨ÒÑË¢
                }
                else
                {
                    //Î´Ë¢¹ý¿¨

                }
            }
        }


        //xprintf("%s\n", TASKNAME_EVSECard);
        //vTaskDelay(500);
    }
}
