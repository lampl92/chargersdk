/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.30                          *
*        Compiled Jul  1 2015, 10:50:32                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*
*   BUTTON_0    信息查询                                             *
*   BUTTON_1    历史记录
*   BUTTON_2    系统配置
*   BUTTON_3    退出
*   BUTTON_4    模拟量 告警记录
*   BUTTON_5    状态量 充电记录                                         *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/
// USER START (Optionally insert additional includes)
#include "xbffontcreate.h"
#include "touchtimer.h"
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
// USER START (Optionally insert additional defines)
#define ID_IMAGE_0    (GUI_ID_USER + 0x1C)
#define ID_TEXT_1     (GUI_ID_USER + 0x0B)
#define ID_TEXT_2     (GUI_ID_USER + 0x0C)
#define ID_TEXT_3     (GUI_ID_USER + 0x0D)
#define ID_TEXT_4     (GUI_ID_USER + 0x0E)

#define ID_BUTTON_0  (GUI_ID_USER + 0x00)//信息
#define ID_BUTTON_1  (GUI_ID_USER + 0x01)//历史
#define ID_BUTTON_2  (GUI_ID_USER + 0x02)//系统
#define ID_BUTTON_3  (GUI_ID_USER + 0x03)//退出
#define ID_BUTTON_4  (GUI_ID_USER + 0x04)//模拟
#define ID_BUTTON_5  (GUI_ID_USER + 0x05)//状态
#define ID_TEXT_5  (GUI_ID_USER + 0x06)//
#define ID_EDIT_0  (GUI_ID_USER + 0x07)//
#define ID_TEXT_6  (GUI_ID_USER + 0x08)//
#define ID_TEXT_7  (GUI_ID_USER + 0x09)//
#define ID_TEXT_8  (GUI_ID_USER + 0x0A)//
#define ID_TEXT_9  (GUI_ID_USER + 0x0F)//
#define ID_TEXT_10  (GUI_ID_USER + 0x10)//
#define ID_TEXT_11  (GUI_ID_USER + 0x11)//
#define ID_TEXT_12  (GUI_ID_USER + 0x18)//
#define ID_EDIT_1  (GUI_ID_USER + 0x12)//
#define ID_EDIT_2  (GUI_ID_USER + 0x13)//
#define ID_EDIT_3  (GUI_ID_USER + 0x14)//
#define ID_EDIT_4  (GUI_ID_USER + 0x15)//
#define ID_EDIT_5  (GUI_ID_USER + 0x16)//
#define ID_EDIT_6  (GUI_ID_USER + 0x17)//
#define ID_EDIT_7  (GUI_ID_USER + 0x19)//
#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3
// USER END
#define ID_ICONVIEW_0   (GUI_ID_USER + 0x1D)

WM_HWIN IconviewWin;    //ICONVIEW控件窗口

