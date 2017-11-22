/**
* @file FreeRTOSHook.c
* @brief
* @author rgw
* @version v1.0
* @date 2016-10-26
*/
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //被系统调用
extern __IO uint32_t uwTick;
void vApplicationTickHook(void)
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
void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
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
void vApplicationIdleHook(void)
{
}

/**
* @brief
*
* @param pxTask
* @param pcTaskName
*/
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    (void) pcTaskName;
    (void) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for (;;)
        ;
}
#if 0
void vAssertCalled( const char *pcFile,
    unsigned long ulLine)
{
    volatile unsigned long ul = 0;

    taskENTER_CRITICAL();
    {
        while (ul == 0)
        {
            portNOP();
        }
    }
    taskEXIT_CRITICAL();
}
#endif