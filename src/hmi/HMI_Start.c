#include "GUI.h"
#include "xbffontcreate.h"
#include "bmpdisplay.h"
#include "ff.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "interface.h"

uint8_t calebrate_done = 0;
uint8_t winInitDone = 0;
uint8_t current_page = 0;

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
        encodetobmp("system/encodeCharge.bmp",pCON->info.strQRCode);
        //readBackGroundNOFREE("system/background.bmp");
        readBackGroundNOFREE("system/background_tree.bmp");
        readEncodeNOFREE("system/encodeCharge.bmp");
        Create_XBF12("system/XBF_Song_12.xbf");//创建xbf12号路径
        Create_XBF14("system/XBF_Song_14.xbf");//创建XBF14号路径
        Create_XBF16("system/XBF_Song_16.xbf");//创建xbf16号路径
        Create_XBF19("system/XBF_Song_19.xbf");//创建xbf19号路径
        Create_XBF24("system/XBF_Song_24.xbf");//创建xbf24号路径
        Create_XBF36("system/XBF_Song_36.xbf");//创建xbf36号路径


        WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

        GUI_UC_SetEncodeUTF8();
    //    PutOut_SelAOrB();
    //    PutOut_Window();
    //    PutOut_Window();
    //    PutOut_Charging_2dimen();
        //Keypad_GetValue(LOGIN_PASSWD,"");
        CreateHome();
        //CreateCardInfo();
        //CreateCharging();
        //CreateChargeDone();
        //CreateManagerInfoAnalog();
        //CreateManagerInfoStatus();
        //CreateManagerAlarmLog();
        //CreateManagerSysSet();
    //    PutOut_Card_Info();
    //    PutOut_Charging();
    //
    //    PutOut_Charge_Done();
    //
    //	PutOut_Card_Valid();
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
    }
}