static WM_HWIN _hWinManagerBottom;
static WM_HTIMER _timerRTC, _timerData, _timerSignal;
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data)
// USER END
/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 789, 459, 0, 0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 630, 0, 80, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_2, 720, 0, 70, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 440, 0, 180, 16, 0, 0x0, 0 },//网络信号强度
    { TEXT_CreateIndirect, "Text", ID_TEXT_4, 225, 367, 300, 20, 0, 0x0, 0 },//最底端的说明
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void Data_Flush(WM_MESSAGE *pMsg)
{
    CON_t *pCon;
    uint8_t strTmp[30];
    
    pCon = CONGetHandle(0);

    sprintf(strTmp, "%.1f", pCon->status.dChargingVoltage);
    strcat(strTmp, "V");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_0), strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dChargingCurrent);
    strcat(strTmp, "A");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1), strTmp);

    /**< 控制导引电压确认 */
    sprintf(strTmp, "%.1f", Sys_samp.DC.CP1);
    strcat(strTmp, "V");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_2), strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dChargingFrequence);
    strcat(strTmp, "Hz");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_3), strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dACLTemp);
    strcat(strTmp, "℃");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_4), strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dACNTemp);
    strcat(strTmp, "℃");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_5), strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dBTypeSocketTemp1);//acl or acn
    strcat(strTmp, "℃");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_6), strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dBTypeSocketTemp2);
    strcat(strTmp, "℃");
    EDIT_SetText(WM_GetDialogItem(pMsg->hWin, ID_EDIT_7), strTmp);
}
/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE *pMsg)
{
    const void *pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;
    static int Iconview0_Sel;

    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        /// TODO (zshare#1#): 下面的if不起作用.\
        但是if里嵌套的if起作用,目前先用此来规避不起作用的if
        if(_hWinManagerBottom == cur_win)
        {

        }
        break;
    case WM_INIT_DIALOG:
        //
        // Initialization of 'Framewin'
        //
//        FrameWin_Init(pMsg, ID_TEXT_1, ID_TEXT_2, ID_TEXT_3, ID_TEXT_4,ID_IMAGE_0);

        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id) {
            case ID_ICONVIEW_0:	//ICONVIEW0控件
				switch(NCode)	//通知代码
				{
					case WM_NOTIFICATION_CLICKED:
						break;
					case WM_NOTIFICATION_RELEASED:
						Iconview0_Sel=ICONVIEW_GetSel(pMsg->hWinSrc);
						switch(Iconview0_Sel)
						{
//							case 0:		//APP0
//                                if(BagAPPWin==NULL)CreateBagAPP();      //创建BagAPP
//								break;
//							case 1:		//APP1
//                                if(BookAPPWin==NULL)CreateBookAPP();    //创建BookAPP
//								break;
//                            case 2:		//APP2
//                                if(BookshAPPWin==NULL)CreateBookshAPP();    //创建BookshAPP
//								break;
//                            case 3:		//APP3
//                                if(BrowerAPPWin==NULL)CreateBrowerAPP();    //创建BrowerAPP
//								break;
						}
						break;
					case WM_NOTIFICATION_SEL_CHANGED:
						break;	
				}
            }
		    break;
    case WM_TIMER:
        if(pMsg->Data.v == _timerRTC)
        {
            /**< 显示时间和日期 */
            Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), strCSQ);
            /**< 重启定时器 */
            WM_RestartTimer(pMsg->Data.v, 20);
        }
        if(pMsg->Data.v == _timerSignal)
        {
            WM_RestartTimer(pMsg->Data.v, 2000);
        }
        if(pMsg->Data.v == _timerData)
        {
            
            WM_RestartTimer(pMsg->Data.v,5000);
        }
        break;
    case MSG_CREATERRWIN:
        /**< 故障界面不存在则创建,存在则刷新告警 */
        err_window(pMsg->hWin);
        break;
    case MSG_DELERRWIN:
        /**< 故障界面存在则删除故障界面 */
        if(bittest(winCreateFlag,0))
        {
            bitclr(winCreateFlag,0);
            GUI_EndDialog(err_hItem,0);
            err_hItem = 0;
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/** @brief
 *  输出管理员界面里的信息查询模拟量部分
 * @param
 * @param
 * @return
 *       CreateManagerInfoAnalog
*/
WM_HWIN CreateManagerBottom(void)
{
    uint8_t i = 0;
    _hWinManagerBottom = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    cur_win = _hWinManagerBottom;
    //WM_SetCallback(WM_HBKWIN, _cbDialog);
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinManagerBottom), ID_TimerTime, 20, 0);
    _timerData = WM_CreateTimer(WM_GetClientWindow(_hWinManagerBottom), ID_TimerFlush,1000,0);
    _timerSignal = WM_CreateTimer(WM_GetClientWindow(_hWinManagerBottom), ID_TimerSignal,5000,0);
    
    //建立一个ICONVIEW作为主界面
    IconviewWin = ICONVIEW_CreateEx(0,100,                    //左上角坐标
        800,300,                        //小工具的水平和垂直尺寸
        WM_GetClientWindow(_hWinManagerBottom),                      //父窗口为背景窗口
        WM_CF_SHOW|WM_CF_HASTRANS,      
        ICONVIEW_CF_AUTOSCROLLBAR_V,    //自动增加垂直滚动条
        ID_ICONVIEW_0,                  //小工具ID
        80,                            //图标的水平尺寸为100
        80);                           //图标的垂直尺寸为100
    ICONVIEW_SetFont(IconviewWin, &SIF16_Font);
    ICONVIEW_SetBkColor(IconviewWin, ICONVIEW_CI_SEL, GUI_BLUE);
    ICONVIEW_SetSpace(IconviewWin, GUI_COORD_X, 15);
    ICONVIEW_SetSpace(IconviewWin, GUI_COORD_Y, 15);
    ICONVIEW_SetIconAlign(IconviewWin, ICONVIEW_IA_HCENTER | ICONVIEW_IA_VCENTER);
    ICONVIEW_EnableStreamAuto();
    iconBitmapInit();
    
    //ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapHome, "RETURN");
    //ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapBaseInfo, "基本");
    
        uint32_t bread;
        uint32_t bwrite;
        uint8_t result;
        FIL iconFile;
    
        result = f_open(&iconFile, "system/ICON/home.bmp", FA_READ);	//打开文件
        //文件打开错误或者文件大于BMPMEMORYSIZE
        if ((result != FR_OK) || (iconFile.obj.objsize > ICONMEMORYSIZE)) 	return 1;

        pBitmapHome.pData = malloc(iconFile.obj.objsize);//申请内存

        if (pBitmapHome.pData == NULL)
        {
            return 2;//分配失败
        }
  
        result = f_read(&iconFile, (void *)pBitmapHome.pData, iconFile.obj.objsize, (UINT *)&bread); //读取数据
        if (result != FR_OK) 
        {
            free(pBitmapHome.pData);
            return 3;
        }
        pBitmapHome.XSize = 61;//GUI_BMP_GetXSize(pBitmapHome.pData);
        pBitmapHome.YSize = 58;//GUI_BMP_GetYSize(pBitmapHome.pData);
        pBitmapHome.BytesPerLine = 183;//xper;//一行位图数据含有的字节数
        pBitmapHome.BitsPerPixel = 24;//pInfo.BitsPerPixel;    
        pBitmapHome.pPal = NULL;
        pBitmapHome.pMethods = GUI_DRAW_BMP24;
        //pBitmapHome.pData = pBitmapHome.pData;//(unsigned char *)_acbaseinfo;
    
        f_close(&iconFile);				//关闭BMPFile文件

    
    
    //if (!iconRead("system/ICON/home.bmp", &pBitmapHome))
    {
        //createIcon(&pBitmapHome,100,100,256,32);    
        if (!ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapHome, "返回"))
        {
            printf_safe("chenggong!\n");
        }
    }
    
