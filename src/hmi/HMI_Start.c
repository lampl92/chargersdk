#include "GUI.h"
#include "xbffontcreate.h"
#include "bmpdisplay.h"
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
    //readBackGroundNOFREE("system/background.bmp");
    readBackGroundNOFREE("system/background_tree.bmp");
    readEncodeNOFREE("system/encodeCharge.bmp");
    Create_XBF12("system/XBF����12.xbf");//����xbf12��·��
    Create_XBF14("system/XBF����14.xbf");//����XBF14��·��
    Create_XBF16("system/XBF����16.xbf");//����xbf16��·��
    Create_XBF19("system/XBF����19.xbf");//����xbf19��·��
    Create_XBF24("system/XBF����24.xbf");//����xbf24��·��
    Create_XBF36("system/XBF����36.xbf");//����xbf36��·��


    WM_SetDesktopColor(GUI_WHITE);//���ñ�����ɫ

    GUI_UC_SetEncodeUTF8();
//    PutOut_SelAOrB();
//    PutOut_Window();
//    PutOut_Window();
//    PutOut_Charging_2dimen();
    //CreateHome();
    CreateCardInfo();
//
//    PutOut_Card_Info();
//    PutOut_Charging();
//
//    PutOut_Charge_Done();
//
//	PutOut_Card_Valid();
    while(1)
    {
        GUI_Delay(100);
        dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        vTaskDelay(100);
    }
}
