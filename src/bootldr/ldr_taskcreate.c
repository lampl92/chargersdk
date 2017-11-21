/**
* @file taskcreate.c
* @brief 创建任务Todolist 对照表
*        1. 定义STACK大小
*        2. 定义PRIORITY
*        3. 声明任务
*        4. 定义任务句柄
*        5. 任务入口
*        6. 创建任务创建任务Todolist 对照表
* @author rgw
* @version v1.0
* @date 2016-11-03
*/
#include "includes.h"
#include "interface.h"
#include "cli_main.h"
#include "timercallback.h"
#include "bsp.h"
#include "utils.h"
#include "stringName.h"
#include "bsp_stmflash.h"
#include "cfg_parse.h"
#include "yaffsfs.h"

#define APP_ADDRESS         ADDR_FLASH_SECTOR_6

/*---------------------------------------------------------------------------/
/ 任务栈大小
/---------------------------------------------------------------------------*/
#define defSTACK_TaskInit                   (1024*10)
#define defSTACK_TaskCLI                    (1024 * 50)
#define defSTACK_TaskOTA                    512

/*---------------------------------------------------------------------------/
/ 任务优先级
/---------------------------------------------------------------------------*/
//优先级规则为系统任务优先级低，OTA > 充电任务 > 故障处理 > 系统监视 > 刷卡与通信 > 数据处理与系统任务
#define defPRIORITY_TaskOTA                 31/* 最高*/
#define defPRIORITY_TaskInit                10
#define defPRIORITY_TaskCLI                 2

/*---------------------------------------------------------------------------/
/ 任务名称
/---------------------------------------------------------------------------*/
const char *TASKNAME_INIT           = "TaskInit";
const char *TASKNAME_CLI            = "TaskCLI";
const char *TASKNAME_OTA            = "TaskOTA";



/*---------------------------------------------------------------------------/
/ 任务声明
/---------------------------------------------------------------------------*/
void vTaskInit(void *pvParameters);
void vTaskCLI(void *pvParameters);
void vTaskOTA(void *pvParameters);

/*---------------------------------------------------------------------------/
/ 任务句柄
/---------------------------------------------------------------------------*/
static TaskHandle_t xHandleTaskInit = NULL;
static TaskHandle_t xHandleTaskCLI = NULL;
static TaskHandle_t xHandleTaskOTA = NULL;
/*---------------------------------------------------------------------------/
/ 任务间通信
/---------------------------------------------------------------------------*/
SemaphoreHandle_t xMutexTimeStruct;
SemaphoreHandle_t xMutexNandHW;
SemaphoreHandle_t  xprintfMutex = NULL;


EventGroupHandle_t xHandleEventTimerCBNotify = NULL;
EventGroupHandle_t xHandleEventData = NULL;
EventGroupHandle_t xHandleEventDiag = NULL;
EventGroupHandle_t xHandleEventRemote = NULL;
EventGroupHandle_t xHandleEventHMI  = NULL;
EventGroupHandle_t xHandleEventTCP   = NULL;

//队列

QueueHandle_t xHandleQueueErrorPackage = NULL;

//con中还定义了几个定时器，xHandleTimerVolt，xHandleTimerCurr，xHandleTimerCharge分别在使用时进行初始化
//Mutex
extern void fs_init(void);
extern void *_app_start[];

uint8_t *GetFileBuffer(char *path, uint32_t *psize)
{
    int fd;
    uint32_t fsize;
    struct yaffs_stat st;
    uint32_t br;
    int fres;
    
    uint8_t *pbuff = NULL;
    
    fd = yaffs_open(path, O_EXCL | O_RDONLY, 0);
    fres = yaffsfs_GetLastError();
    if (fres != 0)
    {
        printf_safe("No %s!\n", path);
        return NULL;
    }
    yaffs_stat(path, &st);
    fsize = st.st_size;
    pbuff = (uint8_t *)malloc(fsize * sizeof(uint8_t));
    if (pbuff == NULL)
    {
        printf_safe("Malloc error!\n");
        yaffs_close(fd);
        return NULL;
    }
    br = yaffs_read(fd, pbuff, fsize);
    if (fsize == br)
    {
        yaffs_close(fd);
        *psize = fsize;
        return pbuff;
    }
    else
    {
        yaffs_close(fd);
        *psize = 0;
        free(pbuff);
        pbuff = NULL;
        return NULL;
    }
}

