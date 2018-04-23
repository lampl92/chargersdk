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
#include "bsp_cpu_flash.h"
#include "cfg_parse.h"
#include "yaffsfs.h"


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
#define APP_ADDRESS         (uint32_t)_app_start


uint8_t set_upgrade_tmp(char *path, char *flg)
{
    int fd;
    int bw;
    fd = yaffs_open(path, O_CREAT | O_TRUNC | O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-open");
        return 0;
    }
    taskENTER_CRITICAL();
    bw = yaffs_write(fd, flg, 1);
    taskEXIT_CRITICAL();
    if (1 != bw)
    {
        ThrowFSCode(yaffs_get_error(), path, "set_upgrade_tmp()-write");
        yaffs_close(fd);
        return 0;
    }
    yaffs_close(fd);
    return 1;
}


uint8_t *GetFileBuffer(char *path, uint32_t *psize)
{
    int fd;
    uint32_t fsize;
    struct yaffs_stat st;
    uint32_t br;
    int fres = 0;
    
    uint8_t *pbuff = NULL;
    
    fd = yaffs_open(path, O_EXCL | O_RDONLY, 0);
    if (fd < 0)
    {
        fres = yaffs_get_error();
    }
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

void Jump_To_APP(void)
{
    asm("cpsid i");
    asm("ldr sp, =_estack");
    ((void(*)())_app_start[1])();
}

uint32_t initstart;
void vTaskInit(void *pvParameters)
{
    uint8_t *pucBinBuffer;
    uint32_t size;
    uint32_t crc32;
    uint32_t trymax = 3;
    uint32_t tryread = 0;
    char upflag;
    
    char cli_std[1];
    uint32_t cli_std_len;
    char strFindName[256];
    
    uint32_t crc32_calc, crc32_orig;
    char ch_crc32[9] = { 0 };
    ul2uc ul2ucCrc32;
    char filepath[64 + 1];
    int i;
    
    AppObjCreate();
    sys_Init();
    printf_safe("\nPRESS 'C' IN 3 SECONDS FOR CLI MODE...\n");
    while (1)
    {
        cli_std_len = uart_read(UART_PORT_CLI, cli_std, 1, 3000);
        if (cli_std_len >= 1 && (cli_std[0] == 'c' || cli_std[0] == 'C'))
        {
            cli_std[0] = 0;
            cli_std_len = 0;
            SysTaskCreate();
            printf_safe("task init sleep.zZ\n");
            while (1)
            {
                if (initstart == 1)
                {
                    xSysconf.GetSysCfg((void *)&xSysconf, NULL);
                    initstart = 0;
                    break;
                }
                vTaskDelay(1000);
            }
        }
        if (tryread < trymax)
        {
            if (find_file(pathUpgradeDir, "new_fw", strFindName) == 1)
            {
                ++tryread;
                for (i = 0; i < 8; i++)
                {
                    ch_crc32[i] = strFindName[i + 7];
                }
                StrToHex(ch_crc32, ul2ucCrc32.ucVal, strlen(ch_crc32));
                crc32_orig = utils_ntohl(ul2ucCrc32.ulVal);
                
                sprintf(filepath, "%s%s", pathUpgradeDir, strFindName);
                pucBinBuffer = GetFileBuffer(filepath, &size);
                if (pucBinBuffer != NULL && size > 0)
                {
                    GetBufferCrc32(pucBinBuffer, size, &crc32_calc);
                    printf_safe("crc32_calc = %x\n", crc32_calc);
                    printf_safe("crc32_orgi = %x\n", crc32_orig);
                    if (crc32_calc != crc32_orig)
                    {
                        printf_safe("Get Crc32 Err!\n");
                        free(pucBinBuffer);
                    }
                    else
                    {
                        printf_safe("Crc32 OK!\n");
                        printf_safe("正在升级程序，请勿断电！\n");
                        bsp_WriteCpuFlash(APP_ADDRESS, pucBinBuffer, size);
                        free(pucBinBuffer);
                        yaffs_unlink(filepath);
                        upflag = '2';
                        set_upgrade_tmp(pathUpgradeTmp, &upflag);
                    }
                }
            }
            else
            {
                printf_safe("app_start!\n");
                Jump_To_APP();
            }
        }
        else
        {
            yaffs_unlink(filepath);
            upflag = '3';
            set_upgrade_tmp(pathUpgradeTmp, &upflag);
            printf_safe("升级失败, 请手动重启或检查待升级固件与CRC32值!\n");
        }
       
        vTaskDelay(2000);
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

