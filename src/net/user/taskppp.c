/**
* @file taskppp.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#include "modem.h"
#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t xHandleTaskPPP = NULL;

void vTaskPPP(void *pvParameters)
{
    pModem = DevModemCreate();
    Modem_Poll(pModem);//这是任务
}
