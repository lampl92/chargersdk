#include "includes.h"
#include "task.h"
uint8_t acTaskStatusBuffer[500];
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //被系统调用

void cli_tasklist_fnt(int argc, char **argv)
{
    taskENTER_CRITICAL();
    ulHighFrequencyTimerTicks = xTaskGetTickCount();
    printf("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    printf("=================================================\r\n");
	printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
	printf("%s\r\n", acTaskStatusBuffer);
	printf("\r\n任务名       运行计数         使用率\r\n");
	vTaskGetRunTimeStats((char *)&acTaskStatusBuffer);
    printf("%s\r\n", acTaskStatusBuffer);
    taskEXIT_CRITICAL();
}

tinysh_cmd_t cli_tasklist_cmd={
    0,
    "tasklist",
    "display task status",
    0,
    cli_tasklist_fnt,
    "<cr>",0,0
    };
