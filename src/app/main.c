/**
* @file main.c
* @brief 1. 初始化外设.
*        2. 创建任务.
*           1\ vTaskFailureHandle  对运行中的各种数据进行故障检测，发生故障时启动
*           故障流程
*           2\ vTaskCharger
*           
* @author rgw
* @version v1.0
* @date 2016-10-26
*/

/* Includes ------------------------------------------------------------------*/

#include "includes.h"



/* Private functions ---------------------------------------------------------*/


/* ---------------------------------------------------------------------------*/
/**
* @brief main
*
* @return 
*/
/* ---------------------------------------------------------------------------*/
int main(void)
{
    /**
     * 关闭全局中断(除了NMI和HardFault), 在移植文件port.c中的
     * 函数prvStartFirstTask中会重新开启全局中断。
     */
    DISABLE_INT(); 
    
    /* 硬件初始化 */
    bsp_Init(); 
    my_mem_init(SRAMIN);
    my_mem_init(SRAMEX);		    //初始化SDRAM
    my_mem_init(SRAMCCM);
    /* Activate the use of memory device feature */
    WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到
    printf("\nhello charger\n\r");
    /* 创建任务 */
    AppTaskCreate();
    
    /* 启动调度，开始执行任务 */
    vTaskStartScheduler();

    /* 
      如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或
      者空闲任务的 heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义
      的heap大小：
      #define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 30 * 1024 ) )
    */
    while(1);
}


