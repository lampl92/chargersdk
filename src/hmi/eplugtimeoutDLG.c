#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)

#define ID_statechangeTime    1

static WM_HTIMER _timerstatechange;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN      hItem;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        break;
    case WM_NOTIFY_PARENT:
        break;
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevcardinfoback, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevcardinfoplugtimeout, 45, 77);
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstatechange)
        { 
            if (gbsstate == StateHome)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            WM_RestartTimer(pMsg->Data.v, 300);
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

WM_HWIN CreateplugtimeoutDLG(void);
WM_HWIN CreateplugtimeoutDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstatechange = WM_CreateTimer(hWin, ID_statechangeTime, 20, 0);
    return 0;
}

