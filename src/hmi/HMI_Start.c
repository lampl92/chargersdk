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

#pragma region MyRegion
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
p_inf *DtaFileBeijing;
p_inf *DtaFileCheckboxDisable;
GUI_BITMAP BitmapCheckboxChosen;
GUI_BITMAP BitmapCheckboxNotChosen;
GUI_BITMAP BitmapCheckboxDisable;
GUI_BITMAP BitmapBeijing;

GUI_LOGPALETTE Palette;			  
#pragma endregion

int SignalIntensity;
int PreSignalIntensity;

//int QR_Width;//NUmber of "Moudle"
//int QR_Size;//Size of Bitmap in pixels

GUI_QR_INFO QR_info;
#pragma region MyRegion
//公用图片“退出”
GUI_BITMAP BitmapQuit;
GUI_BITMAP BitmapQuitPress;
GUI_BITMAP BitmapbackQuit;
GUI_BITMAP BitmapbackQuitPress;

//选择页图片
GUI_BITMAP BitmapSelectGunBack;
GUI_BITMAP BitmapSelectGunAbottonNotpress;
GUI_BITMAP BitmapSelectGunBbottonNotpress;
GUI_BITMAP BitmapSelectGunAbottonPress;
GUI_BITMAP BitmapSelectGunBbottonPress;

//选择充电模式页图片
GUI_BITMAP Bitmapselectpatternback;
GUI_BITMAP Bitmapselectpatternelectricnumber;
GUI_BITMAP Bitmapselectpatternfull;
GUI_BITMAP Bitmapselectpatternmoneynumber;
GUI_BITMAP Bitmapselectpatterntime;

GUI_BITMAP Bitmapselectpatternelectricnumberpress;
GUI_BITMAP Bitmapselectpatternfullpress;
GUI_BITMAP Bitmapselectpatternmoneynumberpress;
GUI_BITMAP Bitmapselectpatterntimepress;

GUI_BITMAP Bitmapselectpatternunityuan;
GUI_BITMAP Bitmapselectpatternunitdu;
GUI_BITMAP Bitmapselectpatternunitfen;
GUI_BITMAP Bitmapselectpatternunitno;

GUI_BITMAP BitmapKeyboardback;
GUI_BITMAP BitmapKeyboard0;
GUI_BITMAP BitmapKeyboard1;
GUI_BITMAP BitmapKeyboard2;
GUI_BITMAP BitmapKeyboard3;
GUI_BITMAP BitmapKeyboard4;
GUI_BITMAP BitmapKeyboard5;
GUI_BITMAP BitmapKeyboard6;
GUI_BITMAP BitmapKeyboard7;
GUI_BITMAP BitmapKeyboard8;
GUI_BITMAP BitmapKeyboard9;
GUI_BITMAP BitmapKeyboardBackspace;
GUI_BITMAP BitmapKeyboardesc;
GUI_BITMAP BitmapKeyboardok;
GUI_BITMAP BitmapKeyboardpoint;

GUI_BITMAP BitmapKeyboardbackpress;
GUI_BITMAP BitmapKeyboard0press;
GUI_BITMAP BitmapKeyboard1press;
GUI_BITMAP BitmapKeyboard2press;
GUI_BITMAP BitmapKeyboard3press;
GUI_BITMAP BitmapKeyboard4press;
GUI_BITMAP BitmapKeyboard5press;
GUI_BITMAP BitmapKeyboard6press;
GUI_BITMAP BitmapKeyboard7press;
GUI_BITMAP BitmapKeyboard8press;
GUI_BITMAP BitmapKeyboard9press;
GUI_BITMAP BitmapKeyboardBackspacepress;
GUI_BITMAP BitmapKeyboardescpress;
GUI_BITMAP BitmapKeyboardokpress;
GUI_BITMAP BitmapKeyboardpointpress;

//卡信息页图片
GUI_BITMAP Bitmapcardinfoarrears;
GUI_BITMAP Bitmapcardinfoback;
GUI_BITMAP Bitmapcardinfoempty;
GUI_BITMAP Bitmapcardinfoget;
GUI_BITMAP Bitmapcardinfoplug;
GUI_BITMAP Bitmapcardinfounregister;

//主页图片
GUI_BITMAP Bitmaphomeback;
GUI_BITMAP BitmaphomegunAchargedone;
GUI_BITMAP BitmaphomegunAcharging;
GUI_BITMAP BitmaphomegunAerror;
GUI_BITMAP BitmaphomegunAfree;
GUI_BITMAP BitmaphomegunBchargedone;
GUI_BITMAP BitmaphomegunBcharging;
GUI_BITMAP BitmaphomegunBerror;
GUI_BITMAP BitmaphomegunBfree;
GUI_BITMAP Bitmaphomegunlookinfo;
GUI_BITMAP Bitmaphomegunscancode;
GUI_BITMAP Bitmaphomegunlookinfopress;
GUI_BITMAP Bitmaphomegunscancodepress;
GUI_BITMAP Bitmaphomesignal0;
GUI_BITMAP Bitmaphomesignal1;
GUI_BITMAP Bitmaphomesignal2;
GUI_BITMAP Bitmaphomesignal3;

