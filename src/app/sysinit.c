#include "includes.h"
#include "ff_gen_drv.h"
#include "nand_diskio.h"
#include "mxml.h"

#if configAPPLICATION_ALLOCATED_HEAP == 1
//uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ((at(0XC0B00000)));//used by heap_4.c
uint8_t *ucHeap = (uint8_t *)(0XC0B00000);//used by heap_4.c
#endif

sysconf_t   sysconf_param;//存放系统初始化参数

FATFS NANDDISKFatFs;  /* File system object for RAM disk logical drive */
char NANDDISKPath[4]; /* RAM disk logical drive path */

static void Error_Handler()
{
    while(1);
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
uint8_t create_sysconf_xml()
{
    FRESULT res;
    FIL fil;
    mxml_node_t *sysconf_tree, *sysconf_node, *tp_calibrate_node, *data_node;
    res = f_open(&fil, "system/sysconf.xml", FA_CREATE_NEW | FA_WRITE);
    switch(res)
    {
        case FR_OK:
            sysconf_tree = mxmlNewXML("1.0");
            sysconf_node = mxmlNewElement(sysconf_tree, "sysconf");
            tp_calibrate_node = mxmlNewElement(sysconf_node, "tp_calibrate");
            data_node = mxmlNewElement(tp_calibrate_node, "AD_TOP");
            mxmlNewInteger(data_node, 270);
            data_node = mxmlNewElement(tp_calibrate_node, "AD_BOTTOM");
            mxmlNewInteger(data_node, 3865);
            data_node = mxmlNewElement(tp_calibrate_node, "AD_LEFT");
            mxmlNewInteger(data_node, 100);
            data_node = mxmlNewElement(tp_calibrate_node, "AD_RIGHT");
            mxmlNewInteger(data_node, 3964);
            mxmlSaveFile(sysconf_tree, &fil, whitespace_cb);
            f_close(&fil);
            mxmlDelete(sysconf_tree);
            return TRUE;
        case FR_EXIST:
            return FALSE;
        default:
            return FALSE;
    }
}
uint8_t write_sysconf_xml(void)
{
    FRESULT res;
    FIL fil;
    mxml_node_t *sysconf_tree, *sysconf_node, *tp_calibrate_node, *data_node;
    res = f_open(&fil, "system/sysconf.xml", FA_OPEN_EXISTING |FA_READ|FA_WRITE);
    switch(res)
    {
        case FR_OK:
            sysconf_tree = mxmlLoadFile(NULL, &fil, MXML_INTEGER_CALLBACK);
            if(sysconf_tree != NULL)
            {
                sysconf_node = mxmlFindElement(sysconf_tree, sysconf_tree, "sysconf", NULL, NULL, MXML_DESCEND);
                if(sysconf_node == NULL)
                {
                    return FALSE;
                }
                tp_calibrate_node = mxmlFindElement(sysconf_node, sysconf_tree, "tp_calibrate", NULL, NULL, MXML_DESCEND);
                if(tp_calibrate_node == NULL)
                {
                    return FALSE;
                }
                data_node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_TOP", NULL, NULL, MXML_DESCEND);
                if(data_node == NULL)
                {
                    return FALSE;
                }
                mxmlSetInteger (data_node, sysconf_param.ad_top);
                data_node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_BOTTOM", NULL, NULL, MXML_DESCEND);
                if(data_node == NULL)
                {
                    return FALSE;
                }
                mxmlSetInteger (data_node, sysconf_param.ad_bottom);
                data_node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_LEFT", NULL, NULL, MXML_DESCEND);
                if(data_node == NULL)
                {
                    return FALSE;
                }
                mxmlSetInteger (data_node, sysconf_param.ad_left);
                data_node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_RIGHT", NULL, NULL, MXML_DESCEND);
                if(data_node == NULL)
                {
                    return FALSE;
                }
                mxmlSetInteger (data_node, sysconf_param.ad_right);
                f_lseek(&fil,0);
                mxmlSaveFile(sysconf_tree, &fil, whitespace_cb);
                f_close(&fil);
                mxmlDelete(sysconf_tree);
                return TRUE;
            }
        default:
            return FALSE;
    }
}

uint8_t read_sysconf_xml(void)
{
    FRESULT res;
    FIL fil;
    mxml_node_t *sysconf_tree, *sysconf_node, *tp_calibrate_node, *node;
    res = f_open(&fil, "system/sysconf.xml", FA_OPEN_EXISTING | FA_READ);
    switch(res)
    {
        case FR_OK:
            sysconf_tree = mxmlLoadFile(NULL, &fil, MXML_INTEGER_CALLBACK);
            if(sysconf_tree != NULL)    //文件打开成功, 且有xml配置内容
            {
                sysconf_node = mxmlFindElement(sysconf_tree, sysconf_tree, "sysconf", NULL, NULL, MXML_DESCEND);
                if(sysconf_node == NULL)
                {
                    return FALSE;
                }
                tp_calibrate_node = mxmlFindElement(sysconf_node, sysconf_tree, "tp_calibrate", NULL, NULL, MXML_DESCEND);
                if(tp_calibrate_node == NULL)
                {
                    return FALSE;
                }
                node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_TOP", NULL, NULL, MXML_DESCEND);
                if(node == NULL)
                {
                    return FALSE;
                }
                sysconf_param.ad_top = mxmlGetInteger(node);
                node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_BOTTOM", NULL, NULL, MXML_DESCEND);
                if(node == NULL)
                {
                    return FALSE;
                }
                sysconf_param.ad_bottom = mxmlGetInteger(node);
                node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_LEFT", NULL, NULL, MXML_DESCEND);
                if(node == NULL)
                {
                    return FALSE;
                }
                sysconf_param.ad_left = mxmlGetInteger(node);
                node = mxmlFindElement(tp_calibrate_node, sysconf_tree, "AD_RIGHT", NULL, NULL, MXML_DESCEND);
                if(node == NULL)
                {
                    return FALSE;
                }
                sysconf_param.ad_right = mxmlGetInteger(node);
                f_close(&fil);
                mxmlDelete(sysconf_tree);
                return TRUE;
            }
        default:
            return FALSE;
    }
}
extern void retarget_init(void);
void sys_Init(void)
{
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
            /* FatFs Initialization Error */
            Error_Handler();
        }
    }

    /*---------------------------------------------------------------------------/
    /                               系统参数初始化
    /---------------------------------------------------------------------------*/
    create_system_dir();
    create_sysconf_xml();
    read_sysconf_xml();

    /*---------------------------------------------------------------------------/
    /                               GUI初始化
    /---------------------------------------------------------------------------*/
    WM_SetCreateFlags(WM_CF_MEMDEV);    /* Activate the use of memory device feature */
    GUI_Init();
    WM_MULTIBUF_Enable(1);  //开启STemWin多缓冲,RGB屏会用到

    xprintf("\nsystem initialized\n\r");
    xprintf("\nhello charger\n\r");
}
