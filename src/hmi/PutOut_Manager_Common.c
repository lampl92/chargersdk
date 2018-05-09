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
#include "touchtimer.h"
#include "HMI_Start.h"
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
#define ID_IMAGE_0    (GUI_ID_USER + 0x01)
#define ID_TEXT_1     (GUI_ID_USER + 0x02)
#define ID_TEXT_2     (GUI_ID_USER + 0x03)
#define ID_TEXT_3     (GUI_ID_USER + 0x04)
#define ID_TEXT_4     (GUI_ID_USER + 0x05)
#define ID_MULTIPAGE_0 (GUI_ID_USER + 0x06)
#define ID_BUTTON_0     (GUI_ID_USER + 0x07)
#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3
// USER END
WM_HWIN _hWinManagerCommon;
static WM_HTIMER _timerRTC,_timerData,_timerSignal;
volatile static int page = 0;
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
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 630, 0, 80, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_2, 720, 0, 70, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 440, 0, 180, 16, 0, 0x0, 0 },//网络信号强度
    { TEXT_CreateIndirect, "Text", ID_TEXT_4, 225, 367, 300, 20, 0, 0x0, 0 },//最底端的说明
	{ MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_0, 0, 0, 800, 360, 0, 0x0, 0 },//multipage
    { BUTTON_CreateIndirect, "退出管理", ID_BUTTON_0, 650, 380, 100, 30, 0, 0x0, 0 },
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
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
    WM_HWIN     hWinPage;
    CON_t       *pCont;
    EventBits_t uxBits;

    pCont = CONGetHandle(0);
    
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        Err_Analy(pMsg->hWin);
        CaliDone_Analy(pMsg->hWin);
        break;
    case WM_INIT_DIALOG:
        //
        // Initialization of 'Framewin'
        //
        FrameWin_Init(pMsg, ID_TEXT_1, ID_TEXT_2, ID_TEXT_3, ID_TEXT_4,ID_IMAGE_0);
        //
        //初始化所有的page
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_0);
        MULTIPAGE_SetFont(hItem, &SIF24_Font);

        hWinPage = CreateManagerInfoAnalog(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "模拟量");

        hWinPage = CreateManagerInfoStatus(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "状态量");

        hWinPage = CreateManagerLogDate(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "记录查询");

        hWinPage = CreateManagerConSet(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "枪A配置");

        hWinPage = CreateManagerSysSet(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "厂商配置");

        hWinPage = CreateManagerSysInfo(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "系统信息");
        
        if (managerLevel == 0)
        {
            hWinPage = CreateManagerTerminate(pMsg->hWin);
            MULTIPAGE_AddEmptyPage(hItem, hWinPage, "终端");
        }
        
//        hWinPage = CreateManagerExit(pMsg->hWin);
//        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "退出管理员");

        MULTIPAGE_SelectPage(hItem,0);

        Button_Show(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), GUI_TA_HCENTER|GUI_TA_VCENTER,
            &SIF24_Font,BUTTON_CI_UNPRESSED, GUI_BLACK, BUTTON_CI_UNPRESSED,GUI_BLACK, "退出管理");


        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id) {
        case ID_BUTTON_0:	//退出管理员进行的相关控件的删除操作
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                WM_SendMessageNoPara(_hWinManagerInfoAnalog, MSG_DELETEMANAGERWIN);
                WM_SendMessageNoPara(_hWinManagerInfoStatus, MSG_DELETEMANAGERWIN);
                WM_SendMessageNoPara(_hWinManagerLogDate, MSG_DELETEMANAGERWIN);
                WM_SendMessageNoPara(_hWinManagerConSet, MSG_DELETEMANAGERWIN);
                WM_SendMessageNoPara(_hWinManagerSysSet, MSG_DELETEMANAGERWIN);
                WM_SendMessageNoPara(_hWinManagerSysInfo, MSG_DELETEMANAGERWIN);
                if (managerLevel == 0)
                {
                    WM_SendMessageNoPara(_hWinManagerTerminate, MSG_DELETEMANAGERWIN);
                    managerLevel = 1;
                }
                _deleteWin(_hWinManagerCommon);
                home();              
                break;
            }
            break;
        case ID_MULTIPAGE_0: // Notifications sent by 'Button'
          switch(NCode) {
          case WM_NOTIFICATION_CLICKED:
            // USER START (Optionally insert code for reacting on notification message)
            // USER END
            break;
          case WM_NOTIFICATION_RELEASED:
            // USER START (Optionally insert code for reacting on notification message)
            page = MULTIPAGE_GetSelection(WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_0));
            switch(page)
            {
            case 0:
                //发送对应页初始化的消息
//                WM_SendMessageNoPara(_hWinManagerCommon,0);
                break;
            case 1:
//                WM_SendMessageNoPara(_hWinManagerCommon, 1);
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            }
            // USER END
            break;
          // USER START (Optionally insert additional code for further notification handling)
          // USER END
          }
          break;
        }
        break;
    case WM_TIMER:
        if(pMsg->Data.v == _timerRTC)
        {
            /**< 显示时间和日期 */
            Led_Show();
            Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);
           // TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), strCSQ);
            /**< 重启定时器 */
            WM_RestartTimer(pMsg->Data.v, 20);
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
 *  输出管理员界面里的共用部分
 * @param
 * @param
 * @return
 *       CreateManagerCommon
*/
WM_HWIN CreateManagerCommon(void)
{
    _hWinManagerCommon = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    cur_win = _hWinManagerCommon;
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinManagerCommon), ID_TimerTime, 20, 0);
}
/*************************** End of file ****************************/



