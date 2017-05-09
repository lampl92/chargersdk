/**
* @file taskcreate.c
* @brief 鍒涘缓浠诲姟Todolist 瀵圭収琛?
*        1. 瀹氫箟STACK澶у皬
*        2. 瀹氫箟PRIORITY
*        3. 澹版槑浠诲姟
*        4. 瀹氫箟浠诲姟鍙ユ焺
*        5. 浠诲姟鍏ュ彛
*        6. 鍒涘缓浠诲姟
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "interface.h"
#include "cli_main.h"
#include "timercallback.h"
/*---------------------------------------------------------------------------/
/ 浠诲姟鏍堝畾涔?
/---------------------------------------------------------------------------*/
#define defSTACK_TaskInit                   512
#define defSTACK_TaskCLI                    1024
#define defSTACK_TaskGUI                    (1024*4)
#define defSTACK_TaskTouch                  128
#define defSTACK_TaskOTA                    512

#define defSTACK_TaskEVSERemote             512
#define defSTACK_TaskEVSERFID               512
#define defSTACK_TaskEVSECharge             512
#define defSTACK_TaskEVSEMonitor            512
#define defSTACK_TaskEVSEDiag               512
#define defSTACK_TaskEVSEData               512

/*---------------------------------------------------------------------------/
/ 浠诲姟浼樺厛绾?
/---------------------------------------------------------------------------*/
//优先级规则为系统任务优先级低，OTA > 充电任务 > 故障处理 > 系统监视 > 刷卡与通信 > 数据处理与系统任务
#define defPRIORITY_TaskInit                1
#define defPRIORITY_TaskCLI                 1
#define defPRIORITY_TaskGUI                 1   //不能高,GUI任务时间太长,会影响硬件响应
#define defPRIORITY_TaskTouch               1
#define defPRIORITY_TaskOTA                 15 /* 鏈?楂?*/

#define defPRIORITY_TaskEVSERemote          3
#define defPRIORITY_TaskEVSERFID            4
#define defPRIORITY_TaskEVSECharge          5
#define defPRIORITY_TaskEVSEMonitor         7
#define defPRIORITY_TaskEVSEDiag            9
#define defPRIORITY_TaskEVSEData            1

/*---------------------------------------------------------------------------/
/ 浠诲姟鍚嶇О
/---------------------------------------------------------------------------*/
const char *TASKNAME_INIT           = "TaskInit";
const char *TASKNAME_CLI            = "TaskCLI";
const char *TASKNAME_GUI            = "TaskGUI";
const char *TASKNAME_Touch          = "TaskTouch";
const char *TASKNAME_OTA            = "TaskOTA";
const char *TASKNAME_EVSERemote     = "TaskEVSERemote";
const char *TASKNAME_EVSERFID       = "TaskEVSERFID";
const char *TASKNAME_EVSECharge     = "TaskEVSECharge";
const char *TASKNAME_EVSEMonitor    = "TaskEVSEMonitor";
const char *TASKNAME_EVSEDiag       = "TaskEVSEDiag";
const char *TASKNAME_EVSEData       = "TaskEVSEData";

/*---------------------------------------------------------------------------/
/ 浠诲姟澹版槑
/---------------------------------------------------------------------------*/
void vTaskInit(void *pvParameters);
void vTaskCLI(void *pvParameters);
void vTaskGUI(void *pvParameters);
void vTaskTouch(void *pvParameters);
void vTaskOTA(void *pvParameters);                  //鍦ㄧ嚎鍗囩骇

void vTaskEVSERemote(void *pvParameters);           //杩滅▼閫氫俊
void vTaskEVSERFID(void *pvParameters);             //鍒峰崱
void vTaskEVSECharge(void *pvParameters);           //鍏呯數
void vTaskEVSEMonitor(void *pvParameters);          //鐩戞帶
void vTaskEVSEDiag(void *pvParameters);             //璇婃柇澶勭悊
void vTaskEVSEData(void *pvParameters);             //鏁版嵁澶勭悊

