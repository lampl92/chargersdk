#include "touchtimer.h"
#include "HMI_Start.h"

#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_MULTIPAGE_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_0     (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)

#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3

WM_HWIN _hWinManagerCommon;

static WM_HTIMER _timerRTC, _timerData, _timerSignal;
volatile static int page = 0;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { WINDOW_CreateIndirect, "Home", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 350, 7, 235, 30, TEXT_CF_HCENTER, 0x0, 0 },
    { TEXT_CreateIndirect, "Headline", ID_TEXT_1, 10, 5, 230, 32, TEXT_CF_LEFT, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_0, 730, 10, 50, 50, 0, 0x0, 0 },
    { MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_0, 10, 45, 780, 420, 0, 0x0, 0 },
    //{ MULTIPAGE_CreateIndirect, "Multipage", ID_MULTIPAGE_0, 10, 45, 300, 100, 0, 0x0, 0 },
};

static void updatetime(WM_MESSAGE *pMsg, uint16_t ID_TEXT)
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    WM_HWIN hWin = pMsg->hWin;
    uint8_t Date_buf[30];
    uint8_t Time_buf[30];
    uint8_t date_time_buf[100];

    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
    sprintf((char *)Date_buf, "20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);

    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    sprintf((char *)Time_buf, "%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    
    sprintf((char*)date_time_buf, "%s   %s", Date_buf, Time_buf);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT), date_time_buf);
}

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
    int i;

    pCont = CONGetHandle(0);
    
    switch (pMsg->MsgId)
    {
    case WM_CREATE:
        break;
    case WM_PAINT:
        //GUI_SetColor(0x3399FF);//蓝色
        //GUI_SetColor(0x33CCFF);//绿蓝色
        GUI_SetColor(color_manager_out);
        GUI_FillRect(0, 0, 800, 480);
//        GUI_FillRect(0, 0, 800, 40);
//        GUI_FillRect(0, 0, 10, 480);
//        GUI_FillRect(0, 460, 800, 480);
//        GUI_FillRect(790, 0, 800, 480);
        break;
    case WM_INIT_DIALOG:                
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetFont(hItem, &fontwryhcg30e);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
        TEXT_SetTextColor(hItem, GUI_WHITE);
        TEXT_SetFont(hItem, &fontwryhcg36e);
        TEXT_SetText(hItem, "充电桩管理系统");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttonManagerQuit);
        //GUI_SetColor(color_manager_out);
       // BUTTON_SetFont(hItem, &fontwryhcg36e);
       // BUTTON_SetText(hItem, "X");
        

        hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_0);
        //MULTIPAGE_SetBkColor(hItem, color_manager_in, MULTIPAGE_CI_ENABLED);
        //MULTIPAGE_SetFont(hItem, &SIF24_Font);
        MULTIPAGE_SetFont(hItem, &fontwryhcg30e);
        MULTIPAGE_SetTabHeight(hItem,50);                    
        
        hWinPage = CreateManagerInfoAnalog(pMsg->hWin);
        MULTIPAGE_AddEmptyPage(hItem, hWinPage, "状态信息");

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

        MULTIPAGE_SelectPage(hItem, 0);
        
        //MULTIPAGE_SetSkin(hItem, SKIN_mulitipage);
        for (i = 0; i < 6;i++)
        {
            MULTIPAGE_SetTabWidth(hItem, 120,i);   
        }

        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0:	//退出管理员进行的相关控件的删除操作
            switch (NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
            case WM_NOTIFICATION_MOVED_OUT:
//                WM_SendMessageNoPara(_hWinManagerInfoAnalog, MSG_DELETEMANAGERWIN);
//                WM_SendMessageNoPara(_hWinManagerInfoStatus, MSG_DELETEMANAGERWIN);
//                WM_SendMessageNoPara(_hWinManagerLogDate, MSG_DELETEMANAGERWIN);
//                WM_SendMessageNoPara(_hWinManagerConSet, MSG_DELETEMANAGERWIN);
//                WM_SendMessageNoPara(_hWinManagerSysSet, MSG_DELETEMANAGERWIN);
//                WM_SendMessageNoPara(_hWinManagerSysInfo, MSG_DELETEMANAGERWIN); 
                GUI_EndDialog(pMsg->hWin, 0);
                home();              
                break;
            }
            break;
        case ID_MULTIPAGE_0: // Notifications sent by 'Button'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                page = MULTIPAGE_GetSelection(WM_GetDialogItem(pMsg->hWin, ID_MULTIPAGE_0));
                switch (page)
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
        if (pMsg->Data.v == _timerRTC)
        {
            updatetime(pMsg, ID_TEXT_0);
            WM_RestartTimer(pMsg->Data.v, 400);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreateManagerCommon(void)
{
    _hWinManagerCommon = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    //cur_win = _hWinManagerCommon;
    _timerRTC = WM_CreateTimer(_hWinManagerCommon, ID_TimerTime, 20, 0);
    return _hWinManagerCommon;
}

/*************************** End of file ****************************/



