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
uint8_t create_sysconf_file()
{
    FRESULT res;
    FIL f;
    uint8_t *p;
    UINT bw;

    s2j_create_json_obj(Sysconf_j);
    s2j_json_set_struct_element(subCalibrate_j, Sysconf_j, subCalibrate_t, &xSysconf, Calibrate_t , xCalibrate);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_top);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_bottom);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_left);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_right);

    p = cJSON_Print(Sysconf_j);
    s2j_delete_json_obj(Sysconf_j);
    res = f_open(&f, pathSysconf, FA_CREATE_NEW | FA_WRITE);
    switch(res)
    {
    case FR_OK:
        f_write(&f, p, strlen(p), &bw);
        free(p);
        f_close(&f);
        return TRUE;
    case FR_EXIST:
    default:
        free(p);
        f_close(&f);
        return FALSE;
    }
}
void create_evsecfg_file(void)
{
    FIL f;
    UINT bw;
    FRESULT res;
    res = f_open(&f, pathEVSECfg, FA_CREATE_NEW | FA_WRITE);
    switch(res)
    {
    case FR_OK:
        f_write(&f, strEVSECfg, strlen(strEVSECfg), &bw);
        f_close(&f);
        return TRUE;
    case FR_EXIST:
    default:
        f_close(&f);
        return FALSE;
    }
}
extern void retarget_init(void);
void sys_Init(void)
{
    timeInit();
    retarget_init();
#if configAPPLICATION_ALLOCATED_HEAP == 0
    my_mem_init(SRAMIN);            //初始化内部内存池
    my_mem_init(SRAMEX);            //初始化外部内存池
    my_mem_init(SRAMCCM);           //初始化CCM内存池
#endif

    /*---------------------------------------------------------------------------/
    /                               FATFS初始化
    /---------------------------------------------------------------------------*/
    /*##-1- Link the NAND disk I/O driver #######################################*/
    if(FATFS_LinkDriver(&NANDDISK_Driver, NANDDISKPath) == 0)
    {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 1) != FR_OK)
        {
            fatfs_format();
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
    create_sysconf_file();
    create_evsecfg_file();

    /*---------------------------------------------------------------------------/
    /                               GUI初始化
    /---------------------------------------------------------------------------*/
    WM_SetCreateFlags(WM_CF_MEMDEV);    /* Activate the use of memory device feature */
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到

    xprintf("\nsystem initialized\n\r");
    xprintf("\nhello charger\n\r");
}
