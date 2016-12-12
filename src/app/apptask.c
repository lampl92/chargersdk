/**
* @file apptask.c
* @brief
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "GUIDEMO.h"

/* Private function prototypes
-----------------------------------------------*/

static void vTaskCLI(void *pvParameters);
static void vTaskGUI(void *pvParameters);
static void vTaskTouch(void *pvParameters);
static void vTaskStart(void *pvParameters);

/* Private variables
������-------------------------------------------------*/

TaskHandle_t xHandleTaskCLI = NULL;
TaskHandle_t xHandleTaskGUI = NULL;
TaskHandle_t xHandleTaskTouch = NULL;
TaskHandle_t xHandleTaskStart = NULL;
/*
---------------------------------------------------------------------------*/
/**
* @brief �û��ӿ����ȼ�1  (��ֵԽС���ȼ�Խ�ͣ������uCOS�෴)
*
* @param pvParameters
*/
/*
---------------------------------------------------------------------------*/
static void vTaskCLI(void *pvParameters)
{
//    while(1)
//    {
//        //bsp_LedToggle(1);
//        printf("TaskUserIF\n\r");
//
//        vTaskDelay(100);
//    }
    cli_main();
}

/*
---------------------------------------------------------------------------*/
/**
* @brief ���ȼ�2
*
* @param pvParameters
*/
/*
---------------------------------------------------------------------------*/
static void vTaskGUI(void *pvParameters)
{
    MainTask();
        vTaskDelay(1000);
}

/*
---------------------------------------------------------------------------*/
/**
* @brief ���ȼ�3
*
* @param pvParameters ���ڴ���������ʱ���ݵ��β�
*/
/*
---------------------------------------------------------------------------*/
static void vTaskTouch(void *pvParameters)
{
    while(1)
    {
        GUI_TOUCH_Exec();
        //TouchTask();
        vTaskDelay(10);
    }
}
/*
---------------------------------------------------------------------------*/
/**
* @brief ���ȼ�4
*
* @param pvParameters
*/
/*
---------------------------------------------------------------------------*/
static void vTaskStart(void *pvParameters)
{
    while(1)
    {
        /* ����ɨ�� */
        //bsp_LedToggle(4);
        //printf("TaskStart\r\n");
        vTaskDelay(1000);
    }
}
/*
---------------------------------------------------------------------------*/
/**
* @brief ����Ӧ������
*/
/*
---------------------------------------------------------------------------*/
void AppTaskCreate (void)
{
    xTaskCreate( vTaskCLI,       /* ������  */
                 "vTaskCLI",         /* ������    */
                 1024,                   /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 1,                     /* �������ȼ�*/
                 &xHandleTaskCLI );  /* ������  */


    xTaskCreate( vTaskGUI,          /* ������  */
                 "vTaskGUI",        /* ������    */
                 1024,               /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,              /* �������  */
                 2,                 /* �������ȼ�*/
                 &xHandleTaskGUI ); /* ������  */

    xTaskCreate( vTaskTouch,           /* ������  */
                 "vTaskTouch",         /* ������    */
                 512,                   /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 3,                     /* �������ȼ�*/
                 &xHandleTaskTouch );  /* ������  */


    xTaskCreate( vTaskStart,            /* ������  */
                 "vTaskStart",          /* ������    */
                 512,                   /* ����ջ��С����λword��Ҳ����4�ֽ� */
                 NULL,                  /* �������  */
                 4,                     /* �������ȼ�*/
                 &xHandleTaskStart );   /* ������  */
}

/*
---------------------------------------------------------------------------*/
/**
* @brief
*/
/*
---------------------------------------------------------------------------*/
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

