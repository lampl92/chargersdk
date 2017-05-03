#include "GUI.h"
#include "xbffontcreate.h"
#include "ff.h"
#include "HMI_Start.h"

uint8_t calebrate_done = 0;

void MainTask(void)
{
    if(calebrate_done == 0)
    {
        //GUI_Touch_Calibrate();
        calebrate_done = 1;
        //LCD_X_Config();
    }
	//GUI_CURSOR_Show();//����ͷ��ʾ������Ĭ�ϲ���ʾ
	//CreateEVSE();
    Create_XBF12("system/XBF����12.xbf");//����xbf12��·��
    Create_XBF16("system/XBF����16.xbf");//����xbf16��·��
    Create_XBF24("system/XBF����24.xbf");//����xbf24��·��
    Create_XBF36("system/XBF����36.xbf");//����xbf36��·��


    WM_SetDesktopColor(GUI_WHITE);//���ñ�����ɫ

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
