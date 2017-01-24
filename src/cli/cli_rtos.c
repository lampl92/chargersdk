#include "includes.h"
#include "task.h"
uint8_t acTaskStatusBuffer[500];

void cli_tasklist_fnt(int argc, char **argv)
{
    u16 FreeBytesRemaining=0;
    u16 memused;
    u8 paddr[20];
    taskENTER_CRITICAL();

    xprintf("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    xprintf("=================================================\r\n");
	xprintf("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
	xprintf("%s\r\n", acTaskStatusBuffer);
	xprintf("\r\n������       ���м���         ʹ����\r\n");
	vTaskGetRunTimeStats((char *)&acTaskStatusBuffer);
    xprintf("%s\r\n", acTaskStatusBuffer);

    FreeBytesRemaining=xPortGetFreeHeapSize();
    memused = (configTOTAL_HEAP_SIZE-FreeBytesRemaining) * 100 / configTOTAL_HEAP_SIZE;
	xsprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
    xprintf("SDRAM ʹ����: %s\n",paddr);
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
