#include "GUI.h"
#include "xbffontcreate.h"
#include "ff.h"

uint8_t calebrate_done = 0;

void MainTask(void)
{
    if(calebrate_done == 0)
    {
        //GUI_Touch_Calibrate();
        calebrate_done = 1;
        //LCD_X_Config();
    }
	//GUI_CURSOR_Show();//鼠标箭头显示函数，默认不显示
	//CreateEVSE();
    Create_XBF12("system/XBF宋体12.xbf");//创建xbf12号路径
    Create_XBF16("system/XBF宋体16.xbf");//创建xbf16号路径
    Create_XBF24("system/XBF宋体24.xbf");//创建xbf24号路径
    Create_XBF36("system/XBF宋体36.xbf");//创建xbf36号路径


    WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

    GUI_UC_SetEncodeUTF8();
    //keypad_demo();
//	CreateFramewin();
//    dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
    PutOut_Home();
	//PutOut_Card_Valid();
	while(1)
	{
		GUI_Delay(10);
	}
}
