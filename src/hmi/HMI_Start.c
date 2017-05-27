#include "GUI.h"
#include "xbffontcreate.h"
#include "ff.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "interface.h"
uint8_t calebrate_done = 0;

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
    pCON = CONGetHandle(0);/** @todo (zshare#1#): ˫ǹʱ�޸�ID */
    encodetobmp("system/encodeCharge.bmp",pCON->info.strQRCode);

    Create_XBF12("system/XBF����12.xbf");//����xbf12��·��
    Create_XBF16("system/XBF����16.xbf");//����xbf16��·��
    Create_XBF24("system/XBF����24.xbf");//����xbf24��·��
    Create_XBF36("system/XBF����36.xbf");//����xbf36��·��


    WM_SetDesktopColor(GUI_WHITE);//���ñ�����ɫ

    GUI_UC_SetEncodeUTF8();
    //PutOut_SelAOrB();
    //PutOut_Window();
    //PutOut_Charging_2dimen();
    PutOut_Home();
    //PutOut_Charge_Done();
    //PutOut_Card_Info();
	//PutOut_Card_Valid();
}
