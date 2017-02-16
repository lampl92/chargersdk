/**
* @file taskcreate.c
* @brief ��������Todolist ���ձ�
*        1. ����STACK��С
*        2. ����PRIORITY
*        3. ��������
*        4. ����������
*        5. �������
*        6. ��������
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "cli_main.h"
/*---------------------------------------------------------------------------/
/ ����ջ����
/---------------------------------------------------------------------------*/
#define defSTACK_TaskCLI                    512
#define defSTACK_TaskGUI                    1024
#define defSTACK_TaskTouch                  128
#define defSTACK_TaskOTA                    512

#define defSTACK_TaskEVSERemote             512
#define defSTACK_TaskEVSERFID               512
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

#define defPRIORITY_TaskEVSERemote          3
#define defPRIORITY_TaskEVSERFID            4
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
const char *TASKNAME_EVSERemote     = "TaskEVSERemote";
const char *TASKNAME_EVSERFID       = "TaskEVSERFID";
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

void vTaskEVSERemote(void *pvParameters);           //Զ��ͨ��
void vTaskEVSERFID(void *pvParameters);             //ˢ��
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

static TaskHandle_t xHandleTaskEVSERemote = NULL;
static TaskHandle_t xHandleTaskEVSERFID = NULL;
static TaskHandle_t xHandleTaskEVSECharge = NULL;
static TaskHandle_t xHandleTaskEVSEMonitor = NULL;
static TaskHandle_t xHandleTaskEVSEError = NULL;
static TaskHandle_t xHandleTaskEVSEData = NULL;
/*---------------------------------------------------------------------------/
/ ����ͨ��
/---------------------------------------------------------------------------*/
EventGroupHandle_t xHandleEventGroupRFID;
QueueHandle_t xHandleQueueErrorCode;
//�����ʱ��
TimerHandle_t xHandleTimerTemp; //4���¶�
TimerHandle_t xHandleTimerLockState;
TimerHandle_t xHandleTimerGetChargePoint;
TimerHandle_t xHandleTimerCPCCState;
TimerHandle_t xHandleTimerChargingData;
TimerHandle_t xHandleTimerEVSEState;

void vTaskCLI(void *pvParameters)
{
    cli_main();
}

void vTaskGUI(void *pvParameters)
{
    MainTask();
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
    xTaskCreate( vTaskEVSERemote, TASKNAME_EVSERemote, defSTACK_TaskEVSERemote, NULL, defPRIORITY_TaskEVSERemote, &xHandleTaskEVSERemote );
    xTaskCreate( vTaskEVSERFID, TASKNAME_EVSERFID, defSTACK_TaskEVSERFID, NULL, defPRIORITY_TaskEVSERFID, &xHandleTaskEVSERFID );
    xTaskCreate( vTaskEVSECharge, TASKNAME_EVSECharge, defSTACK_TaskEVSECharge, NULL, defPRIORITY_TaskEVSECharge, &xHandleTaskEVSECharge );
    xTaskCreate( vTaskEVSEMonitor, TASKNAME_EVSEMonitor, defSTACK_TaskEVSEMonitor, NULL, defPRIORITY_TaskEVSEMonitor, &xHandleTaskEVSEMonitor );
    xTaskCreate( vTaskEVSEError, TASKNAME_EVSEError, defSTACK_TaskEVSEError, NULL, defPRIORITY_TaskEVSEError, &xHandleTaskEVSEError );
    xTaskCreate( vTaskEVSEData, TASKNAME_EVSEData, defSTACK_TaskEVSEData, NULL, defPRIORITY_TaskEVSEData, &xHandleTaskEVSEData );
}

/** @brief ��������ͨ�Ż��ơ����ź����������ʱ��������������
 */
extern void vEVSETimerCB(TimerHandle_t xTimer);
extern void vChargePointTimerCB(TimerHandle_t xTimer);
void AppObjCreate (void)
{
    xHandleEventGroupRFID = xEventGroupCreate();
    xHandleQueueErrorCode = xQueueCreate(100, sizeof(ErrorPackage_t));

    xHandleTimerTemp = xTimerCreate("TimerTemp", 5000, pdTRUE, (void *)defTIMERID_Temp, vChargePointTimerCB);
    xHandleTimerLockState = xTimerCreate("TimerLockState", 1000, pdTRUE, (void *)defTIMERID_LockState, vChargePointTimerCB);
    xHandleTimerCPCCState = xTimerCreate("TimerCPCCState", 50, pdTRUE, (void *)defTIMERID_CPCCState, vChargePointTimerCB);
    xHandleTimerChargingData = xTimerCreate("TimerChargingData", 50, pdTRUE, (void *)defTIMERID_ChargingData, vChargePointTimerCB);
    xHandleTimerEVSEState = xTimerCreate("TimerEVSEState", 50, pdTRUE, (void *)defTIMERID_EVSEState, vEVSETimerCB);
    xTimerStart(xHandleTimerTemp, 0);
    xTimerStart(xHandleTimerLockState, 0);
    xTimerStart(xHandleTimerCPCCState, 0);
    xTimerStart(xHandleTimerChargingData, 0);
    xTimerStart(xHandleTimerEVSEState, 0);
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
    printf_safe("stackoverflow!! task = %s\n",pcTaskName);
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
