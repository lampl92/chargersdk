#include "includes.h"
#include "yaffsfs.h"
#include "cJSON.h"
#include <time.h>
#include "stringName.h"
#include "factorycfg.h"
#include "cfg_sys.h"
#include "cfg_parse.h"
#include "yaffs2misc.h"
#include "evse_version.h"
#include "ifconfig.h"
#include "interface_network.h"
#include "utils.h"

#if configAPPLICATION_ALLOCATED_HEAP == 1
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((at(0XC0B00000)));//used by heap_4.c
uint8_t *ucHeap = (uint8_t *)(0XC1500000);//used by heap_4.c
#endif

Sysconf_t   xSysconf;//存放系统初始化参数

extern time_t time_dat;
void timeInit()
{
    time_t settime;
    struct tm orig;
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);

    orig.tm_sec = RTC_TimeStruct.Seconds;
    orig.tm_min = RTC_TimeStruct.Minutes;
    orig.tm_hour = RTC_TimeStruct.Hours;
    orig.tm_mday = RTC_DateStruct.Date;
    orig.tm_mon = RTC_DateStruct.Month - 1;
    orig.tm_year = 2000 + RTC_DateStruct.Year - 1900;
//    orig.tm_wday = 5;
//    orig.tm_yday = 19;
    orig.tm_isdst = -1;
    putenv("TZ=Etc/GMT-8");
    tzset();
    settime = mktime (&orig);
    time(&settime);
}

char g_strChipID[24 + 1] = { 0 };
void Get_ChipID(void)
{
    int i;
    __IO uint8_t *pid;
    pid = (__IO uint8_t *)0x1FFF7A10;
    for (i = 0; i < 6; i++)
    {
        g_strChipID[i] = pid[i+6];
    }
    //printf_safe("ChipID: %s\n", g_strChipID);
//    HexToStr((uint8_t *)pid, g_strChipID, 12);
}
uint8_t create_dir(char *dir)
{
    int res = 1;
    res = yaffs_mkdir(dir, S_IREAD | S_IWRITE);
    if (res != 0)
    {
        res = yaffs_get_error();
    }
    switch (res)
    {
    case 0:
    case -EEXIST:
        return TRUE;
    default:
        return FALSE;
    }
}


void create_cfg_file(const char *path, const char *context)
{
    uint32_t bw;
    int fd;
    int res = 1;
    fd = yaffs_open(path, O_CREAT | O_EXCL | O_RDWR, S_IWRITE | S_IREAD);
    if (fd < 0)
    {
        res = yaffs_get_error();
    }
    else
    {
        bw = yaffs_write(fd, context, strlen(context));
        yaffs_close(fd);
    }
}
extern void retarget_init(void);
void yaffs_init(void)
{
    int res;
    yaffs_start_up();
    yaffs_set_trace(0);
    res = yaffs_mount(YAFFS_MOUNT_POINT);
    if (res != 0)
    {
        yaffs_format(YAFFS_MOUNT_POINT, 0, 0, 0);
        yaffs_mount(YAFFS_MOUNT_POINT);
    }
}
extern void cli_init(void);
void sys_Init(void)
{
    int res;
    Get_ChipID();
    retarget_init();
    cli_init();
    timeInit();
    yaffs_init();
    /*---------------------------------------------------------------------------/
    /                               系统参数初始化
    /---------------------------------------------------------------------------*/
    create_dir(pathSystemDir);
    create_dir(pathDownloadDir);
    create_dir(pathUpgradeDir);
    create_dir(pathResourceDir);
#if BOOTLOADER
#else
    create_cfg_file(pathEVSECfg, strEVSECfg);
    create_cfg_file(pathProtoCfg, strProtoCfg);
    create_cfg_file(pathWhiteList, strWhiteListCfg);
    create_cfg_file(pathBlackList, strBlackListCfg);
    create_cfg_file(pathOrder, strOrderCfg);
    create_cfg_file(pathEVSELog, strLogCfg);
    create_cfg_file(pathSysCfg, strSysCfg);
    create_cfg_file(pathFTPCfg, strFtpCfg);
    create_cfg_file(pathNetCfg, strNetCfg);
    create_cfg_file(pathMeterCfg, strMeterCfg);
    dump_directory_tree(YAFFS_MOUNT_POINT);
    
    SysCfgInit(&xSysconf);
    xSysconf.GetSysCfg((void *)&xSysconf, NULL);
    sprintf(xSysconf.strVersion, "%s.%s.%s", EVSE_MAJOR, EVSE_MINOR, EVSE_PATCH);
#endif
    /*---------------------------------------------------------------------------/
    /                               NET初始化
    /---------------------------------------------------------------------------*/
#if EVSE_USING_NET
    ifconfig_init();
    net_stack_init();
#endif
    /*---------------------------------------------------------------------------/
    /                               GUI初始化
    /---------------------------------------------------------------------------*/
#if EVSE_USING_GUI
    WM_SetCreateFlags(WM_CF_MEMDEV);    /* Activate the use of memory device feature */
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到
#endif
    printf_safe("\nsystem initialized\n\r");
#if BOOTLOADER
    printf_safe("\nhello bootldr\n\r");
#else
    printf_safe("\nhello charger\n\r");
#endif
}
