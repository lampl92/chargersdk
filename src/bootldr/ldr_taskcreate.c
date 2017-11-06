/**
* @file taskcreate.c
* @brief 创建任务Todolist 对照表
*        1. 定义STACK大小
*        2. 定义PRIORITY
*        3. 声明任务
*        4. 定义任务句柄
*        5. 任务入口
*        6. 创建任务创建任务Todolist 对照表
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "interface.h"
#include "cli_main.h"
#include "timercallback.h"
#include "gprs_m26.h"
#include "bsp.h"
#include "modem.h"


/*---------------------------------------------------------------------------/
/ 任务栈大小
/---------------------------------------------------------------------------*/
#define defSTACK_TaskInit                   (1024*10)
#define defSTACK_TaskCLI                    (1024 * 50)
#define defSTACK_TaskGUI                    (1024*10)
#define defSTACK_TaskTouch                  512
#define defSTACK_TaskOTA                    512
#define defSTACK_TaskPPP                    (1024*10)
#define defSTACK_TaskTCPClient               (1024*10)
#define defSTACK_TaskRemoteCmdProc          (1024*10)

#define defSTACK_TaskEVSERemote             (1024*10)
#define defSTACK_TaskEVSERFID               512
#define defSTACK_TaskEVSECharge             512
#define defSTACK_TaskEVSEMonitor            512
#define defSTACK_TaskEVSEDiag               512
#define defSTACK_TaskEVSEData               (1024*50)


//#define TCPIP_THREAD_STACKSIZE      512

/*---------------------------------------------------------------------------/
/ 任务优先级
/---------------------------------------------------------------------------*/
//优先级规则为系统任务优先级低，OTA > 充电任务 > 故障处理 > 系统监视 > 刷卡与通信 > 数据处理与系统任务
#define defPRIORITY_TaskOTA                 31/* 最高*/

#define defPRIORITY_TaskEVSECharge          27
#define defPRIORITY_TaskEVSEDiag            25
#define defPRIORITY_TaskEVSEMonitor         23
//#define configTIMER_TASK_PRIORITY     ( defined in FreeRTOSConfig.h ) 22
#define defPRIORITY_TaskEVSERFID            20
#define defPRIORITY_TaskEVSERemote          18
#define defPRIORITY_TaskEVSEData            16

#define defPRIORITY_TaskPPP                 14
#define defPRIORITY_TaskTCPClient           12
#define defPRIORITY_TaskRemoteCmdProc       19

#define defPRIORITY_TaskInit                10
#define defPRIORITY_TaskTouch               6
#define defPRIORITY_TaskGUI                 4   //不能高,GUI任务时间太长,会影响硬件响应
#define defPRIORITY_TaskCLI                 2

//#define TCPIP_THREAD_PRIO         13 //defined in lwipopts.h
// define PPP_THREAD_PRIO          14

/*---------------------------------------------------------------------------/
/ 任务名称
/---------------------------------------------------------------------------*/
const char *TASKNAME_INIT           = "TaskInit";
const char *TASKNAME_CLI            = "TaskCLI";
const char *TASKNAME_GUI            = "TaskGUI";
const char *TASKNAME_Touch          = "TaskTouch";
const char *TASKNAME_OTA            = "TaskOTA";
const char *TASKNAME_PPP            = "TaskPPP";
const char *TASKNAME_TCP_CLIENT     = "TaskTCPClient";
const char *TASKNAME_RemoteCmdProc  = "TaskRemoteCmdProc" ;

const char *TASKNAME_EVSERemote     = "TaskEVSERemote";
const char *TASKNAME_EVSERFID       = "TaskEVSERFID";
const char *TASKNAME_EVSECharge     = "TaskEVSECharge";
const char *TASKNAME_EVSEMonitor    = "TaskEVSEMonitor";
const char *TASKNAME_EVSEDiag       = "TaskEVSEDiag";
const char *TASKNAME_EVSEData       = "TaskEVSEData";
//#define TCPIP_THREAD_NAME           "tcpip_thread"
/*---------------------------------------------------------------------------/
/ 任务声明
/---------------------------------------------------------------------------*/
void vTaskInit(void *pvParameters);
void vTaskCLI(void *pvParameters);
void vTaskGUI(void *pvParameters);
void vTaskTouch(void *pvParameters);
void vTaskOTA(void *pvParameters);
void vTaskPPP(void *pvParameters);
void vTaskTCPClient(void *pvParameters);
void vTaskRemoteCmdProc(void *pvParameters);

