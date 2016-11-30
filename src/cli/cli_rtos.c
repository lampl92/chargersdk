#include "includes.h"
#include "task.h"
uint8_t acTaskStatusBuffer[500];

void cli_tasklist_fnt(int argc, char **argv)
{
    u16 FreeBytesRemaining=0;
    u16 memused;
    u8 paddr[20];				//??P Addr:+p???ASCII?
    taskENTER_CRITICAL();
    
    printf("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    printf("=================================================\r\n");
	printf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
	printf("%s\r\n", acTaskStatusBuffer);
	printf("\r\n������       ���м���         ʹ����\r\n");
	vTaskGetRunTimeStats((char *)&acTaskStatusBuffer);
    printf("%s\r\n", acTaskStatusBuffer);
    
    FreeBytesRemaining=xPortGetFreeHeapSize();
    memused = (configTOTAL_HEAP_SIZE-FreeBytesRemaining) * 100 / configTOTAL_HEAP_SIZE;
	sprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
    printf("SDRAM ʹ����: %s\n",paddr);
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
