/**
* @file taskmonitor.c
* @brief ¼à¿ØÈÎÎñ
* @author rgw
* @version v1.0
* @date 2017-01-19
*/
#include "taskcreate.h"
#include "taskmonitor.h"
#include "interface.h"
void vTaskEVSEMonitor(void *pvParameters)
{
    while(1)
    {


#if DEBUG_TASK
        xprintf("%s\n", TASKNAME_EVSEMonitor);
#endif
        vTaskDelay(50);
    }
}