/*---------------------------------------------------------------------------/
/ 浠诲姟鍙ユ焺
/---------------------------------------------------------------------------*/
static TaskHandle_t xHandleTaskInit = NULL;
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;

static TaskHandle_t xHandleTaskEVSERemote = NULL;
static TaskHandle_t xHandleTaskEVSERFID = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEDiag = NULL;
static TaskHandle_t xHandleTaskEVSEData = NULL;
/*---------------------------------------------------------------------------/
/ 浠诲姟閫氫俊
/---------------------------------------------------------------------------*/
EventGroupHandle_t xHandleEventTimerCBNotify = NULL;
EventGroupHandle_t xHandleEventData = NULL;
EventGroupHandle_t xHandleEventDiag = NULL;
EventGroupHandle_t xHandleEventRemote = NULL;
EventGroupHandle_t xHandleEventHMI  = NULL;

//涓嬮潰鐨勪簨浠跺畾涔夊湪鍚勪釜缁撴瀯浣撲腑
//pRFIDDev->xHandleEventGroupRFID
//pCON->status.xHandleEventCharge;
//pCON->status.xHandleEventException;
//闃熷垪
QueueHandle_t xHandleQueueOrders = NULL;
QueueHandle_t xHandleQueueErrorPackage = NULL;
//杞欢瀹氭椂鍣?
TimerHandle_t xHandleTimerTemp = NULL; //4涓俯搴?
TimerHandle_t xHandleTimerLockState = NULL;
TimerHandle_t xHandleTimerPlugState = NULL;
TimerHandle_t xHandleTimerVolt = NULL;
TimerHandle_t xHandleTimerChargingData = NULL;
TimerHandle_t xHandleTimerEVSEState = NULL;
TimerHandle_t xHandleTimerRFID = NULL;
TimerHandle_t xHandleTimerDataRefresh = NULL;
TimerHandle_t xHandleTimerHeartbeat = NULL;
//con涓繕瀹氫箟浜嗗嚑涓畾鏃跺櫒锛寈HandleTimerVolt锛寈HandleTimerCurr锛寈HandleTimerCharge鍒嗗埆鍦ㄤ娇鐢ㄦ椂杩涜鍒濆鍖?
//Mutex
void vTaskInit(void *pvParameters)
{
    while(1)
    {
        vTaskDelay(1000);
    }
}
void vTaskCLI(void *pvParameters)
{
    cli_main();
}

void vTaskGUI(void *pvParameters)
{
    MainTask();
//    while(1)
//    {
//     vTaskDelay(1000);
//    }
}

void vTaskTouch(void *pvParameters)
{
    while(1)
    {
        GUI_TOUCH_Exec();//激活XY轴的测量
        vTaskDelay(10);
    }
}

void TaskInit(void)
{
    xTaskCreate( vTaskInit, TASKNAME_INIT, defSTACK_TaskInit, NULL, defPRIORITY_TaskInit, &xHandleTaskInit );
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
    xTaskCreate( vTaskEVSERemote, TASKNAME_EVSERemote, defSTACK_TaskEVSERemote, NULL, defPRIORITY_TaskEVSERemote, &xHandleTaskEVSERemote );
    xTaskCreate( vTaskEVSERFID, TASKNAME_EVSERFID, defSTACK_TaskEVSERFID, NULL, defPRIORITY_TaskEVSERFID, &xHandleTaskEVSERFID );
    xTaskCreate( vTaskEVSECharge, TASKNAME_EVSECharge, defSTACK_TaskEVSECharge, NULL, defPRIORITY_TaskEVSECharge, &xHandleTaskEVSECharge );
    xTaskCreate( vTaskEVSEMonitor, TASKNAME_EVSEMonitor, defSTACK_TaskEVSEMonitor, NULL, defPRIORITY_TaskEVSEMonitor, &xHandleTaskEVSEMonitor );
    xTaskCreate( vTaskEVSEDiag, TASKNAME_EVSEDiag, defSTACK_TaskEVSEDiag, NULL, defPRIORITY_TaskEVSEDiag, &xHandleTaskEVSEDiag );
    xTaskCreate( vTaskEVSEData, TASKNAME_EVSEData, defSTACK_TaskEVSEData, NULL, defPRIORITY_TaskEVSEData, &xHandleTaskEVSEData );
}

