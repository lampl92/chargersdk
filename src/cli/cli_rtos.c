#include "includes.h"
#include "task.h"
#include "interface.h"

void print_stack(void)
{
    printf_safe("stack = %d\n", uxTaskGetStackHighWaterMark(NULL));
}

void cli_taskstack_fnt(int argc, char **argv)
{
    UBaseType_t uxHighWaterMark;
    TaskHandle_t xHandle;

    xHandle = xTaskGetHandle(argv[1]);
    while (1)
    {
        uxHighWaterMark = uxTaskGetStackHighWaterMark(xHandle);
        printf_safe("%s stack = %d\n", argv[1], uxHighWaterMark);
        vTaskDelay(1);
    }
}

void cli_tasklist_fnt(int argc, char **argv)
{
    char acTaskStatusBuffer[2000] = { 0 };
    uint32_t FreeBytesRemaining = 0;
    double memused;
    double usedkb;
    char paddr[20] = { 0 };

    printf_safe("\r\n");
    printf_safe("=================================================\r\n");
    printf_safe("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
    vTaskList(acTaskStatusBuffer);
    printf_safe("%s", acTaskStatusBuffer);
    
    printf_safe("\r\n任务名       运行计数         使用率\r\n");
    vTaskGetRunTimeStats(acTaskStatusBuffer);
    printf_safe("%s", acTaskStatusBuffer);
    
    FreeBytesRemaining = xPortGetFreeHeapSize();
    memused = (configTOTAL_HEAP_SIZE - FreeBytesRemaining) * 100.0 / configTOTAL_HEAP_SIZE;
    sprintf(paddr, "%.2lf%%", memused);
    usedkb = (configTOTAL_HEAP_SIZE - FreeBytesRemaining) / 1024;
    printf_safe("\nSDRAM 使用率: %s，%.2lf KB\n", paddr, usedkb);
}

tinysh_cmd_t cli_tasklist_cmd =
{
    0,
    "tasklist",
    "display task status",
    0,
    cli_tasklist_fnt,
    "<cr>", 0, 0
};
tinysh_cmd_t cli_taskstack_cmd =
{
    0,
    "taskstack",
    "display task stack",
    0,
    cli_taskstack_fnt,
    "<cr>",
    0,
    0
};

