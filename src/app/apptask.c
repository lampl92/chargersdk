/**
* @file apptask.c
* @brief 
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"

/* Private function prototypes 
-----------------------------------------------*/

static void vTaskCLI(void *pvParameters);
static void vTaskLED(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);

/* Private variables 
任务句柄-------------------------------------------------*/

TaskHandle_t xHandleTaskCLI = NULL;
TaskHandle_t xHandleTaskLED = NULL;
TaskHandle_t xHandleTaskMsgPro = NULL;
TaskHandle_t xHandleTaskStart = NULL;
/* 
---------------------------------------------------------------------------*/
/**
* @brief 用户接口优先级1  (数值越小优先级越低，这个跟uCOS相反)
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
* @brief 优先级2
*
* @param pvParameters
*/
/* 
---------------------------------------------------------------------------*/
static void vTaskLED(void *pvParameters)
{
    while(1)
    {
        //bsp_LedToggle(2);
        //printf("TaskLED\r\n");
        vTaskDelay(1000);
    }
}

/* 
---------------------------------------------------------------------------*/
/**
* @brief 优先级3
*
* @param pvParameters 是在创建该任务时传递的形参
*/
/* 
---------------------------------------------------------------------------*/
static void vTaskMsgPro(void *pvParameters)
{
    while(1)
    {
        //bsp_LedToggle(3);
        //printf("TaskMsg\r\n");
        vTaskDelay(1000);
    }
}
/* 
---------------------------------------------------------------------------*/
/**
* @brief 优先级4
*
* @param pvParameters
*/
/* 
---------------------------------------------------------------------------*/
static void vTaskStart(void *pvParameters)
{
    while(1)
    {
        /* 按键扫描 */
        //bsp_LedToggle(4);
        //printf("TaskStart\r\n");
        vTaskDelay(1000);
    }
}
/* 
---------------------------------------------------------------------------*/
/**
* @brief 创建应用任务
*/
/* 
---------------------------------------------------------------------------*/
void AppTaskCreate (void)
{
    xTaskCreate( vTaskCLI,       /* 任务函数  */
                 "vTaskCLI",         /* 任务名    */
                 512,                   /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 1,                     /* 任务优先级*/
                 &xHandleTaskCLI );  /* 任务句柄  */
    
    
    xTaskCreate( vTaskLED,          /* 任务函数  */
                 "vTaskLED",        /* 任务名    */
                 512,               /* 任务栈大小，单位word，也就是4字节 */
                 NULL,              /* 任务参数  */
                 2,                 /* 任务优先级*/
                 &xHandleTaskLED ); /* 任务句柄  */
    
    xTaskCreate( vTaskMsgPro,           /* 任务函数  */
                 "vTaskMsgPro",         /* 任务名    */
                 512,                   /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 3,                     /* 任务优先级*/
                 &xHandleTaskMsgPro );  /* 任务句柄  */
    
    
    xTaskCreate( vTaskStart,            /* 任务函数  */
                 "vTaskStart",          /* 任务名    */
                 512,                   /* 任务栈大小，单位word，也就是4字节 */
                 NULL,                  /* 任务参数  */
                 4,                     /* 任务优先级*/
                 &xHandleTaskStart );   /* 任务句柄  */
}

/* 
---------------------------------------------------------------------------*/
/**
* @brief 
*/
/* 
---------------------------------------------------------------------------*/
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