//提示信息页图片
GUI_BITMAP Bitmapchargedoneinfo;
GUI_BITMAP Bitmapcharginginfo;				  
#pragma endregion

//主页储存设备
GUI_MEMDEV_Handle Memdevhomeback;
GUI_MEMDEV_Handle MemdevhomegunAchargedone;
GUI_MEMDEV_Handle MemdevhomegunAcharging;
GUI_MEMDEV_Handle MemdevhomegunAerror;
GUI_MEMDEV_Handle MemdevhomegunAfree;
GUI_MEMDEV_Handle MemdevhomegunBchargedone;
GUI_MEMDEV_Handle MemdevhomegunBcharging;
GUI_MEMDEV_Handle MemdevhomegunBerror;
GUI_MEMDEV_Handle MemdevhomegunBfree;
GUI_MEMDEV_Handle Memdevhomegunlookinfo;
GUI_MEMDEV_Handle Memdevhomegunscancode;
GUI_MEMDEV_Handle Memdevhomegunlookinfopress;
GUI_MEMDEV_Handle Memdevhomegunscancodepress;
GUI_MEMDEV_Handle Memdevhomesignal0;
GUI_MEMDEV_Handle Memdevhomesignal1;
GUI_MEMDEV_Handle Memdevhomesignal2;
GUI_MEMDEV_Handle Memdevhomesignal3;

GUI_MEASDEV_Handle MemdevSelectGunBack;


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
#pragma region MyRegion
        DtaFileCheckboxChosen = readPicInf(pathCheckboxDta);
        DtaFileCheckboxNotChosen = readPicInf(pathCheckboxNotDta);
        DtaFileCheckboxDisable = readPicInf(pathCheckboxDisable);
        DtaFileBeijing = readPicInf(pathBeijingDta);
        GUI_CreateBitmapFromStream565(&BitmapCheckboxChosen, &Palette, DtaFileCheckboxChosen->pfilestring);
        GUI_CreateBitmapFromStream565(&BitmapCheckboxNotChosen, &Palette, DtaFileCheckboxNotChosen->pfilestring);
        GUI_CreateBitmapFromStream565(&BitmapCheckboxDisable, &Palette, DtaFileCheckboxDisable->pfilestring);
        GUI_CreateBitmapFromStream565(&BitmapBeijing, &Palette, DtaFileBeijing->pfilestring);
        //公用图片读取
        BitmapQuit = readDtafile(pathQuit);
        BitmapQuitPress = readDtafile(pathQuitPress);
        BitmapbackQuit = readDtafile(pathbackquit);
        BitmapbackQuitPress = readDtafile(pathbackquitpress);
        //选择页图片读取
        BitmapSelectGunAbottonNotpress = readDtafile(pathSelectGunAbottonNotpress);
        BitmapSelectGunAbottonPress = readDtafile(pathSelectGunAbottonPress);
        BitmapSelectGunBack = readDtafile(pathSelectGunBack);
        BitmapSelectGunBbottonNotpress = readDtafile(pathSelectGunBbottonNotpress);
        BitmapSelectGunBbottonPress = readDtafile(pathSelectGunBbottonPress);
        
        //选择充电模式页图片读取
        Bitmapselectpatternback = readDtafile(pathselectpatternback);
        Bitmapselectpatternelectricnumber = readDtafile(pathselectpatternelectricnumber);
        Bitmapselectpatternfull = readDtafile(pathselectpatternfull);
        Bitmapselectpatternmoneynumber = readDtafile(pathselectpatternmoneynumber);
        Bitmapselectpatterntime = readDtafile(pathselectpatterntime);

        Bitmapselectpatternelectricnumberpress = readDtafile(pathselectpatternelectricnumberpress);
        Bitmapselectpatternfullpress = readDtafile(pathselectpatternfullpress);
        Bitmapselectpatternmoneynumberpress = readDtafile(pathselectpatternmoneynumberpress);
        Bitmapselectpatterntimepress = readDtafile(pathselectpatterntimepress);

        Bitmapselectpatternunityuan = readDtafile(pathselectpatternunityuan);
        Bitmapselectpatternunitdu = readDtafile(pathselectpatternunitdu);
        Bitmapselectpatternunitfen = readDtafile(pathselectpatternunitfen);
        Bitmapselectpatternunitno = readDtafile(pathselectpatternunitno);
        
        BitmapKeyboardback = readDtafile(pathKeyboardback);
        
        BitmapKeyboard0 = readDtafile(pathKeyboard0);
        BitmapKeyboard1 = readDtafile(pathKeyboard1);
        BitmapKeyboard2 = readDtafile(pathKeyboard2);
        BitmapKeyboard3 = readDtafile(pathKeyboard3);
        BitmapKeyboard4 = readDtafile(pathKeyboard4);
        BitmapKeyboard5 = readDtafile(pathKeyboard5);
        BitmapKeyboard6 = readDtafile(pathKeyboard6);
        BitmapKeyboard7 = readDtafile(pathKeyboard7);
        BitmapKeyboard8 = readDtafile(pathKeyboard8);
        BitmapKeyboard9 = readDtafile(pathKeyboard9);
        BitmapKeyboardBackspace = readDtafile(pathKeyboardBackspace);
        BitmapKeyboardesc = readDtafile(pathKeyboardesc);
        BitmapKeyboardok = readDtafile(pathKeyboardok);
        BitmapKeyboardpoint = readDtafile(pathKeyboardpoint);

        BitmapKeyboard0press = readDtafile(pathKeyboard0press);
        BitmapKeyboard1press = readDtafile(pathKeyboard1press);
        BitmapKeyboard2press = readDtafile(pathKeyboard2press);
        BitmapKeyboard3press = readDtafile(pathKeyboard3press);
        BitmapKeyboard4press = readDtafile(pathKeyboard4press);
        BitmapKeyboard5press = readDtafile(pathKeyboard5press);
        BitmapKeyboard6press = readDtafile(pathKeyboard6press);
        BitmapKeyboard7press = readDtafile(pathKeyboard7press);
        BitmapKeyboard8press = readDtafile(pathKeyboard8press);
        BitmapKeyboard9press = readDtafile(pathKeyboard9press);
        BitmapKeyboardBackspacepress = readDtafile(pathKeyboardBackspacepress);
        BitmapKeyboardescpress = readDtafile(pathKeyboardescpress);
        BitmapKeyboardokpress = readDtafile(pathKeyboardokpress);
        BitmapKeyboardpointpress = readDtafile(pathKeyboardpointpress);
        
        //卡信息页图片
        Bitmapcardinfoarrears = readDtafile(pathcardinfoarrears);
        Bitmapcardinfoback = readDtafile(pathcardinfoback);
        Bitmapcardinfoempty = readDtafile(pathcardinfoempty);
        Bitmapcardinfoget = readDtafile(pathcardinfoget);
        Bitmapcardinfoplug = readDtafile(pathcardinfoplug);
        Bitmapcardinfounregister = readDtafile(pathcardinfounregister);
        
        //主页图片读取
        Bitmaphomeback = readDtafile(pathhomeback);
        BitmaphomegunAchargedone = readDtafile(pathhomegunAchargedone);
        BitmaphomegunAcharging = readDtafile(pathhomegunAcharging);
        BitmaphomegunAerror = readDtafile(pathhomegunAerror);
        BitmaphomegunAfree = readDtafile(pathhomegunAfree);
        BitmaphomegunBchargedone = readDtafile(pathhomegunBchargedone);
        BitmaphomegunBcharging = readDtafile(pathhomegunBcharging);
        BitmaphomegunBerror = readDtafile(pathhomegunBerror);
        BitmaphomegunBfree = readDtafile(pathhomegunBfree);
        Bitmaphomegunlookinfo = readDtafile(pathhomegunlookinfo);
        Bitmaphomegunscancode = readDtafile(pathhomegunscancode);
        Bitmaphomegunlookinfopress = readDtafile(pathhomegunlookinfopress);
        Bitmaphomegunscancodepress = readDtafile(pathhomegunscancodepress);   
        Bitmaphomesignal0 = readDtafile(pathhomesignal0);
        Bitmaphomesignal1 = readDtafile(pathhomesignal1);
        Bitmaphomesignal2 = readDtafile(pathhomesignal2);
        Bitmaphomesignal3 = readDtafile(pathhomesignal3);
        
        Bitmapchargedoneinfo = readDtafile(pathchargedoneinfo);
        Bitmapcharginginfo = readDtafile(pathcharginginfo);				  
