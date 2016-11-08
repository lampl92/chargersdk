#include "includes.h"
#include "task.h"
uint8_t acTaskStatusBuffer[500];

void cli_tasklist_fnt(int argc, char **argv)
{
    taskENTER_CRITICAL();
    
    printf("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    printf("=================================================\r\n");
	printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
	printf("%s\r\n", acTaskStatusBuffer);
	printf("\r\n������       ���м���         ʹ����\r\n");
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
