#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x03)
#define ID_IMAGE_0  (GUI_ID_USER + 0x04)
#define ID_BUTTON_2 (GUI_ID_USER + 0x05)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "selectgun", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "HomeImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { BUTTON_CreateIndirect, "gunA", ID_BUTTON_0, 285, 195, 100, 100, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gunB", ID_BUTTON_1, 455, 195, 100, 100, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_2, 100, 400, 100, 50, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBitmap(hItem, &BitmapSelectGunBack);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapSelectGunAbottonPress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapSelectGunAbottonNotpress, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapSelectGunBbottonPress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapSelectGunBbottonNotpress, 0, 0);
                
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapQuit, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapQuitPress, 0, 0);
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
                Tempuserlike.user_like.ucCONID = 1;
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPSELECTPATTERN);
                break;
            }
            break;
        case ID_BUTTON_1: //'gunB'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                Tempuserlike.user_like.ucCONID = 2;
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPSELECTPATTERN);
                break;
            }
            break;
        case ID_BUTTON_2://�˳�
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
                break;
            }
            break;
        }
        break;
    case MSG_JUMPSELECTPATTERN:
        GUI_EndDialog(pMsg->hWin,0);
        CreateselectpatternDLG();
        break;
    case MSG_JUMPHOME:
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
    return hWin;
}

