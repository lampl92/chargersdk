#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_2 (GUI_ID_USER + 0x07)
#define ID_TEXT_3 (GUI_ID_USER + 0x08)
#define ID_IMAGE_2 (GUI_ID_USER + 0x09)
#define ID_BUTTON_0 (GUI_ID_USER + 0x0A)

#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3

static WM_HTIMER _timerRTC, _timerData, _timerSignal;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "CardInfoback", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { TEXT_CreateIndirect, "CardNumber", ID_TEXT_2, 326, 165, 280, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Balance", ID_TEXT_3, 326, 223, 280, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "PromptImage", ID_IMAGE_2, 240, 316, 319, 59, 0, 0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_0, 100, 400, 100, 50, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLACK, "1234567890");//卡号
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_BLACK, "?");        
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);   //背景
        IMAGE_SetBitmap(hItem, &Bitmapcardinfoback);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2); //提示
        IMAGE_SetBitmap(hItem, &Bitmapcardinfoget);        
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapQuit, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapQuitPress, 0, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'quit'
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
    case WM_PAINT:
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerRTC)
        {
            WM_RestartTimer(pMsg->Data.v, 300);
        }
        if (pMsg->Data.v == _timerSignal)
        {        
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        if (pMsg->Data.v == _timerData)
        {
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

/*
*  CreateCardInfoPage
*       
*/
WM_HWIN CreateCardInfoDLG(void);
WM_HWIN CreateCardInfoDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerRTC = WM_CreateTimer(hWin, ID_TimerTime, 20, 0);
    _timerData = WM_CreateTimer(hWin, ID_TimerFlush, 10, 0);
    _timerSignal = WM_CreateTimer(hWin, ID_TimerSignal, 300, 0);
    return 0;
}