/**
* @file main.c
* @brief 1. 初始化外设.
*
* @author rgw
* @version v1.0
* @date 2016-10-26
*/

/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "bsp_define.h"
/* Private functions ---------------------------------------------------------*/

int main(void)
{
    SCnSCB->ACTLR |= SCnSCB_ACTLR_DISDEFWBUF_Msk;
    /**
     * 关闭全局中断(除了NMI和HardFault), 在移植文件port.c中的
     * 函数prvStartFirstTask中会重新开启全局中断。
     */
    DISABLE_INT();

    bsp_Init();
    sys_Init();

    EVSEinit();

    TaskInit();
    SysTaskCreate();
    AppTaskCreate();
    AppObjCreate();

    vTaskStartScheduler();

    /*
      如果系统正常启动是不会运行到这里的，运行到这里极有可能是用于定时器任务或
      者空闲任务的 heap空间不足造成创建失败，此要加大FreeRTOSConfig.h文件中定义
      的heap大小：
      #define configTOTAL_HEAP_SIZE       ( ( size_t ) ( 30 * 1024 ) )
    */
    while(1);
}