/** @brief 鍒涘缓浠诲姟閫氫俊鏈哄埗銆傦紙淇″彿閲忥紝杞欢瀹氭椂鍣ㄥ垱寤轰笌鍚姩锛?
 */
void AppObjCreate (void)
{
    xHandleEventTimerCBNotify = xEventGroupCreate();
    xHandleEventData = xEventGroupCreate();
    xHandleEventDiag = xEventGroupCreate();
    xHandleEventRemote = xEventGroupCreate();
    xHandleEventHMI = xEventGroupCreate();


    xHandleQueueOrders = xQueueCreate(2, sizeof(OrderData_t));
    xHandleQueueErrorPackage = xQueueCreate(100, sizeof(ErrorPackage_t));

    xHandleTimerTemp = xTimerCreate("TimerTemp", defMonitorTempCyc, pdTRUE, (void *)defTIMERID_Temp, vCONTimerCB);
    xHandleTimerLockState = xTimerCreate("TimerLockState", defMonitorLockStateCyc, pdTRUE, (void *)defTIMERID_LockState, vCONTimerCB);
    xHandleTimerPlugState = xTimerCreate("TimerPlugState", defMonitorPlugStateCyc, pdTRUE, (void *)defTIMERID_PlugState, vCONTimerCB);
    xHandleTimerVolt = xTimerCreate("TimerVolt", defMonitorChargingDataCyc, pdTRUE, (void *)defTIMERID_Volt, vCONTimerCB);
    xHandleTimerChargingData = xTimerCreate("TimerChargingData", defMonitorChargingDataCyc, pdTRUE, (void *)defTIMERID_ChargingData, vCONTimerCB);
    xHandleTimerEVSEState = xTimerCreate("TimerEVSEState", defMonitorEVSEStateCyc, pdTRUE, (void *)defTIMERID_EVSEState, vEVSETimerCB);
    xHandleTimerRFID = xTimerCreate("TimerRFID", defMonitorRFIDCyc, pdTRUE, (void *)defTIMERID_RFID, vRFIDTimerCB);
    xHandleTimerDataRefresh = xTimerCreate("TimerDataRefresh", defMonitorDataRefreshCyc, pdTRUE, (void *)defTIMERID_DATAREFRESH, vEVSETimerCB);
    xHandleTimerHeartbeat = xTimerCreate("TimerHeartbeat", defRemoteHeartbeatCyc, pdTRUE, (void *)defTIMERID_Heartbeat, vEVSETimerCB);

    xTimerStart(xHandleTimerTemp, 0);
    xTimerStart(xHandleTimerLockState, 0);
    xTimerStart(xHandleTimerPlugState, 0);
    xTimerStart(xHandleTimerVolt, 0);
    //xTimerStart(xHandleTimerChargingData, 0);
    xTimerStart(xHandleTimerEVSEState, 0);
    xTimerStart(xHandleTimerRFID, 0);
    xTimerStart(xHandleTimerDataRefresh, 0);
    //TimerHeartbeat鍦ㄨ仈缃戝悗鍐嶅惎鍔?
}
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //琚郴缁熻皟鐢?
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
#if 0
void vAssertCalled( const char *pcFile,
                    unsigned long ulLine )
{
    volatile unsigned long ul = 0;

    taskENTER_CRITICAL();
    {
        while( ul == 0 )
        {
            portNOP();
        }
    }
    taskEXIT_CRITICAL();
}
#endif
