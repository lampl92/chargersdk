#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
//#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)

#define ID_statechangeTime    1

static WM_HTIMER _timerstatechange;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 367, 147, 240, 24, 0, 0x0, 0 },
//    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 40, 120, 400, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN      hItem;
    uint8_t strblance[10];

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        sprintf(strblance, "%6.1f", GBSBalance);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg24e);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), strblance);
//        
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
//        BUTTON_SetSkin(hItem, SKIN_buttontest);
        break;
//    case WM_NOTIFY_PARENT:
//        Id    = WM_GetId(pMsg->hWinSrc);
//        NCode = pMsg->Data.v;
//        switch (Id) {
//        case ID_BUTTON_2: //'testButton'
//            switch (NCode) {
//            case WM_NOTIFICATION_CLICKED:
//
//                break;
//            case WM_NOTIFICATION_RELEASED:
//                GUI_EndDialog(pMsg->hWin, 0);
//                CreateHomeDLG();
//                break;
//            }
//            break;
//        }
//        break;
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevcardinfoback, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevcardinfochargingok, 195, 110);
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstatechange)
        { 
            if (gbsstate == StateHome)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateHomeDLG();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreatechargingokDLG(void);
WM_HWIN CreatechargingokDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstatechange = WM_CreateTimer(hWin, ID_statechangeTime, 100, 0);
    return 0;
}

