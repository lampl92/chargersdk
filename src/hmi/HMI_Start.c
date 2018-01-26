#include "GUI.h"
#include "siffontcreate.h"
#include "display.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "interface.h"
#include "stringName.h"

static TaskHandle_t xHandleTaskReadPic = NULL;

I32 memoryfree;
uint16_t calebrate_done = 0;
uint8_t AdvertisementRecordFlag = 0;//标志                                                                             现在是否处于广告页，1为在广告页，0为不在广告页
uint8_t winInitDone = 0;
uint8_t current_page = 0;
GUI_HMEM    qr_hmem;
WM_HWIN cur_win;//记录当前界面




//int QR_Width;//NUmber of "Moudle"
//int QR_Size;//Size of Bitmap in pixels

GUI_QR_INFO QR_info;

static void vTaskReadPic(void *pvParameters)
{ 
    vTaskDelete(xTaskGetCurrentTaskHandle());
}


void MainTask1(void)
{
    GUI_MEMDEV_Handle hMempic;
//    Bitmapcardinfoback = readDtafile(pathcardinfoback);
//    GUI_MEMDEV_Handle hMemBMP;
//    Bitmapcardinfoback = readDtafile(pathcardinfoback);
//    WM_MULTIBUF_Enable(1);
//    hMemBMP = GUI_MEMDEV_CreateEx(0, 0, Bitmapcardinfoback.XSize, Bitmapcardinfoback.YSize, GUI_MEMDEV_HASTRANS);
//    GUI_MEMDEV_Select(hMemBMP);
//    GUI_DrawBitmap(&Bitmapcardinfoback, 0, 0);
//    GUI_MEMDEV_Select(0);
//    GUI_MEMDEV_WriteAt(hMemBMP, 0, 0); 
    GUI_SetFont(&GUI_Font16_1);
    GUI_DispStringAt("Encoding JPEG......", 0, 0);
	
    /* 绘制桌面窗口的背景图片 ------------------------------------------*/
    hMempic = createMemdev(pathcardinfoback);
//    GUI_MEMDEV_Select(hMempic);
//    GUI_DrawBitmap(&Bitmapcardinfoback, 0, 0);
//    GUI_MEMDEV_Select(0);
//    GUI_MEMDEV_WriteAt(hMempic, 0, 0);
    while (1)
    {
        //GUI_MEMDEV_CopyToLCDAt(hMempic, 0, 0);
        GUI_MEMDEV_WriteAt(hMempic, 0, 0);
        GUI_Delay(20);
        vTaskDelay(100);
    }
}


void MainTask(void)
{
    CON_t *pCON;
    if (calebrate_done == 0)
    {
        GUI_CURSOR_Show();
        GUI_Touch_Calibrate();
        calebrate_done = 1;
    }

    if (calebrate_done != 0xff)
    {
        WM_MULTIBUF_Enable(1);
        pCON = CONGetHandle(0);/** @todo (zshare#1#): 双枪时修改ID */
        qr_hmem = GUI_QR_Create(pCON->info.strQRCode, 7, GUI_QR_ECLEVEL_L, 0);
        GUI_QR_GetInfo(qr_hmem, &QR_info);
        createGUI_BITMAP();
        creatememdev();
        createfont();
//        memoryfree = GUI_ALLOC_GetNumUsedBlocks();
//        memoryfree = GUI_ALLOC_GetNumFreeBytes();
        WM_SetDesktopColor(GUI_WHITE);//设置背景颜色
        GUI_Exec();
        GUI_UC_SetEncodeUTF8();

//        WM_HideWindow(_hWinAdvertizement);
//        WM_ShowWindow(cur_win);
//        CreateKeyBoardWindow();
        CreateHomeDLG();                
    }
    else
    {
        calebrate_done = 1;
        WM_MULTIBUF_Enable(1);
        pCON = CONGetHandle(0);/** @todo (zshare#1#): 双枪时修改ID */
        WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

        GUI_UC_SetEncodeUTF8();

        CreateHomePage();
    }
    xTaskCreate(vTaskReadPic, "TaskReadPic", 1024, NULL, 2, &xHandleTaskReadPic);
    while (1)
    {
//	    printf_safe("exec start = %d\n", clock());
        GUI_Exec();
//	    printf_safe("exec end = %d\n", clock());
//	    dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        vTaskDelay(100);
    }
}
