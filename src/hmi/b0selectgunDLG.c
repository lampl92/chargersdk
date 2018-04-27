#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x03)
#define ID_IMAGE_0  (GUI_ID_USER + 0x04)
#define ID_BUTTON_2 (GUI_ID_USER + 0x05)
#define ID_Timerstateflash 1
static WM_HTIMER _timerstateflash;
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "selectgun", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gunA", ID_BUTTON_0, 137, 200, 213, 76, 0, 0x0, 10 },
    { BUTTON_CreateIndirect, "gunB", ID_BUTTON_1, 454, 200, 213, 76, 0, 0x0, 10 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_2, 52, 404, 171, 59, 0, 0x0, 10 },
};

static void selectGunButton(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetUserData(hItem, "enable", 10);
    if (GBSgunstate[0] != GunfreeState)
    {
        BUTTON_SetUserData(hItem, "disable", 10);
    }
    BUTTON_SetSkin(hItem, SKIN_selectbutton);
        
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetUserData(hItem, "enable", 10);
    if (GBSgunstate[1] != GunfreeState)
    {
        BUTTON_SetUserData(hItem, "disable", 10);
    }
    BUTTON_SetSkin(hItem, SKIN_selectbutton);
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;
    char s[10];
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:        
        selectGunButton(pMsg);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetSkin(hItem, SKIN_buttonquit);
        BUTTON_SetUserData(hItem, "canPress", 10);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break; 
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(MemdevSelectGunBack,0,0);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'gunA'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                if (GBSgunstate[0] == GunfreeState)
                {
                    Tempuserlike.user_like.ucCONID = 0;
                    WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPSELECTPATTERN);
                }
                break;
            }
            break;
        case ID_BUTTON_1: //'gunB'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                if (GBSgunstate[1] == GunfreeState)
                {
                    Tempuserlike.user_like.ucCONID = 1;
                    WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPSELECTPATTERN);
                }
                break;
            }
            break;
        case ID_BUTTON_2://ÍË³ö
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
                BUTTON_GetUserData(hItem, s, 10);
                if (strcmp(s, "canPress") == 0)
                {
                    quitflag = 1;    
                    BUTTON_SetUserData(hItem, "noPress", 10);
                }                
                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstateflash)
        {
            if (gbsstate == StateHome)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            selectGunButton(pMsg);
            WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateHomeDLG();
        break;
    case MSG_JUMPSELECTPATTERN:
        GUI_EndDialog(pMsg->hWin,0);
        CreateselectpatternbetterDLG();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}


WM_HWIN CreateselectgunDLG(void);
WM_HWIN CreateselectgunDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 100, 0);
    return hWin;
}

