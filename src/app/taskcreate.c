/**
* @file taskcreate.c
* @brief
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "RFIDReader_mt626.h"

#define defSTACK_TaskCLI                        512
#define defSTACK_TaskGUI                        1024
#define defSTACK_TaskTouch                      128

#define defSTACK_TaskEVSERemoteComm            512
#define defSTACK_TaskEVSECard                  512
#define defSTACK_TaskEVSECharge                512
#define defSTACK_TaskEVSEMonitor               512
#define defSTACK_TaskEVSEErrorHandle           512
#define defSTACK_TaskEVSEDataHandle            512

#define defPRIORITY_TaskCLI                     1
#define defPRIORITY_TaskGUI                     1
#define defPRIORITY_TaskTouch                   1

#define defPRIORITY_TaskEVSERemoteComm           1
#define defPRIORITY_TaskEVSECard                 1
#define defPRIORITY_TaskEVSECharge               1
#define defPRIORITY_TaskEVSEMonitor              1
#define defPRIORITY_TaskEVSEErrorHandle          1
#define defPRIORITY_TaskEVSEDataHandle           1
//ϵͳ����
static void vTaskCLI(void *pvParameters);
static void vTaskGUI(void *pvParameters);
static void vTaskTouch(void *pvParameters);
//APP����
static void vTaskStart(void *pvParameters);
static void vTaskEVSERemoteComm(void *pvParameters);    //Զ��ͨ��
static void vTaskEVSECard(void *pvParameters);          //ˢ��
static void vTaskEVSECharge(void *pvParameters);        //���
static void vTaskEVSEMonitor(void *pvParameters);       //���
static void vTaskEVSEErrorHandle(void *pvParameters);   //������
static void vTaskEVSEDataHandle(void *pvParameters);    //���ݴ���

//ϵͳ������
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
//APP������
static TaskHandle_t xHandleTaskEVSERemoteComm = NULL;
static TaskHandle_t xHandleTaskEVSECard = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEErrorHandler = NULL;
static TaskHandle_t xHandleTaskEVSEDataHandler = NULL;

/** @brief CLI����,���ȼ�1
 *
 * @param pvParameters void*
 * @return void
 *
 */
static void vTaskCLI(void *pvParameters)
{
    cli_main();
}

/** @brief GUI����,���ȼ�2
 *
 * @param pvParameters void*
 * @return void
 *
 */
static void vTaskGUI(void *pvParameters)
{
    Touch_Calibrate();
}

/** @brief ����������,���ȼ�3
 *
 * @param pvParameters void* ����������ʱ���ݵ��β�
 * @return void
 *
 */
static void vTaskTouch(void *pvParameters)
{
    while(1)
    {
        GUI_TOUCH_Exec();
        vTaskDelay(10);
    }
}

/** @brief ���ȼ�4
 *
 * @param pvParameters void*
 * @return void
 *
 */
static void vTaskAppStart(void *pvParameters)
{

    //testobserver();
    testmt626();
    APPTaskCreate();
}

void SysTaskCreate (void)
{
    xTaskCreate( vTaskCLI, "vTaskCLI", defSTACK_TASKCLI, NULL, defPRIORITY_TASKCLI, &xHandleTaskCLI );
    xTaskCreate( vTaskGUI, "vTaskGUI", defSTACK_TASKGUI, NULL, defPRIORITY_TASKGUI, &xHandleTaskGUI );
    xTaskCreate( vTaskTouch, "vTaskTouch", defSTACK_TASKTOUCH, NULL, defPRIORITY_TASKTOUCH, &xHandleTaskTouch );


    xTaskCreate( vTaskAppStart,            /* ������  */
                 "vTaskAppStart",          /* ������    */
                 512,                   /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 4,                     /* �������ȼ�*/
                 &xHandleTaskStart );   /* ������  */
}

void AppTaskCreate (void)
{
    xTaskCreate( vTaskCLI,              /* ������  */
                 "vTaskCLI",            /* ������    */
                 1024,                  /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 1,                     /* �������ȼ�*/
                 &xHandleTaskCLI );     /* ������  */


    xTaskCreate( vTaskGUI,              /* ������  */
                 "vTaskGUI",            /* ������    */
                 1024,                  /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 2,                     /* �������ȼ�*/
                 &xHandleTaskGUI );     /* ������  */

    xTaskCreate( vTaskTouch,            /* ������  */
                 "vTaskTouch",          /* ������    */
                 512,                   /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 3,                     /* �������ȼ�*/
                 &xHandleTaskTouch );   /* ������  */


    xTaskCreate( vTaskAppStart,            /* ������  */
                 "vTaskAppStart",          /* ������    */
                 512,                   /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 4,                     /* �������ȼ�*/
                 &xHandleTaskStart );   /* ������  */
}
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //��ϵͳ����

void vApplicationTickHook( void )
{
    ulHighFrequencyTimerTicks = xTaskGetTickCount();
}

/*
---------------------------------------------------------------------------*/
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
/*
---------------------------------------------------------------------------*/
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

/*
---------------------------------------------------------------------------*/
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

/*
---------------------------------------------------------------------------*/
/**
* @brief
*
* @param pxTask
* @param pcTaskName
*/
/*
---------------------------------------------------------------------------*/
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

