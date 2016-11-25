#include "includes.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"

FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
char NANDDISKPath[4]; /* RAM disk logical drive path */

static void Error_Handler()
{
    while(1);
}

void sys_Init(void)
{
/*
*********************************************************************************************************
*                                        �ڴ�����ʼ��
*********************************************************************************************************
*/
    my_mem_init(SRAMIN);
    my_mem_init(SRAMEX);
    my_mem_init(SRAMCCM);

/*
*********************************************************************************************************
*                                        FATFS��ʼ��
*********************************************************************************************************
*/
    /*##-1- Link the NAND disk I/O driver #######################################*/
    if(FATFS_LinkDriver(&NANDDISK_Driver, NANDDISKPath) == 0)
    {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 1) != FR_OK)
        {
            /* FatFs Initialization Error */
            Error_Handler();
        }
    }
    
/*
*********************************************************************************************************
*                                        GUI��ʼ��
*********************************************************************************************************
*/
    /* Activate the use of memory device feature */
    WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //����STemWin�໺��,RGB�����õ�
    
    printf("\nsystem initialized\n\r");
    printf("\nhello charger\n\r");
}
