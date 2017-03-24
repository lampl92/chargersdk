#include "includes.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"
#include "cJSON.h"
#include "s2j.h"
#include <time.h>

#if configAPPLICATION_ALLOCATED_HEAP == 1
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((at(0XC0B00000)));//used by heap_4.c
uint8_t *ucHeap = (uint8_t *)(0XC0B00000);//used by heap_4.c
#endif

Sysconf_t   xSysconf;//���ϵͳ��ʼ������

FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
char NANDDISKPath[4]; /* RAM disk logical drive path */

extern time_t time_dat;

static void Error_Handler()
{
    while(1);
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
    orig.tm_mon = RTC_DateStruct.Month-1;
    orig.tm_year = 2000 + RTC_DateStruct.Year - 1900;
//    orig.tm_wday = 5;
//    orig.tm_yday = 19;
    orig.tm_isdst = -1;
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
    FIL fil;
    uint8_t *p;
    uint32_t bw;

    s2j_create_json_obj(Sysconf_j);
    s2j_json_set_struct_element(subCalibrate_j, Sysconf_j, subCalibrate_t, &xSysconf, Calibrate_t , xCalibrate);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_top);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_bottom);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_left);
    s2j_json_set_basic_element(subCalibrate_j, subCalibrate_t, int, ad_right);

    p = cJSON_Print(Sysconf_j);
    s2j_delete_json_obj(Sysconf_j);
    res = f_open(&fil, "system/sysconf.cfg", FA_CREATE_NEW | FA_WRITE);
    switch(res)
    {
    case FR_OK:
        f_write(&fil, p, strlen(p), (void *)&bw);
        free(p);
        f_close(&fil);
        return TRUE;
    case FR_EXIST:
    default:
        free(p);
        f_close(&fil);
        return FALSE;
    }
}

extern void retarget_init(void);
void sys_Init(void)
{
    timeInit();
    retarget_init();
#if configAPPLICATION_ALLOCATED_HEAP == 0
    my_mem_init(SRAMIN);            //��ʼ���ڲ��ڴ��
    my_mem_init(SRAMEX);            //��ʼ���ⲿ�ڴ��
    my_mem_init(SRAMCCM);           //��ʼ��CCM�ڴ��
#endif

    /*---------------------------------------------------------------------------/
    /                               FATFS��ʼ��
    /---------------------------------------------------------------------------*/
    /*##-1- Link the NAND disk I/O driver #######################################*/
    if(FATFS_LinkDriver(&NANDDISK_Driver, NANDDISKPath) == 0)
    {
        /*##-2- Register the file system object to the FatFs module ##############*/
        if(f_mount(&NANDDISKFatFs, (TCHAR const *)NANDDISKPath, 1) != FR_OK)
        {
            /* FatFs Initialization Error */
            Error_Handler();
        }
    }

    /*---------------------------------------------------------------------------/
    /                               ϵͳ������ʼ��
    /---------------------------------------------------------------------------*/
    xSysconf.xCalibrate.ad_top = 270;
    xSysconf.xCalibrate.ad_bottom = 3865;
    xSysconf.xCalibrate.ad_left = 100;
    xSysconf.xCalibrate.ad_right  = 3964;
    create_system_dir();
    create_sysconf_file();

    /*---------------------------------------------------------------------------/
    /                               GUI��ʼ��
    /---------------------------------------------------------------------------*/
    WM_SetCreateFlags(WM_CF_MEMDEV);    /* Activate the use of memory device feature */
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //����STemWin�໺��,RGB�����õ�

    xprintf("\nsystem initialized\n\r");
    xprintf("\nhello charger\n\r");
}
