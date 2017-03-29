#include "includes.h"
#include "task.h"
uint8_t acTaskStatusBuffer[500];

void cli_tasklist_fnt(int argc, char **argv)
{
    uint16_t FreeBytesRemaining=0;
    uint16_t memused;
    uint8_t paddr[20];

    printf_safe("\r\n");
    vTaskList((char *)&acTaskStatusBuffer);
    printf_safe("=================================================\r\n");
	printf_safe("������      ����״̬ ���ȼ�   ʣ��ջ �������\r\n");
	printf_safe("%s\r\n", acTaskStatusBuffer);
	printf_safe("\r\n������       ���м���         ʹ����\r\n");
	vTaskGetRunTimeStats((char *)&acTaskStatusBuffer);
    printf_safe("%s\r\n", acTaskStatusBuffer);

    FreeBytesRemaining=xPortGetFreeHeapSize();
    memused = (configTOTAL_HEAP_SIZE-FreeBytesRemaining) * 100 / configTOTAL_HEAP_SIZE;
	xsprintf((char*)paddr,"%d.%01d%%",memused/10,memused%10);
    printf_safe("\nSDRAM ʹ����: %s\n",paddr);
}

tinysh_cmd_t cli_tasklist_cmd={
    0,
    "tasklist",
    "display task status",
    0,
    cli_tasklist_fnt,
    "<cr>",0,0
    };
