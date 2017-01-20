/**
* @file taskcreate.c
* @brief
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "RFIDReader_mt626.h"

//创建任务Todolist 对照表
//1. 定义STACK大小
//2. 定义PRIORITY
//3. 声明任务
//4. 定义任务句柄
//5. 任务入口
//6. 创建任务

//系统
#define defSTACK_TaskCLI                    512
#define defSTACK_TaskGUI                    1024
#define defSTACK_TaskTouch                  128
#define defSTACK_TaskOTA                    512
//APP
#define defSTACK_TaskEVSERemoteComm         512
#define defSTACK_TaskEVSECard               512
#define defSTACK_TaskEVSECharge             512
#define defSTACK_TaskEVSEMonitor            512
#define defSTACK_TaskEVSEError              512
#define defSTACK_TaskEVSEData               512


//优先级规则为系统任务优先级低，OTA > 充电任务 > 故障处理 > 系统监视 > 刷卡与通信 > 数据处理与系统任务
//系统
#define defPRIORITY_TaskCLI                 1
#define defPRIORITY_TaskGUI                 1
#define defPRIORITY_TaskTouch               1
#define defPRIORITY_TaskOTA                 15 //最高
//APP
#define defPRIORITY_TaskEVSERemoteComm      3
#define defPRIORITY_TaskEVSECard            3
#define defPRIORITY_TaskEVSECharge          5
#define defPRIORITY_TaskEVSEMonitor         7
#define defPRIORITY_TaskEVSEError           9
#define defPRIORITY_TaskEVSEData            1


//系统任务
static void vTaskCLI(void *pvParameters);
static void vTaskGUI(void *pvParameters);
static void vTaskTouch(void *pvParameters);
static void vTaskOTA(void *pvParameters);               //在线升级
//APP任务
static void vTaskEVSERemoteComm(void *pvParameters);    //远程通信
static void vTaskEVSECard(void *pvParameters);          //刷卡
static void vTaskEVSECharge(void *pvParameters);        //充电
static void vTaskEVSEMonitor(void *pvParameters);       //监控
static void vTaskEVSEError(void *pvParameters);   //错误处理
static void vTaskEVSEData(void *pvParameters);    //数据处理
static void vTaskOTA(void *pvParameters);               //在线升级

//系统任务句柄
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;
//APP任务句柄
static TaskHandle_t xHandleTaskEVSERemoteComm = NULL;
static TaskHandle_t xHandleTaskEVSECard = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEError = NULL;
static TaskHandle_t xHandleTaskEVSEData = NULL;

static void vTaskCLI(void *pvParameters)
{
    cli_main();
}

static void vTaskGUI(void *pvParameters)
{
    Touch_Calibrate();
}

static void vTaskTouch(void *pvParameters)
{
    while(1)
    {
        GUI_TOUCH_Exec();
        vTaskDelay(10);
    }
}
static void vTaskOTA(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}

static void vTaskStart(void *pvParameters)
{

    //testobserver();
    testmt626();
    APPTaskCreate();
}
static void vTaskEVSERemoteComm(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}
static void vTaskEVSECard(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}
static void vTaskEVSECharge(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}
static void vTaskEVSEMonitor(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}
static void vTaskEVSEError(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}
static void vTaskEVSEData(void *pvParameters)
{
    TaskHandle_t xTaskHandle = xTaskGetCurrentTaskHandle();
    while(1)
    {
        printf("%s\n", pcTaskGetName( xTaskHandle ));
        vTaskDelay(1000);
    }
}


void SysTaskCreate (void)
{
    xTaskCreate( vTaskCLI, "vTaskCLI", defSTACK_TaskCLI, NULL, defPRIORITY_TaskCLI, &xHandleTaskCLI );
    xTaskCreate( vTaskGUI, "vTaskGUI", defSTACK_TaskGUI, NULL, defPRIORITY_TaskGUI, &xHandleTaskGUI );
    xTaskCreate( vTaskTouch, "vTaskTouch", defSTACK_TaskTouch, NULL, defPRIORITY_TaskTouch, &xHandleTaskTouch );
    xTaskCreate( vTaskOTA, "vTaskOTA", defSTACK_TaskOTA, NULL, defPRIORITY_TaskOTA, &xHandleTaskOTA );
}

void AppTaskCreate (void)
{
    xTaskCreate( vTaskEVSERemoteComm, "vTaskEVSERemoteComm", defSTACK_TaskEVSERemoteComm, NULL, defPRIORITY_TaskEVSERemoteComm, &xHandleTaskEVSERemoteComm );
    xTaskCreate( vTaskEVSECard, "vTaskEVSECard", defSTACK_TaskEVSECard, NULL, defPRIORITY_TaskEVSECard, &xHandleTaskEVSECard );
    xTaskCreate( vTaskEVSECharge, "vTaskEVSECharge", defSTACK_TaskEVSECharge, NULL, defPRIORITY_TaskEVSECharge, &xHandleTaskEVSECharge );
    xTaskCreate( vTaskEVSEMonitor, "vTaskEVSEMonitor", defSTACK_TaskEVSEMonitor, NULL, defPRIORITY_TaskEVSEMonitor, &xHandleTaskEVSEMonitor );
    xTaskCreate( vTaskEVSEError, "vTaskEVSEError", defSTACK_TaskEVSEError, NULL, defPRIORITY_TaskEVSEError, &xHandleTaskEVSEError );
    xTaskCreate( vTaskEVSEData, "vTaskEVSEData", defSTACK_TaskEVSEData, NULL, defPRIORITY_TaskEVSEData, &xHandleTaskEVSEData );
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

