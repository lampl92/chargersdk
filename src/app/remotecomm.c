/**
* @file remotecomm.c
* @brief 远程通信的操作，接收通信实体的指针
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "taskcreate.h"
#include "remotecomm.h"

void vTaskEVSERemoteComm(void *pvParameters)
{
    while(1)
    {
        //xprintf("%s\n", TASKNAME_EVSERemoteComm);
        vTaskDelay(1000);
    }
}
