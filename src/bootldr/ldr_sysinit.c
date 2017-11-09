#include "includes.h"
#include "ff.h"
#include "nand_diskio.h"
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

FATFS NANDDISKFatFs;  /* File system object for Nand disk logical drive */
char NANDDISKPath[10] = "0:/"; /* Nand disk logical drive path */

extern time_t time_dat;
static void fs_mkfs(void)
{
    BYTE work[4096]; /* Work area (larger is better for processing time) */

    /*##-3- Create a FAT file system (format) on the logical drive #########*/
    if(f_mkfs((TCHAR const *)NANDDISKPath, FM_FAT, 0, work, sizeof(work)) != FR_OK)
    {
        /* FatFs Format Error */
        bsp_Error_Handler();
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
//    res = f_open(&f, path, FA_CREATE_ALWAYS | FA_WRITE);
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
void fs_init(void)
{
    FRESULT res;
    //fs_mkfs();
    res = f_mount(&NANDDISKFatFs, NANDDISKPath, 1);
    if (res != FR_OK)
    {
        fs_mkfs();
        f_mount(&NANDDISKFatFs, NANDDISKPath, 1);
    }
}
void sys_Init(void)
{
    fs_init();
    timeInit();
    retarget_init();
    /*---------------------------------------------------------------------------/
    /                               系统参数初始化
    /---------------------------------------------------------------------------*/
//    create_system_dir();
//
//    create_cfg_file(pathSysCfg, strSysCfg);

    SysCfgInit(&xSysconf);
    xSysconf.GetSysCfg((void *)&xSysconf, NULL);
    /*---------------------------------------------------------------------------/
    /                               GUI初始化
    /---------------------------------------------------------------------------*/

    xprintf("\nsystem initialized\n\r");
    xprintf("\nhello bootldr\n\r");
}
