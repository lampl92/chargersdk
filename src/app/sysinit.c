#include "includes.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"
#include "malloc.h"

#if configAPPLICATION_ALLOCATED_HEAP == 1
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((at(0XC0300000)));//used by heap_4.c
#endif

FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
char NANDDISKPath[4]; /* RAM disk logical drive path */

static void Error_Handler()
{
    while(1);
}

void sys_Init(void)
{
    
    my_mem_init(SRAMIN);		    //初始化内部内存池
	my_mem_init(SRAMEX);		    //初始化外部内存池
	my_mem_init(SRAMCCM);		    //初始化CCM内存池
    
/*
*********************************************************************************************************
*                                        FATFS初始化
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
*                                        GUI初始化
*********************************************************************************************************
*/
    /* Activate the use of memory device feature */
    WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到
    
    printf("\nsystem initialized\n\r");
    printf("\nhello charger\n\r");
}