//    if (!iconRead("system/ICON/log.bmp", &pBitmapLog))
//    {
//        createIcon(&pBitmapLog, 64, 64, 256, 32);    
//        ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapLog, "记录");
//    }
//    
//    if (!iconRead("system/ICON/netinfo.bmp", &pBitmapNet))
//    {
//        createIcon(&pBitmapNet, 64, 64, 256, 32);
//        if (!ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapNet, "网络"))
//        {
//            printf_safe("chengogng!\n");
//        }
//    } 
//
//    if (!iconRead("system/ICON/evseinfo.bmp", &pBitmapEvseInfo))
//    {
//        createIcon(&pBitmapEvseInfo, 64, 64, 256, 32);    
//        ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapEvseInfo, "桩体");
//    } 
//    
//    if (!iconRead("system/ICON/baseinfo.bmp", &pBitmapBaseInfo))
//    {
//        createIcon(&pBitmapBaseInfo, 64, 64, 256, 32); 
//        ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapBaseInfo, "基本");
//    }
//    
//    if (!iconRead("system/ICON/sysset.bmp", &pBitmapSys))
//    {
//        createIcon(&pBitmapSys, 64, 64, 256, 32); 
//        ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapSys, "设置");
//    }
//    
//    if (!iconRead("system/ICON/rotate.bmp", &pBitmapRotate))
//    {
//        createIcon(&pBitmapRotate, 64, 64, 256, 32);    
//        ICONVIEW_AddBitmapItem(IconviewWin, &pBitmapRotate, "翻转");
//    }
}
/*************************** End of file ****************************/


