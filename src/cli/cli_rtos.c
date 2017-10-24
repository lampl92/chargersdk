#include "includes.h"
#include "task.h"
#include "interface.h"
uint8_t acTaskStatusBuffer[500];

void cli_tasklist_fnt(int argc, char **argv)
{
    uint32_t FreeBytesRemaining = 0;
    uint32_t memused;
    double usedkb;
    uint8_t paddr[20];
    int i;

    printf_safe("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    printf_safe("=================================================\r\n");
    printf_safe("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
    for(i = 0; i < strlen(acTaskStatusBuffer); i++)
    {
        printf_safe("%c", acTaskStatusBuffer[i]);
    }
    printf_safe("\r\n任务名       运行计数         使用率\r\n");
    vTaskGetRunTimeStats((char *)&acTaskStatusBuffer);
    for(i = 0; i < strlen(acTaskStatusBuffer); i++)
    {
        printf_safe("%c", acTaskStatusBuffer[i]);
    }
    FreeBytesRemaining = xPortGetFreeHeapSize();
    memused = (configTOTAL_HEAP_SIZE - FreeBytesRemaining) * 100 / configTOTAL_HEAP_SIZE;
    sprintf((char *)paddr, "%d.%01d%%", memused / 10, memused % 10);
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

