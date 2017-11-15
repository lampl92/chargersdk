#include "GUI.h"
#include "xbffontcreate.h"
#include "siffontcreate.h"
#include "ttffontcreate.h"
#include "bmpdisplay.h"
#include "ff.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "interface.h"

uint8_t calebrate_done = 0;
uint8_t winInitDone = 0;
uint8_t current_page = 0;
GUI_HMEM    qr_hmem;

static uint8_t codetest[]={
    "https://www.baidu.com"
};
void MainTask(void)
{
    CON_t *pCON;

    if(calebrate_done == 0)
    {
        GUI_CURSOR_Show();
        GUI_Touch_Calibrate();
        calebrate_done = 1;
    }

    if(calebrate_done != 0xff)
    {
        WM_MULTIBUF_Enable(1);
        pCON = CONGetHandle(0);/** @todo (zshare#1#): 双枪时修改ID */
        qr_hmem = GUI_QR_Create(pCON->info.strQRCode, 6, GUI_QR_ECLEVEL_L, 0);
        readBackGroundNOFREE("system/background_tree.bmp");
//        Create_XBF12("system/XBF_Song_12.xbf");//创建xbf12号路径
//        Create_XBF14("system/XBF_Song_14.xbf");//创建XBF14号路径
//        Create_XBF16("system/XBF_Song_16.xbf");//创建xbf16号路径
//        Create_XBF19("system/XBF_Song_19.xbf");//创建xbf19号路径
//        Create_XBF24("system/XBF_Song_24.xbf");//创建xbf24号路径
//        Create_XBF36("system/XBF_Song_36.xbf");//创建xbf36号路径

//        Create_SIF12("system/SIF12.sif");
//        Create_SIF16("system/SIF16.sif");
//        Create_SIF24("system/SIF24.sif");
//        Create_SIF36("system/SIF36.sif");

//        Create_SIF12("system/lsSIF12.sif");
//        Create_SIF16("system/lsSIF16.sif");
//        Create_SIF24("system/lsSIF24.sif");
//        Create_SIF36("system/lsSIF36.sif");

//        Create_SIF12("system/htSIF12.sif");
//        Create_SIF16("system/htSIF16.sif");
//        Create_SIF24("system/htSIF24.sif");
//        Create_SIF36("system/htSIF36.sif");

            Create_SIF12("system/stSIF12.sif");
            Create_SIF16("system/stSIF16.sif");
            Create_SIF24("system/stSIF24.sif");
            Create_SIF36("system/stSIF36.sif");
     
          //创建ttf字体
        Create_TTFFont("system/TTF/calibrib.ttf");
        
        WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

        GUI_UC_SetEncodeUTF8();
        CreateHome();
    }
    else
    {
        calebrate_done = 1;
	    WM_MULTIBUF_Enable(1);
	    pCON = CONGetHandle(0);/** @todo (zshare#1#): 双枪时修改ID */
	    WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

	    GUI_UC_SetEncodeUTF8();

        CreateHome();
    }

    while(1)
    {
//	    printf_safe("exec start = %d\n", clock());
        GUI_Exec();
//	    printf_safe("exec end = %d\n", clock());
	    //dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        vTaskDelay(100);
#if 0
        if(bittest(winInitDone,7))
        {
            bitclr(winInitDone,7);
            switch(current_page)
            {
                case _HOMEPAGE:
                    CreateHome();
                    break;
                case _CARDINFOPAGE:
                    CreateCardInfo();
                    break;
                case _CHARGINGPAGE:
                    CreateCharging();
                    break;
                case _CHARGEDONEPAGE:
                    CreateChargeDone();
                    break;
            }
        }
#endif
    }
}