void vTaskEVSERemote(void *pvParameters);
void vTaskEVSERFID(void *pvParameters);
void vTaskEVSECharge(void *pvParameters);
void vTaskEVSEMonitor(void *pvParameters);
void vTaskEVSEDiag(void *pvParameters);
void vTaskEVSEData(void *pvParameters);

/*---------------------------------------------------------------------------/
/ 任务句柄
/---------------------------------------------------------------------------*/
static TaskHandle_t xHandleTaskInit = NULL;
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;
static TaskHandle_t xHandleTaskPPP = NULL;
static TaskHandle_t xHandleTaskTCPClient = NULL;
static TaskHandle_t xHandleTaskRemoteCmdProc = NULL;

static TaskHandle_t xHandleTaskEVSERemote = NULL;
static TaskHandle_t xHandleTaskEVSERFID = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEDiag = NULL;
static TaskHandle_t xHandleTaskEVSEData = NULL;
/*---------------------------------------------------------------------------/
/ 任务间通信
/---------------------------------------------------------------------------*/
SemaphoreHandle_t xMutexTimeStruct;
SemaphoreHandle_t xMutexNandHW;
SemaphoreHandle_t  xprintfMutex = NULL;


EventGroupHandle_t xHandleEventTimerCBNotify = NULL;
EventGroupHandle_t xHandleEventData = NULL;
EventGroupHandle_t xHandleEventDiag = NULL;
EventGroupHandle_t xHandleEventRemote = NULL;
EventGroupHandle_t xHandleEventHMI  = NULL;
EventGroupHandle_t xHandleEventTCP   = NULL;

//队列

QueueHandle_t xHandleQueueErrorPackage = NULL;

//con中还定义了几个定时器，xHandleTimerVolt，xHandleTimerCurr，xHandleTimerCharge分别在使用时进行初始化
//Mutex
extern void fs_init(void);
extern void *_app_start[];
void vTaskInit(void *pvParameters)
{
    AppObjCreate();
    sys_Init();
    SysTaskCreate();
    while (1)
    {

        //((void(*)())_app_start[1])();
        vTaskDelay(1000);
    }
}
void vTaskCLI(void *pvParameters)
{
    //cli_main();
}


void TaskInit(void)
{
    xTaskCreate( vTaskInit, TASKNAME_INIT, defSTACK_TaskInit, NULL, defPRIORITY_TaskInit, &xHandleTaskInit );
}

void SysTaskCreate (void)
{
    //xTaskCreate( vTaskCLI, TASKNAME_CLI, defSTACK_TaskCLI, NULL, defPRIORITY_TaskCLI, &xHandleTaskCLI );
    //xTaskCreate( vTaskOTA, TASKNAME_OTA, defSTACK_TaskOTA, NULL, defPRIORITY_TaskOTA, &xHandleTaskOTA );
}

/** @brief
 */
void AppObjCreate (void)
{
    xMutexTimeStruct = xSemaphoreCreateMutex();
    xMutexNandHW = xSemaphoreCreateMutex();
    xprintfMutex = xSemaphoreCreateMutex();
    

    xHandleQueueErrorPackage = xQueueCreate(100, sizeof(ErrorPackage_t));

    //TimerHeartbeat远程服务器连接后开启定时器
}
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //被系统调用
extern __IO uint32_t uwTick;
void vApplicationTickHook( void )
{
    ulHighFrequencyTimerTicks = xTaskGetTickCount();
    uwTick = ulHighFrequencyTimerTicks;
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