uint32_t initstart;
void vTaskInit(void *pvParameters)
{
    uint8_t *pucBinBuffer;
    uint32_t size;
    ul2uc xBinBuffer;
    uint32_t crc32;
    uint32_t trymax = 5;
    uint32_t tryread = 0;
    uint8_t upflag;
    
    char cli_std[1];
    uint32_t cli_std_len;
    
    AppObjCreate();
    sys_Init();
    while (1)
    {
        cli_std_len = uart_read(UART_PORT_CLI, cli_std, 1, 3000);
        if (cli_std_len >= 1 && cli_std[0] == 'c')
        {
            cli_std[0] = 0;
            cli_std_len = 0;
            SysTaskCreate();
            printf_safe("task init sleep.zZ\n");
            while (1)
            {
                if (initstart == 1)
                {
                    initstart = 0;
                    break;
                }
                vTaskDelay(1000);
            }
        }
        if (tryread <= trymax)
        {
            upflag = 0;
            if (xSysconf.xUpFlag.chargesdk_bin == 1)
            {
                ++tryread;
                pucBinBuffer = GetFileBuffer(pathBin, &size);
                printf_safe("Get Buffer OK!\n");

                if (pucBinBuffer != NULL && size > 0)
                {
                    GetBufferCrc32(pucBinBuffer, size, &crc32);
                    if (crc32 != (uint32_t)xSysconf.xUpFlag.chargesdk_bin_crc32)
                    {
                        printf_safe("Get Crc32 Err!\n");
                        free(pucBinBuffer);
                        size = 0;
                    }
                    else
                    {
                        printf_safe("Get Crc32 OK! crc32 = %x\n", crc32);
                        STMFLASH_Write(APP_ADDRESS, (uint32_t *)pucBinBuffer, size / 4);
                        free(pucBinBuffer);
                        upflag = 2;
                    }
                }
            }
            else if (xSysconf.xUpFlag.chargesdk_bin == 0 || xSysconf.xUpFlag.chargesdk_bin == 2)
            {
                printf_safe("direct app_start!\n");
                asm("cpsid i");
                asm("ldr sp, =_estack");
                ((void(*)())_app_start[1])();
            }
        
            if (upflag == 2)
            {
                xSysconf.SetSysCfg(jnSysChargersdk_bin, (void *)&upflag, ParamTypeU8);
                printf_safe("up OK, app_start!\n");
                asm("cpsid i");
                asm("ldr sp, =_estack");
                ((void(*)())_app_start[1])();
            }
        }
        else
        {
            printf_safe("升级失败, 请手动重启或检查待升级固件!\n");
        }
       
        vTaskDelay(1000);
    }
}
void vTaskCLI(void *pvParameters)
{
    cli_main();
}


void TaskInit(void)
{
    xTaskCreate( vTaskInit, TASKNAME_INIT, defSTACK_TaskInit, NULL, defPRIORITY_TaskInit, &xHandleTaskInit );
}

void SysTaskCreate (void)
{
    xTaskCreate( vTaskCLI, TASKNAME_CLI, defSTACK_TaskCLI, NULL, defPRIORITY_TaskCLI, &xHandleTaskCLI );
    //xTaskCreate( vTaskOTA, TASKNAME_OTA, defSTACK_TaskOTA, NULL, defPRIORITY_TaskOTA, &xHandleTaskOTA );
}

/** @brief
 */
void AppObjCreate (void)
{
    xMutexTimeStruct = xSemaphoreCreateMutex();
    xMutexNandHW = xSemaphoreCreateMutex();
    xprintfMutex = xSemaphoreCreateMutex();
    

    xHandleQueueErrorPackage = xQueueCreate(100, sizeof(ErrorPackage_t));

    //TimerHeartbeat远程服务器连接后开启定时器
}
volatile uint32_t ulHighFrequencyTimerTicks = 0UL; //被系统调用
extern __IO uint32_t uwTick;
void vApplicationTickHook( void )
{
    ulHighFrequencyTimerTicks = xTaskGetTickCount();
    uwTick = ulHighFrequencyTimerTicks;
}

/**
* @brief vApplicationMallocFailedHook() will only be called if
    configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
    function that will get called if a call to pvPortMalloc() fails.
    pvPortMalloc() is called internally by the kernel whenever a task, queue,
    timer or semaphore is created.  It is also called by various parts of the
    demo application.  If heap_1.c or heap_2.c are used, then the size of the
    heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
    FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
    to query the size of free heap space that remains (although it does not
    provide information on how the remaining heap might be fragmented).
*/
void vApplicationMallocFailedHook( void )
{
    taskDISABLE_INTERRUPTS();
    for( ;; );
}

/**
* @brief vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is
set
    to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
    task.  It is essential that code added to this hook function never attempts
    to block in any way (for example, call xQueueReceive() with a block time
    specified, or call vTaskDelay()).  If the application makes use of the
    vTaskDelete() API function (as this demo application does) then it is also
    important that vApplicationIdleHook() is permitted to return to its calling
    function, because it is the responsibility of the idle task to clean up
    memory allocated by the kernel to any task that has since been deleted.
*/
void vApplicationIdleHook( void )
{
}

/**
* @brief
*
* @param pxTask
* @param pcTaskName
*/
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
    ( void ) pcTaskName;
    ( void ) pxTask;

    /* Run time stack overflow checking is performed if
    configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
    function is called if a stack overflow is detected. */
    taskDISABLE_INTERRUPTS();
    for( ;; );
}
