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
int main(void)
{
    /**
     * 关闭全局中断(除了NMI和HardFault), 在移植文件port.c中的
     * 函数prvStartFirstTask中会重新开启全局中断。
     */
    DISABLE_INT();

    bsp_Init();
    sys_Init();

    SysTaskCreate();
    AppTaskCreate();

    vTaskStartScheduler();

    /*
      如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或
      者空闲任务的 heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义
      的heap大小：
      #define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 30 * 1024 ) )
    */
    while(1);
}


