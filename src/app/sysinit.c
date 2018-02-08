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

uint8_t create_system_dir(void)
{
    int res = 1;
    res = yaffs_mkdir(pathSystemDir, S_IREAD | S_IWRITE);
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
    //yaffs_format(YAFFS_MOUNT_POINT, 0, 0, 0);
    res = yaffs_mount(YAFFS_MOUNT_POINT);
    if (res != 0)
    {
        yaffs_format(YAFFS_MOUNT_POINT, 0, 0, 0);
        yaffs_mount(YAFFS_MOUNT_POINT);
    }
}
void sys_Init(void)
{
    int res;
    evse_ver ver_from_cfg;
    //ifconfig_init();
    timeInit();
    retarget_init();
    yaffs_init();
#if 1
    /*---------------------------------------------------------------------------/
    /                               系统参数初始化
    /---------------------------------------------------------------------------*/
    create_system_dir();
    create_cfg_file(pathEVSECfg, strEVSECfg);
    create_cfg_file(pathProtoCfg, strProtoCfg);
    create_cfg_file(pathWhiteList, strWhiteListCfg);
    create_cfg_file(pathBlackList, strBlackListCfg);
    create_cfg_file(pathOrder, strOrderCfg);
    create_cfg_file(pathEVSELog, strLogCfg);
    create_cfg_file(pathSysCfg, strSysCfg);
    create_cfg_file(pathFTPCfg, strFtpCfg);
    create_cfg_file(pathNetCfg, strNetCfg);
    dump_directory_tree(YAFFS_MOUNT_POINT);

    SysCfgInit(&xSysconf);
    xSysconf.GetSysCfg((void *)&xSysconf, NULL);
    sscanf(xSysconf.strVersion, "%d.%d.%d", &(ver_from_cfg.major), &(ver_from_cfg.minor), &(ver_from_cfg.patch));
    if (ver_from_cfg.major != EVSE_MAJOR || ver_from_cfg.minor != EVSE_MINOR || ver_from_cfg.patch != EVSE_PATCH)
    {
        sprintf(xSysconf.strVersion, "%02d.%02d.%04d", EVSE_MAJOR, EVSE_MINOR, EVSE_PATCH);
        xSysconf.SetSysCfg(jnSysVersion, xSysconf.strVersion, ParamTypeString);
    }
    
    /*---------------------------------------------------------------------------/
    /                               NET初始化
    /---------------------------------------------------------------------------*/
    ifconfig_init();
    net_stack_init();
    
    /*---------------------------------------------------------------------------/
    /                               GUI初始化
    /---------------------------------------------------------------------------*/
#if EVSE_USING_GUI
    WM_SetCreateFlags(WM_CF_MEMDEV);    /* Activate the use of memory device feature */
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到
#endif
    xprintf("\nsystem initialized\n\r");
    xprintf("\nhello charger\n\r");
#endif
}
