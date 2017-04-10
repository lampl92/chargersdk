#include "GUI.h"
#include "xbffontcreate.h"
#include "touchtimer.h"

void MainTask(void)
{
	GUI_CURSOR_Show();

    Create_XBF12("system/XBF宋体12.xbf");//创建xbf12号路径
    Create_XBF16("system/XBF宋体16.xbf");//创建xbf16号路径
    Create_XBF24("system/XBF宋体24.xbf");//创建xbf24号路径
    Create_XBF36("system/XBF宋体36.xbf");//创建xbf36号路径


    WM_SetDesktopColor(GUI_WHITE);//设置背景颜色

    GUI_UC_SetEncodeUTF8();
    //Createbmptest();
    PutOut_Home();
    //PutOut_Card_Valid();
	//PutOut_Machine_Error();
	//PutOut_Card_Info();
	//PutOut_Charging();
	//PutOut_Charging_2dimen();
	//PutOut_Charge_Done();
    //PutOut_Manager_InfoAnalog();
	//PutOut_Manager_InfoStatus();
	//keypad_demo();
	//Createtest();
	//CreateAlarmLog();

	//GUI_Touch_Calibrate();
    while(1)
    {
        GUI_Delay(1000);
    }
}
