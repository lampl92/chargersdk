#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)

#define ID_statechangeTime    1

static WM_HTIMER _timerstatechange;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    //45,77---x:45+320=365,³¤¶È:(405-320=85),y:47+77=124
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 365, 124, 85, 22, TEXT_CF_VCENTER | TEXT_CF_HCENTER, 0x0, 0 },
//    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 40, 120, 400, 0, 0x0, 0 },
     { BUTTON_CreateIndirect, "quit", ID_BUTTON_2, 52, 404, 171, 59, 0, 0x0, 10 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN  hItem;
    uint8_t strblance[10];
    char s[10];
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        sprintf(strblance, "%6.1f", GBSBalance);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg36e);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_WHITE);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), strblance);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetUserData(hItem, "unplug", 10);
        BUTTON_SetSkin(hItem, SKIN_buttonPleasePlugQuit);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_2: //'quitButton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
                BUTTON_GetUserData(hItem, s, 10);
                if (strcmp("unplug", s) == 0)
                {
                    quitflag = 1;
                }
                break;
            }
            break;
        }
        break;
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevcardinfoback, 0, 0);
        if (Tempuserlike.user_like.ucCONID == 0)
        {
            GUI_MEMDEV_WriteAt(Memdevcardinfopleasepluga, 120, 103);
        }
        else if (Tempuserlike.user_like.ucCONID == 1)
        {
            GUI_MEMDEV_WriteAt(Memdevcardinfopleaseplugb, 120, 103);
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstatechange)
        {    
            if (gbsstate == StatePlug)
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
                BUTTON_SetUserData(hItem, "plug", 10);
                WM_InvalidateWindow(hItem);
            }
            if (gbsstate == StatePlugTimeout)
            {
                WM__SendMessageNoPara(pMsg->hWin, MSG_JUMPStatePlugTimeout);
            }
            if (gbsstate == StateChargingOk)
            {
                WM__SendMessageNoPara(pMsg->hWin, MSG_JUMPChargingOk);
            }
            if (gbsstate == StateHome)
            {
                WM__SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateHomeDLG();
        break;
    case MSG_JUMPChargingOk:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatechargingokDLG();
        break;
    case MSG_JUMPStatePlugTimeout:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateplugtimeoutDLG();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreatepleaseplugDLG(void);
WM_HWIN CreatepleaseplugDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstatechange = WM_CreateTimer(hWin, ID_statechangeTime, 100, 0);
    return 0;
}

