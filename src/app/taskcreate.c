/**
* @file taskcreate.c
* @brief
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "RFIDReader_mt626.h"

//��������Todolist ���ձ�
//1. ����STACK��С
//2. ����PRIORITY
//3. ��������
//4. ����������
//5. �������
//6. ��������

//ϵͳ
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


//���ȼ�����Ϊϵͳ�������ȼ��ͣ�OTA > ������� > ���ϴ��� > ϵͳ���� > ˢ����ͨ�� > ���ݴ�����ϵͳ����
//ϵͳ
#define defPRIORITY_TaskCLI                 1
#define defPRIORITY_TaskGUI                 1
#define defPRIORITY_TaskTouch               1
#define defPRIORITY_TaskOTA                 15 //���
//APP
#define defPRIORITY_TaskEVSERemoteComm      3
#define defPRIORITY_TaskEVSECard            3
#define defPRIORITY_TaskEVSECharge          5
#define defPRIORITY_TaskEVSEMonitor         7
#define defPRIORITY_TaskEVSEError           9
#define defPRIORITY_TaskEVSEData            1


//ϵͳ����
static void vTaskCLI(void *pvParameters);
static void vTaskGUI(void *pvParameters);
static void vTaskTouch(void *pvParameters);
static void vTaskOTA(void *pvParameters);               //��������
//APP����
static void vTaskEVSERemoteComm(void *pvParameters);    //Զ��ͨ��
static void vTaskEVSECard(void *pvParameters);          //ˢ��
static void vTaskEVSECharge(void *pvParameters);        //���
static void vTaskEVSEMonitor(void *pvParameters);       //���
static void vTaskEVSEError(void *pvParameters);   //������
static void vTaskEVSEData(void *pvParameters);    //���ݴ���
static void vTaskOTA(void *pvParameters);               //��������

//ϵͳ������
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskGUI = NULL;
static TaskHandle_t xHandleTaskTouch = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;
//APP������
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

