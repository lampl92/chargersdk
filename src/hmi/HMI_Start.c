#include "GUI.h"
#include "xbffontcreate.h"
#include "ff.h"

void MainTask(void)
{
	//GUI_CURSOR_Show();//����ͷ��ʾ������Ĭ�ϲ���ʾ
	//CreateEVSE();
    Create_XBF12("system/XBF����12.xbf");//����xbf12��·��
    Create_XBF16("system/XBF����16.xbf");//����xbf16��·��
    Create_XBF24("system/XBF����24.xbf");//����xbf24��·��
    Create_XBF36("system/XBF����36.xbf");//����xbf36��·��


    WM_SetDesktopColor(GUI_WHITE);//���ñ�����ɫ

    GUI_UC_SetEncodeUTF8();
    //CreateEVSE();
    //Createbmptest();
    //PutOut_Home();
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
	CreateFramewin();
    dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);

	while(1)
	{
		GUI_Delay(10);
	}
}
