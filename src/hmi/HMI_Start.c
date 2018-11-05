#include "GUI.h"
#include "siffontcreate.h"
#include "display.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "interface.h"
#include "stringName.h"
#include "file_op.h"
#include "yaffsfs.h"

TaskHandle_t xHandleTaskReadData = NULL;

I32 memoryfree;
uint16_t calebrate_done = 0;
uint8_t AdvertisementRecordFlag = 0;//标志                                                                             现在是否处于广告页，1为在广告页，0为不在广告页
uint8_t winInitDone = 0;
uint8_t current_page = 0;
GUI_HMEM    qr_hmem;
WM_HWIN cur_win;//记录当前界面
WM_HWIN startUpWin;//开机窗口句柄
char QR_saveA[defQRCodeLength];//保存的枪A二维码
char QR_saveB[defQRCodeLength];//保存的枪B二维码

Fun home;
//int QR_Width;//NUmber of "Moudle"
//int QR_Size;//Size of Bitmap in pixels

GUI_QR_INFO QR_info;

int flag_read_data = 0;//开机界面存在与否标志,1表示存在,0表示不存在

static void vTask_read_data(void *pvParameters)
{
    createfont();
    creatememdev();
    if (get_bmp_check_tmp() == 3)//检测文件正常,但是还存在tmp,删除tmp并重启
    {
        yaffs_unlink(pathBmpCheckTmp);
        NVIC_SystemReset();
    }
    flag_read_data = 1;
    vTaskDelete(NULL);
}


void MainTask1(void)
{
    GUI_MEMDEV_Handle hMemBMP;
    uint32_t t0, t1, i, count = 0;
    char buf[50];
    GUI_MEMDEV_Handle hMempic;
    createfont();

//    Bitmapcardinfoback = readDtafile(pathcardinfoback);
//    GUI_MEMDEV_Handle hMemBMP;
//    Bitmapcardinfoback = readDtafile(pathcardinfoback);
//    WM_MULTIBUF_Enable(1);
//    hMemBMP = GUI_MEMDEV_CreateEx(0, 0, Bitmapcardinfoback.XSize, Bitmapcardinfoback.YSize, GUI_MEMDEV_HASTRANS);
//    GUI_MEMDEV_Select(hMemBMP);
//    GUI_DrawBitmap(&Bitmapcardinfoback, 0, 0);
//    GUI_MEMDEV_Select(0);
//    GUI_MEMDEV_WriteAt(hMemBMP, 0, 0); 
//    GUI_SetFont(&GUI_Font16_1);
//    GUI_DispStringAt("Encoding JPEG......", 0, 0);
	
    /* 绘制桌面窗口的背景图片 ------------------------------------------*/
    GUI_UC_SetEncodeUTF8();
    GUI_SetFont(&fontwryhcg36aa4e);
    hMempic = createMemdev(pathcardinfoback);
//    GUI_MEMDEV_Select(hMempic);
//    GUI_DrawBitmap(&Bitmapcardinfoback, 0, 0);
//    GUI_MEMDEV_Select(0);
    for (i = 0; i < 20; i++)
    {
        t0 = GUI_GetTime();
        /* 用到 BMP 图片的时候，调用此函数即可 */
        GUI_MEMDEV_WriteAt(hMempic, 0, 0);
        t1 = GUI_GetTime() - t0;
        //App_Printf("速度 = %dms\r\n", t1);
        count += t1;
    }
    /* 求出刷新 20 次的平均速度 */
    sprintf(buf, "speed = %dms/frame", count / i);
    GUI_DispStringAt(buf, 10, 10);
    GUI_SetTextMode(GUI_TM_TRANS);
    while (1)
    {
        //GUI_MEMDEV_CopyToLCDAt(hMempic, 0, 0);
        GUI_MEMDEV_WriteAt(hMempic, 0, 0);
        GUI_DispStringAt(buf, 200, 220);
       
        GUI_Delay(2000);
        vTaskDelay(100);
    }
}

void MainTask(void)
{
    CON_t *pCON;
    if (calebrate_done == 0)
    {
        GUI_CURSOR_Hide();
        GUI_Touch_Calibrate();
        calebrate_done = 1;
    }
    if (pEVSE->info.ucTotalCON == 1)
    {
        pCON = CONGetHandle(0);
        strncpy(QR_saveA, pCON->info.strQRCode, defQRCodeLength);
        home = CreateHome0DLG;
    }
    else
    {
        pCON = CONGetHandle(0);
        strncpy(QR_saveA, pCON->info.strQRCode, defQRCodeLength);
        pCON = CONGetHandle(1);
        strncpy(QR_saveB, pCON->info.strQRCode, defQRCodeLength);
        home = CreateHomeDLG;    
    } 
    if (calebrate_done != 0xff)
    {
        WM_MULTIBUF_Enable(1);
        GUI_UC_SetEncodeUTF8();
        
        createfont();
        CreateManagerWhiteUser();
        
//        createStartUpMemdev();    
//        startUpWin = CreatestartUpDLG();
//        xTaskCreate(vTask_read_data, "vTask_read_data", 1024 * 20, NULL, 4, &xHandleTaskReadData);
//        if (get_bmp_check_tmp() == 3)
//        {
//            vTaskSuspend(NULL);
//        }
        
        
//        memoryfree = GUI_ALLOC_GetNumUsedBlocks();
//        memoryfree = GUI_ALLOC_GetNumFreeBlocks();
//        memoryfree = GUI_ALLOC_GetNumUsedBytes();
//        memoryfree = GUI_ALLOC_GetNumFreeBytes();
        
        
    }
    while (1)
    {
//	    printf_safe("exec start = %d\n", clock());
//        GUI_Exec();
        GUI_Delay(8000);
//	    printf_safe("exec end = %d\n", clock());
//	    dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        vTaskDelay(100);
    }
}
