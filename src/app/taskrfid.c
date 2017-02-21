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

    uint8_t cardUID[4];
    int i;
    RFIDDev_t *pRFIDDev;

    EventBits_t uxBits;
    ErrorCode_t errcode;

//    uint32_t ulTotalPoint = pListChargePoint->Total;
//    ChargePoint_t *pPoint[ulTotalPoint];
//    uint32_t uxTimerID;
//    int i;
//    for(i = 0; i < ulTotalPoint; i++)
//    {
//        pPoint[i] =  (ChargePoint_t *)(pListChargePoint->pListPointArray[i]);
//    }

    pRFIDDev = RFIDDevCreate();
    errcode = ERR_NO;

    while(1)
    {
        THROW_ERROR(errcode = pRFIDDev->status.GetUID(pRFIDDev), ERR_LEVEL_CRITICAL);
        if(pRFIDDev->status.ucFoundCard == 1)
        {
            #ifdef DEBUG_RFID
            printf_safe("UID = ");
            for(i = 0; i < 4; i++)
            {
                printf_safe("%x", pRFIDDev->status.ucUID[i]);
            }
            printf_safe("\n");
            #endif
            uxBits = xEventGroupSetBits(xHandleEventGroupRFID, defEventBitGETUID);
        }
        else
        {
            #ifdef DEBUG_RFID
            printf_safe("No card.\n");
            #endif
        }

        //xprintf("%s\n", TASKNAME_EVSECard);
        vTaskDelay(500);
    }
}
