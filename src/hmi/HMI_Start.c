#include "GUI.h"
#include "siffontcreate.h"
#include "bmpdisplay.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "interface.h"
#include "stringName.h"

static TaskHandle_t xHandleTaskReadPic = NULL;

uint16_t calebrate_done = 0;
uint8_t AdvertisementRecordFlag = 0;//标志现在是否处于广告页，1为在广告页，0为不在广告页
uint8_t winInitDone = 0;
uint8_t current_page = 0;
GUI_HMEM    qr_hmem;
WM_HWIN cur_win;//记录当前界面

int SignalIntensity;
int PreSignalIntensity;

//int QR_Width;//NUmber of "Moudle"
//int QR_Size;//Size of Bitmap in pixels

GUI_QR_INFO QR_info;

p_inf *HomeImage;
p_inf *SignalImage0;
p_inf *SignalImage1;
p_inf *SignalImage2;
p_inf *SignalImage3;
p_inf *SignalImage4;
p_inf *SignalImage5;

p_inf *CardInfoImage;
p_inf *GetCardInfoImage;
p_inf *CardUnregisteredImage;
p_inf *CardArrearsImage;
p_inf *PleaseConnectPlugImage;
p_inf *CardInfoVoidImage;

p_inf *ChargingImage;
p_inf *cartoonImage0;
p_inf *cartoonImage1;
p_inf *cartoonImage2;
p_inf *cartoonImage3;
p_inf *cartoonImage4;
p_inf *cartoonImage5;
p_inf *StopByCardImage;
p_inf *StopByQRImage;
p_inf *ChargingVoidImage;

p_inf *ChargeDoneImage;
p_inf *OrderUploadImage;
p_inf *NormalDoneImage;
p_inf *FullDoneImage;
p_inf *DevErrDoneImage;
p_inf *MoneyNotEnoughDoneImage;
p_inf *ChargeDoneVoidImage;

p_inf *AdvertisementImage;

p_inf *DtaFileCheckboxChosen;
p_inf *DtaFileCheckboxNotChosen;
GUI_BITMAP BitmapCheckboxChosen;
GUI_BITMAP BitmapCheckboxNotChosen;
GUI_LOGPALETTE Palette;



static void vTaskReadPic(void *pvParameters)
{
    ChargeDoneImage = readPicInf(pathChargeDoneImage);
    CardInfoImage = readPicInf(pathCardInfoImage);
    ChargingImage = readPicInf(pathChargingImage);
    GetCardInfoImage = readPicInf(pathGetCardInfoImage);
    cartoonImage0 = readPicInf(pathCartoonImage0);
    cartoonImage1 = readPicInf(pathCartoonImage1);
    cartoonImage2 = readPicInf(pathCartoonImage2);
    cartoonImage3 = readPicInf(pathCartoonImage3);
    cartoonImage4 = readPicInf(pathCartoonImage4);
    cartoonImage5 = readPicInf(pathCartoonImage5);
    CardUnregisteredImage = readPicInf(pathCardUnregisteredImage);
    CardArrearsImage = readPicInf(pathCardArrearsImage);
    PleaseConnectPlugImage = readPicInf(pathPleaseConnectPlugImage);
    CardInfoVoidImage = readPicInf(pathCardInfoVoidImage);
    StopByCardImage = readPicInf(pathStopByCardImage);
    StopByQRImage = readPicInf(pathStopByQRImage);
    ChargingVoidImage = readPicInf(pathChargingVoidImage);
    OrderUploadImage = readPicInf(pathOrderUploadImage);
    NormalDoneImage = readPicInf(pathNormalDoneImage);
    FullDoneImage = readPicInf(pathFullDoneImage);
    DevErrDoneImage = readPicInf(pathDevErrDoneImage);
    MoneyNotEnoughDoneImage = readPicInf(pathMoneyNotEnoughDoneImage);
    ChargeDoneVoidImage = readPicInf(pathChargeDoneVoidImage);
    
    vTaskDelete(xTaskGetCurrentTaskHandle());
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

    if (calebrate_done != 0xff)
    {
        WM_MULTIBUF_Enable(1);
        pCON = CONGetHandle(0);/** @todo (zshare#1#): 双枪时修改ID */
        qr_hmem = GUI_QR_Create(pCON->info.strQRCode, 7, GUI_QR_ECLEVEL_L, 0);
        GUI_QR_GetInfo(qr_hmem, &QR_info);


        SignalImage0 = readPicInf(pathSignalImage0);
        SignalImage1 = readPicInf(pathSignalImage1);
        SignalImage2 = readPicInf(pathSignalImage2);
        SignalImage3 = readPicInf(pathSignalImage3);
        SignalImage4 = readPicInf(pathSignalImage4);
        SignalImage5 = readPicInf(pathSignalImage5);

        AdvertisementImage = readPicInf(pathAdvertisementImage);
        HomeImage = readPicInf(pathHomeImage);
        DtaFileCheckboxChosen = readPicInf(pathCheckboxDta);
        DtaFileCheckboxNotChosen = readPicInf(pathCheckboxNotDta);
        GUI_CreateBitmapFromStream565(&BitmapCheckboxChosen, &Palette, DtaFileCheckboxChosen->pfilestring);
        GUI_CreateBitmapFromStream565(&BitmapCheckboxNotChosen, &Palette, DtaFileCheckboxNotChosen->pfilestring);

        Create_SIF12(pathstSIF12);
        Create_SIF16(pathstSIF16);
        Create_SIF24(pathstSIF24);
        Create_SIF36(pathstSIF36);

        WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

        GUI_UC_SetEncodeUTF8();
//        CreateAdvertisementPage();
//        CreateHomePage();
//        WM_HideWindow(_hWinAdvertizement);
//        WM_ShowWindow(cur_win);
        CreateKeyBoardWindow();
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
	    //dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        vTaskDelay(100);
#if 0
        if (bittest(winInitDone, 7))
        {
            bitclr(winInitDone, 7);
            switch (current_page)
            {
            case _HOMEPAGE:
                CreateHome();
                break;
            case _CARDINFOPAGE:
                CreateCardInfoPage();
                break;
            case _CHARGINGPAGE:
                CreateChargingPage();
                break;
            case _CHARGEDONEPAGE:
                CreateChargeDonePage();
                break;
            }
        }
#endif
    }
}
