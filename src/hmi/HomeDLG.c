#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_IMAGE_1 (GUI_ID_USER + 0x03)
#define ID_IMAGE_2 (GUI_ID_USER + 0x04)
#define ID_IMAGE_3 (GUI_ID_USER + 0x05)
#define ID_BUTTON_0 (GUI_ID_USER + 0x06)
#define ID_BUTTON_1 (GUI_ID_USER + 0x07)
#define ID_TEXT_1 (GUI_ID_USER + 0x08)
#define ID_TEXT_2 (GUI_ID_USER + 0x09)
#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Home", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "HomeBack", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 0, 0, 200, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { IMAGE_CreateIndirect, "gun1stateImage", ID_IMAGE_2, 225, 155, 170, 170, 0, 0, 0 },
    { IMAGE_CreateIndirect, "gun2stateImage", ID_IMAGE_3, 405, 155, 170, 170, 0, 0, 0 },
    { BUTTON_CreateIndirect, "gun1infobutton", ID_BUTTON_0, 225, 335, 170, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun2infobutton", ID_BUTTON_1, 405, 335, 170, 50, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "electricFeetext", ID_TEXT_1, 210, 55, 100, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "severFeetext", ID_TEXT_2, 545, 55, 100, 24, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 100, 100, 50, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_BLACK, "2018-1-15  18:58");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_BLACK, "12");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLACK, "18");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBitmap(hItem, &Bitmaphomeback);
      
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
        IMAGE_SetBitmap(hItem, &Bitmaphomesignal0);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
        IMAGE_SetBitmap(hItem, &BitmaphomegunAfree);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
        IMAGE_SetBitmap(hItem, &BitmaphomegunBfree);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapbackQuitPress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapbackQuit, 0, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'gun1infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:

                break;
            }
            break;
        case ID_BUTTON_1: //'gun2infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
          
                break;
            case WM_NOTIFICATION_RELEASED:

                break;
            }
            break;
        case ID_BUTTON_2: //'testButton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:

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

WM_HWIN CreateHomeDLG(void);
WM_HWIN CreateHomeDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    return hWin;
}