#pragma endregion

        Memdevhomeback = createMemdev(pathhomeback);
        MemdevhomegunAchargedone = createMemdev(pathhomegunAchargedone);
        MemdevhomegunAcharging = createMemdev(pathhomegunAcharging);
        MemdevhomegunAerror = createMemdev(pathhomegunAerror);
        MemdevhomegunAfree = createMemdev(pathhomegunAfree);
        MemdevhomegunBchargedone = createMemdev(pathhomegunBchargedone);
        MemdevhomegunBcharging = createMemdev(pathhomegunBcharging);
        MemdevhomegunBerror = createMemdev(pathhomegunBerror);
        MemdevhomegunBfree = createMemdev(pathhomegunBfree);
        Memdevhomegunlookinfo = createMemdev(pathhomegunlookinfo);
        Memdevhomegunscancode = createMemdev(pathhomegunscancode);
        Memdevhomegunlookinfopress = createMemdev(pathhomegunlookinfopress);
        Memdevhomegunscancodepress = createMemdev(pathhomegunscancodepress);
        Memdevhomesignal0 = createMemdev(pathhomesignal0);
        Memdevhomesignal1 = createMemdev(pathhomesignal1);
        Memdevhomesignal2 = createMemdev(pathhomesignal2);
        Memdevhomesignal3 = createMemdev(pathhomesignal3);
        
        MemdevSelectGunBack = createMemdev(pathSelectGunBack);
        
        
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
 //       CreateKeyBoardWindow();
        CreateHomeDLG();
        //CreateselectgunDLG();                   
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
