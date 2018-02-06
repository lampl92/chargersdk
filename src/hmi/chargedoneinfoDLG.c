#include "DIALOG.h"
#include "HMI_Start.h"
#include "display.h"
#include "touchtimer.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
//#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_TEXT_1 (GUI_ID_USER + 0x03)
#define ID_TEXT_2 (GUI_ID_USER + 0x04)
#define ID_BUTTON_2 (GUI_ID_USER +0x05)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "chargedoneinfo", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
//    { IMAGE_CreateIndirect, "chargedoneinfoImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { TEXT_CreateIndirect, "dutext", ID_TEXT_0, 365, 195, 80, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "yuantext", ID_TEXT_1, 365, 235, 80, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "fentext", ID_TEXT_2, 365, 275, 80, 24, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_2, 50, 350, 170, 70, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
//        IMAGE_SetBitmap(hItem, &Bitmapchargedoneinfo);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_BLACK, "20");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_BLACK, "12");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLACK, "18");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetSkin(hItem ,SKIN_buttonquit);
//        BUTTON_SetText(hItem, "");
//        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapbackQuitPress, 0, 0);
//        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapbackQuit, 0, 0);
        break;
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevchargedoneinfo, 0, 0);
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
                GUI_EndDialog(pMsg->hWin, 0);
                CreateHomeDLG();
                break;
            }
            break;
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreatechargedoneinfoDLG(void);
WM_HWIN CreatechargedoneinfoDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}
