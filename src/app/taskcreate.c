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
//系统任务
static void vTaskCLI(void *pvParameters);
static void vTaskGUI(void *pvParameters);
static void vTaskTouch(void *pvParameters);
//APP任务
static void vTaskStart(void *pvParameters);
static void vTaskEVSERemoteComm(void *pvParameters);    //远程通信
static void vTaskEVSECard(void *pvParameters);          //刷卡
static void vTaskEVSECharge(void *pvParameters);        //充电
static void vTaskEVSEMonitor(void *pvParameters);       //监控
static void vTaskEVSEErrorHandle(void *pvParameters);   //错误处理
static void vTaskEVSEDataHandle(void *pvParameters);    //数据处理

//系统任务句柄
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
//APP任务句柄
static TaskHandle_t xHandleTaskEVSERemoteComm = NULL;
static TaskHandle_t xHandleTaskEVSECard = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEErrorHandler = NULL;
static TaskHandle_t xHandleTaskEVSEDataHandler = NULL;

/** @brief CLI任务,优先级1
 *
 * @param pvParameters void*
 * @return void
 *
 */
static void vTaskCLI(void *pvParameters)
{
    cli_main();
}

/** @brief GUI任务,优先级2
 *
 * @param pvParameters void*
 * @return void
 *
 */
static void vTaskGUI(void *pvParameters)
{
    Touch_Calibrate();
}

/** @brief 触摸屏任务,优先级3
 *
 * @param pvParameters void* 创建该任务时传递的形参
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

/** @brief 优先级4
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


    xTaskCreate( vTaskAppStart,            /* 任务函数  */
                 "vTaskAppStart",          /* 任务名    */
                 512,                   /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 4,                     /* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}

void AppTaskCreate (void)
{
    xTaskCreate( vTaskCLI,              /* 任务函数  */
                 "vTaskCLI",            /* 任务名    */
                 1024,                  /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 1,                     /* 任务优先级*/
                 &xHandleTaskCLI );     /* 任务句柄  */


    xTaskCreate( vTaskGUI,              /* 任务函数  */
                 "vTaskGUI",            /* 任务名    */
                 1024,                  /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 2,                     /* 任务优先级*/
                 &xHandleTaskGUI );     /* 任务句柄  */

    xTaskCreate( vTaskTouch,            /* 任务函数  */
                 "vTaskTouch",          /* 任务名    */
                 512,                   /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 3,                     /* 任务优先级*/
                 &xHandleTaskTouch );   /* 任务句柄  */


    xTaskCreate( vTaskAppStart,            /* 任务函数  */
                 "vTaskAppStart",          /* 任务名    */
                 512,                   /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 4,                     /* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //被系统调用

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

