/**
* @file taskcreate.c
* @brief ��������Todolist ���ձ�
*        1. ����STACK��С
*        2. ����PRIORITY
*        3. ��������
*        4. ����������
*        5. �������
*        6. �������񴴽�����Todolist ���ձ�
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"

/*---------------------------------------------------------------------------/
/ ����ջ����
/---------------------------------------------------------------------------*/
#define defSTACK_TaskCLI                    512
#define defSTACK_TaskGUI                    1024
#define defSTACK_TaskTouch                  128
#define defSTACK_TaskOTA                    512

#define defSTACK_TaskEVSERemoteComm         512
#define defSTACK_TaskEVSECard               512
#define defSTACK_TaskEVSECharge             512
#define defSTACK_TaskEVSEMonitor            512
#define defSTACK_TaskEVSEError              512
#define defSTACK_TaskEVSEData               512

/*---------------------------------------------------------------------------/
/ �������ȼ�
/---------------------------------------------------------------------------*/
//���ȼ�����Ϊϵͳ�������ȼ��ͣ�OTA > ������� > ���ϴ��� > ϵͳ���� > ˢ����ͨ�� > ���ݴ�����ϵͳ����
#define defPRIORITY_TaskCLI                 1
#define defPRIORITY_TaskGUI                 1
#define defPRIORITY_TaskTouch               1
#define defPRIORITY_TaskOTA                 15 /* ���*/

#define defPRIORITY_TaskEVSERemoteComm      3
#define defPRIORITY_TaskEVSECard            3
#define defPRIORITY_TaskEVSECharge          5
#define defPRIORITY_TaskEVSEMonitor         7
#define defPRIORITY_TaskEVSEError           9
#define defPRIORITY_TaskEVSEData            1

/*---------------------------------------------------------------------------/
/ ��������
/---------------------------------------------------------------------------*/
const char *TASKNAME_CLI            = "TaskCLI";
const char *TASKNAME_GUI            = "TaskGUI";
const char *TASKNAME_Touch          = "TaskTouch";
const char *TASKNAME_OTA            = "TaskOTA";
const char *TASKNAME_EVSERemoteComm = "TaskEVSERemoteComm";
const char *TASKNAME_EVSECard       = "TaskEVSECard";
const char *TASKNAME_EVSECharge     = "TaskEVSECharge";
const char *TASKNAME_EVSEMonitor    = "TaskEVSEMonitor";
const char *TASKNAME_EVSEError      = "TaskEVSEError";
const char *TASKNAME_EVSEData       = "TaskEVSEData";

/*---------------------------------------------------------------------------/
/ ��������
/---------------------------------------------------------------------------*/
void vTaskCLI(void *pvParameters);
void vTaskGUI(void *pvParameters);
void vTaskTouch(void *pvParameters);
void vTaskOTA(void *pvParameters);                  //��������

void vTaskEVSERemoteComm(void *pvParameters);       //Զ��ͨ��
void vTaskEVSECard(void *pvParameters);             //ˢ��
void vTaskEVSECharge(void *pvParameters);           //���
void vTaskEVSEMonitor(void *pvParameters);          //���
void vTaskEVSEError(void *pvParameters);            //������
void vTaskEVSEData(void *pvParameters);             //���ݴ���

/*---------------------------------------------------------------------------/
/ ������
/---------------------------------------------------------------------------*/
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;

static TaskHandle_t xHandleTaskEVSERemoteComm = NULL;
static TaskHandle_t xHandleTaskEVSECard = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEError = NULL;
static TaskHandle_t xHandleTaskEVSEData = NULL;

void vTaskCLI(void *pvParameters)
{
    cli_main();
}

void vTaskGUI(void *pvParameters)
{
    Touch_Calibrate();
}

void vTaskTouch(void *pvParameters)
{
    while(1)
    {
        GUI_TOUCH_Exec();
        vTaskDelay(10);
    }
}

void SysTaskCreate (void)
{
    xTaskCreate( vTaskCLI, TASKNAME_CLI, defSTACK_TaskCLI, NULL, defPRIORITY_TaskCLI, &xHandleTaskCLI );
    xTaskCreate( vTaskGUI, TASKNAME_GUI, defSTACK_TaskGUI, NULL, defPRIORITY_TaskGUI, &xHandleTaskGUI );
    xTaskCreate( vTaskTouch, TASKNAME_Touch, defSTACK_TaskTouch, NULL, defPRIORITY_TaskTouch, &xHandleTaskTouch );
    xTaskCreate( vTaskOTA, TASKNAME_OTA, defSTACK_TaskOTA, NULL, defPRIORITY_TaskOTA, &xHandleTaskOTA );
}

void AppTaskCreate (void)
{
    xTaskCreate( vTaskEVSERemoteComm, TASKNAME_EVSERemoteComm, defSTACK_TaskEVSERemoteComm, NULL, defPRIORITY_TaskEVSERemoteComm, &xHandleTaskEVSERemoteComm );
    xTaskCreate( vTaskEVSECard, TASKNAME_EVSECard, defSTACK_TaskEVSECard, NULL, defPRIORITY_TaskEVSECard, &xHandleTaskEVSECard );
    xTaskCreate( vTaskEVSECharge, TASKNAME_EVSECharge, defSTACK_TaskEVSECharge, NULL, defPRIORITY_TaskEVSECharge, &xHandleTaskEVSECharge );
    xTaskCreate( vTaskEVSEMonitor, TASKNAME_EVSEMonitor, defSTACK_TaskEVSEMonitor, NULL, defPRIORITY_TaskEVSEMonitor, &xHandleTaskEVSEMonitor );
    xTaskCreate( vTaskEVSEError, TASKNAME_EVSEError, defSTACK_TaskEVSEError, NULL, defPRIORITY_TaskEVSEError, &xHandleTaskEVSEError );
    xTaskCreate( vTaskEVSEData, TASKNAME_EVSEData, defSTACK_TaskEVSEData, NULL, defPRIORITY_TaskEVSEData, &xHandleTaskEVSEData );
}

volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //��ϵͳ����
void vApplicationTickHook( void )
{
    ulHighFrequencyTimerTicks = xTaskGetTickCount();
}

/**
* @brief vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented).
*/
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

/**
* @brief vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is
set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted.
*/
void vApplicationIdleHook( void )
{
}

/**
* @brief
*
* @param pxTask
* @param pcTaskName
*/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

