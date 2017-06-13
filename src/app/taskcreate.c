/**
* @file taskcreate.c
* @brief 鍒涘缓浠诲姟Todolist 瀵圭収琛?
*        1. 瀹氫箟STACK澶у皬
*        2. 瀹氫箟PRIORITY
*        3. 澹版槑浠诲姟
*        4. 瀹氫箟浠诲姟鍙ユ�?
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
#include "gprs_m26.h"
#include "bsp.h"
#include "modem.h"


/*---------------------------------------------------------------------------/
/ ����ջ��С
/---------------------------------------------------------------------------*/
#define defSTACK_TaskInit                   2048
#define defSTACK_TaskCLI                    1024
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
#define defSTACK_TaskEVSEData               512


//#define TCPIP_THREAD_STACKSIZE      512

/*---------------------------------------------------------------------------/
/ �������ȼ�
/---------------------------------------------------------------------------*/
//���ȼ�����Ϊϵͳ�������ȼ��ͣ�OTA > ������� > ���ϴ��� > ϵͳ���� > ˢ����ͨ�� > ���ݴ�����ϵͳ����
#define defPRIORITY_TaskOTA                 31/* ���*/

#define defPRIORITY_TaskEVSECharge          27
#define defPRIORITY_TaskEVSEDiag            25
#define defPRIORITY_TaskEVSEMonitor         23
//#define configTIMER_TASK_PRIORITY     ( defined in FreeRTOSConfig.h ) 22
#define defPRIORITY_TaskEVSERFID            20
#define defPRIORITY_TaskEVSERemote          18
#define defPRIORITY_TaskEVSEData            16

#define defPRIORITY_TaskPPP                 14
#define defPRIORITY_TaskTCPClient           12
#define defPRIORITY_TaskRemoteCmdProc       8

#define defPRIORITY_TaskInit                10
#define defPRIORITY_TaskTouch               6
#define defPRIORITY_TaskGUI                 4   //���ܸ�,GUI����ʱ��̫��,��Ӱ��Ӳ����Ӧ
#define defPRIORITY_TaskCLI                 2

//#define TCPIP_THREAD_PRIO         13 //defined in lwipopts.h
// define PPP_THREAD_PRIO          14

/*---------------------------------------------------------------------------/
/ ��������
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
/ ��������
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
/ ������
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
/ �����ͨ��
/---------------------------------------------------------------------------*/
EventGroupHandle_t xHandleEventTimerCBNotify = NULL;
EventGroupHandle_t xHandleEventData = NULL;
EventGroupHandle_t xHandleEventDiag = NULL;
EventGroupHandle_t xHandleEventRemote = NULL;
EventGroupHandle_t xHandleEventHMI  = NULL;
EventGroupHandle_t xHandleEventTCP   = NULL;

//�����Event����Ӧ�ṹ���ж���
//pRFIDDev->xHandleEventGroupRFID
//pCON->status.xHandleEventCharge;
//pCON->status.xHandleEventException;
//����
QueueHandle_t xHandleQueueOrders = NULL;
QueueHandle_t xHandleQueueErrorPackage = NULL;
//Timer���
TimerHandle_t xHandleTimerTemp = NULL; //4个温�?
TimerHandle_t xHandleTimerLockState = NULL;
TimerHandle_t xHandleTimerPlugState = NULL;
TimerHandle_t xHandleTimerVolt = NULL;
TimerHandle_t xHandleTimerChargingData = NULL;
TimerHandle_t xHandleTimerEVSEState = NULL;
TimerHandle_t xHandleTimerRFID = NULL;
TimerHandle_t xHandleTimerDataRefresh = NULL;
TimerHandle_t xHandleTimerRemoteHeartbeat = NULL;
TimerHandle_t xHandleTimerRemoteStatus    = NULL;
//con中还定义了几个定时器，xHandleTimerVolt，xHandleTimerCurr，xHandleTimerCharge分别在使用时进行初始�?
//Mutex
void vTaskInit(void *pvParameters)
{
    pModem = DevModemCreate();
    strcpy(pModem->info.strAPN, "CMNET");
    pModem->info.ucContext = 0;
    pModem->xMutex = xSemaphoreCreateMutex();

    modem_open(pModem);
    modem_init(pModem);
    Modem_Poll(pModem);//��������

//    pWIFI = DevWifiCreate();
//    strcpy(pWIFI->info.strSSID, "rgw");
//    strcpy(pWIFI->info.strPWD,"abc666def8");
//    pWIFI->xMutex = xSemaphoreCreateMutex();
//
//    wifi_open(pWIFI);
//    wifi_init(pWIFI);
//    Wifi_Poll(pWIFI);
}
void vTaskCLI(void *pvParameters)
{
    cli_main();
}

void vTaskGUI(void *pvParameters)
{
#ifdef EVSE_DEBUG
    while(1)
    {
        vTaskDelay(1000);
    }
#else
    MainTask();
#endif
}

void vTaskTouch(void *pvParameters)
{
    while(1)
    {
#ifdef EVSE_DEBUG
        while(1)
        {
            vTaskDelay(1000);
        }
#else
        GUI_TOUCH_Exec();//����XY��Ĳ���
        vTaskDelay(10);
#endif
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

    //xTaskCreate( vTaskPPP, TASKNAME_PPP, defSTACK_TaskPPP, NULL, defPRIORITY_TaskPPP, &xHandleTaskPPP );
    //xTaskCreate( vTaskTCPClient, TASKNAME_TCP_CLIENT, defSTACK_TaskTCPClient, NULL, defPRIORITY_TaskTCPClient, &xHandleTaskTCPClient );
    xTaskCreate( vTaskRemoteCmdProc, TASKNAME_RemoteCmdProc, defSTACK_TaskRemoteCmdProc, (void *)pechProto, defPRIORITY_TaskRemoteCmdProc, &xHandleTaskRemoteCmdProc);

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

/** @brief
 */
void AppObjCreate (void)
{
    xHandleEventTimerCBNotify = xEventGroupCreate();
    xHandleEventData = xEventGroupCreate();
    xHandleEventDiag = xEventGroupCreate();
    xHandleEventRemote = xEventGroupCreate();
    xHandleEventHMI = xEventGroupCreate();
    xHandleEventTCP = xEventGroupCreate();


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
    xHandleTimerRemoteHeartbeat = xTimerCreate("TimerHeartbeat", defRemoteHeartbeatCyc, pdTRUE, (void *)defTIMERID_RemoteHeartbeat, vEVSETimerCB);
    xHandleTimerRemoteStatus = xTimerCreate("TimerRemoteStatus", defRemoteStatusCyc, pdTRUE, (void *)defTIMERID_RemoteStatus, vEVSETimerCB);

    xTimerStart(xHandleTimerTemp, 0);
    xTimerStart(xHandleTimerLockState, 0);
    xTimerStart(xHandleTimerPlugState, 0);
    xTimerStart(xHandleTimerVolt, 0);
    //xTimerStart(xHandleTimerChargingData, 0);
    xTimerStart(xHandleTimerEVSEState, 0);
    xTimerStart(xHandleTimerRFID, 0);
    xTimerStart(xHandleTimerDataRefresh, 0);
    //TimerHeartbeatԶ�̷��������Ӻ�����ʱ��
}
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //琚郴缁熻皟�??
void vApplicationTickHook( void )
{
    IWDG_Feed();
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
