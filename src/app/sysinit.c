#include "includes.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"
#include "cJSON.h"
#include "s2j.h"
#include <time.h>
#include "stringName.h"
#include "factorycfg.h"


#if configAPPLICATION_ALLOCATED_HEAP == 1
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((at(0XC0B00000)));//used by heap_4.c
uint8_t *ucHeap = (uint8_t *)(0XC0B00000);//used by heap_4.c
#endif

Sysconf_t   xSysconf;//存放系统初始化参数

FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
char NANDDISKPath[4]; /* RAM disk logical drive path */

extern time_t time_dat;
extern void Error_Handler(void);
static void fatfs_format(void)
{
    BYTE work[_MAX_SS]; /* Work area (larger is better for processing time) */

    /*##-3- Create a FAT file system (format) on the logical drive #########*/
    if(f_mkfs((TCHAR const *)NANDDISKPath, FM_FAT32, 0, work, sizeof(work)) != FR_OK)
    {
        /* FatFs Format Error */
        Error_Handler();
    }

}

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
    FRESULT res;
    res = f_mkdir("system");
    switch(res)
    {
    case FR_OK:
    case FR_EXIST:
        return TRUE;
    default:
        return FALSE;
    }
}

void create_cfg_file(const uint8_t *path, const uint8_t *context)
{
    FIL f;
    UINT bw;
    FRESULT res;
    res = f_open(&f, path, FA_CREATE_NEW | FA_WRITE);
    switch(res)
    {
    case FR_OK:
        f_write(&f, context, strlen(context), &bw);
        f_close(&f);
    case FR_EXIST:
    default:
        f_close(&f);
    }
}
extern void retarget_init(void);
void sys_Init(void)
{
    //ifconfig_init();
    timeInit();
    retarget_init();
    /*---------------------------------------------------------------------------/
    /                               FATFS初始化
    /---------------------------------------------------------------------------*/
    /*##-1- Link the NAND disk I/O driver #######################################*/
    if(FATFS_LinkDriver(&NANDDISK_Driver, NANDDISKPath) == 0)
    {
        //fatfs_format();
        /*##-2- Register the file system object to the FatFs module ##############*/
        //DISABLE_INT();
        if(f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 1) != FR_OK)
        {
            fatfs_format();
            f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 1);
            /* FatFs Initialization Error */
            //Error_Handler();
        }
    }

    /*---------------------------------------------------------------------------/
    /                               系统参数初始化
    /---------------------------------------------------------------------------*/
    xSysconf.xCalibrate.ad_top = 270;
    xSysconf.xCalibrate.ad_bottom = 3865;
    xSysconf.xCalibrate.ad_left = 100;
    xSysconf.xCalibrate.ad_right  = 3964;
    create_system_dir();
    create_cfg_file(pathEVSECfg, strEVSECfg);
    create_cfg_file(pathProtoCfg, strProtoCfg);
    create_cfg_file(pathWhiteList, strWhiteListCfg);
    create_cfg_file(pathBlackList, strBlackListCfg);

    /*---------------------------------------------------------------------------/
    /                               GUI初始化
    /---------------------------------------------------------------------------*/
    WM_SetCreateFlags(WM_CF_MEMDEV);    /* Activate the use of memory device feature */
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到

    xprintf("\nsystem initialized\n\r");
    xprintf("\nhello charger\n\r");
}
