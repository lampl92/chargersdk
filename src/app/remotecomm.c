/**
* @file remotecomm.c
* @brief Զ��ͨ�ŵĲ���������ͨ��ʵ���ָ��
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
#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSERemoteComm);
#endif
        vTaskDelay(1000);
    }
}
