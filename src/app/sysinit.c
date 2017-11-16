#include "includes.h"
#include "yaffsfs.h"
#include "cJSON.h"
#include <time.h>
#include "stringName.h"
#include "factorycfg.h"
#include "cfg_sys.h"

#if configAPPLICATION_ALLOCATED_HEAP == 1
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((at(0XC0B00000)));//used by heap_4.c
uint8_t *ucHeap = (uint8_t *)(0XC0B00000);//used by heap_4.c
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

static uint8_t create_system_dir(void)
{
    int res;
    
    yaffs_mkdir(pathSystemDir, 0);
    res = yaffsfs_GetLastError();
    switch(res)
    {
    case 0:
    case -EEXIST:
        return TRUE;
    default:
        return FALSE;
    }
}

void create_cfg_file(const uint8_t *path, const uint8_t *context)
{
    uint32_t bw;
    int fd;
    int res;
    fd = yaffs_open(path, O_CREAT | O_RDWR, S_IWRITE | S_IREAD);
    res = yaffsfs_GetLastError();
    switch(res)
    {
    case 0:
        bw = yaffs_write(fd, context, strlen(context));
        yaffs_close(fd);
    case -EEXIST:
        yaffs_close(fd);
        break;
    default:
        break;
    }
}
extern void retarget_init(void);
void yaffs_init(void)
{
    int res;
    yaffs_start_up();
    res = yaffs_mount(YAFFS_MOUNT_POINT);
    if (res != 0)
    {
        yaffs_format(YAFFS_MOUNT_POINT, 0, 0, 0);
        yaffs_mount(YAFFS_MOUNT_POINT);
    }
}
void sys_Init(void)
{
    yaffs_init();
    //ifconfig_init();
    timeInit();
    retarget_init();
    /*---------------------------------------------------------------------------/
    /                               系统参数初始化
    /---------------------------------------------------------------------------*/
    create_system_dir();
    yaffs_unlink(pathEVSECfg);
    create_cfg_file(pathEVSECfg, strEVSECfg);
    yaffs_unlink(pathProtoCfg);
    yaffs_unlink(pathWhiteList);
    yaffs_unlink(pathBlackList);
    create_cfg_file(pathProtoCfg, strProtoCfg);
    create_cfg_file(pathWhiteList, strWhiteListCfg);
    create_cfg_file(pathBlackList, strBlackListCfg);
    yaffs_unlink(pathEVSELog);
    yaffs_unlink(pathOrder);
    create_cfg_file(pathOrder, strOrderCfg);
    create_cfg_file(pathEVSELog, strLogCfg);
    yaffs_unlink(pathSysCfg);
    yaffs_unlink(pathFTPCfg);
    create_cfg_file(pathSysCfg, strSysCfg);
    create_cfg_file(pathFTPCfg, strFtpCfg);

    SysCfgInit(&xSysconf);
    xSysconf.GetSysCfg((void *)&xSysconf, NULL);
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
}
