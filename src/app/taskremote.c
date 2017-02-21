/**
* @file taskremote.c
* @brief Զ��ͨ�ŵĲ���������ͨ��ʵ���ָ��
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "includes.h"
#include "taskcreate.h"
#include "taskremote.h"

void vTaskEVSERemote(void *pvParameters)
{
    EventBits_t uxBits = 0;
    while(1)
    {
        uxBits = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGETUID, pdFALSE, pdTRUE, 0);
        if((uxBits & defEventBitGETUID) != 0)
        {
#if DEBUG_REMOTE
            printf_safe("im remote,find card, :)\n");
#endif
        }
#if DEBUG_REMOTE
        printf_safe("%s\n", TASKNAME_EVSERemote);
#endif
        vTaskDelay(100);
    }
}
