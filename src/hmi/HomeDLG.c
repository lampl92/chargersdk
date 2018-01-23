#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"

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

#define ID_Timerstateflash    1

static WM_HTIMER _timerstateflash;

static int i = 1;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Home", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
   // { IMAGE_CreateIndirect, "HomeBack", ID_IMAGE_0, 0, 0, 800, 480, 0, IMAGE_CF_MEMDEV | IMAGE_CF_TILE, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 0, 0, 200, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
//    { IMAGE_CreateIndirect, "gun1stateImage", ID_IMAGE_2, 225, 155, 170, 170, 0, 0, 0 },
//    { IMAGE_CreateIndirect, "gun2stateImage", ID_IMAGE_3, 405, 155, 170, 170, 0, 0, 0 },
    { BUTTON_CreateIndirect, "gun1infobutton", ID_BUTTON_0, 225, 335, 170, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun2infobutton", ID_BUTTON_1, 405, 335, 170, 50, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "electricFeetext", ID_TEXT_1, 210, 55, 100, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "severFeetext", ID_TEXT_2, 545, 55, 100, 24, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 100, 100, 50, 0, 0x0, 0 },
};

static void updategunState(WM_MESSAGE * pMsg)
{
    WM_HWIN      hItem;
    if (gunstate[0] == GunfreeState)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunAfree);
        GUI_MEMDEV_WriteAt(MemdevhomegunAfree,225,155);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
    }
    else if (gunstate[0] == GunchargingState)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunAcharging);
        GUI_MEMDEV_WriteAt(MemdevhomegunAcharging, 225, 0);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
    }
    else if (gunstate[0] == GunchargedoneState)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunAchargedone);
        GUI_MEMDEV_WriteAt(MemdevhomegunAchargedone, 225, 50);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
    }
    else if (gunstate[0] == Gunerror)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunAerror);
        GUI_MEMDEV_WriteAt(MemdevhomegunAerror, 225, 250);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
    }
            
    if (gunstate[1] == GunfreeState)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunBfree);
        GUI_MEMDEV_WriteAt(MemdevhomegunBfree,405,155);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
    }
    else if (gunstate[1] == GunchargingState)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunBcharging);
        GUI_MEMDEV_WriteAt(MemdevhomegunBcharging, 405, 155);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
    }
    else if (gunstate[1] == GunchargedoneState)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunBchargedone);
        GUI_MEMDEV_WriteAt(MemdevhomegunBchargedone, 405, 155);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
    }
    else if (gunstate[1] == Gunerror)
    {
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunBerror);
        GUI_MEMDEV_WriteAt(MemdevhomegunBerror, 405, 155);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;


    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        gunstate[0] = 1;
        gunstate[1] = 1;
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_BLACK, "2018-1-15  18:58");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_BLACK, "12");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLACK, "18");
        
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
//        IMAGE_SetBitmap(hItem, &Bitmaphomeback);
      
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
        IMAGE_SetBitmap(hItem, &Bitmaphomesignal3);

//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunAfree);
//
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
//        IMAGE_SetBitmap(hItem, &BitmaphomegunBfree);
        if (gunstate[0] == GunfreeState || gunstate[0] == Gunerror)
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetText(hItem, "");
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
        }
        else
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
            BUTTON_SetText(hItem, "");
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
        }
        
        if (gunstate[1] == GunfreeState || gunstate[1] == Gunerror)
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetText(hItem, "");
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunscancodepress, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunscancode, 0, 0);
        }
        else
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
            BUTTON_SetText(hItem, "");
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &Bitmaphomegunlookinfopress, 0, 0);
            BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &Bitmaphomegunlookinfo, 0, 0);
        }

        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapbackQuitPress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapbackQuit, 0, 0);
        updategunState(pMsg);
        break;
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevhomeback, 0, 0);
        updategunState(pMsg);
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
                if (gunstate[0] == GunchargingState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatecharginginfoDLG();
                }
                else if (gunstate[0] == GunchargedoneState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatechargedoneinfoDLG();
                }
                break;
            }
            break;
        case ID_BUTTON_1: //'gun2infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
          
                break;
            case WM_NOTIFICATION_RELEASED:
                if (gunstate[1] == GunchargingState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatecharginginfoDLG();
                }
                else if (gunstate[1] == GunchargedoneState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatechargedoneinfoDLG();
                }
                break;
            }
            break;
        case ID_BUTTON_2: //'testButton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:
                GUI_EndDialog(pMsg->hWin, 0);
                CreateselectgunDLG();
                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstateflash)
        {
            if (i > 4 || i < 1) i = 1;
            gunstate[0] = i;
            gunstate[1] = i;
            i++;
            WM_SendMessageNoPara(pMsg->hWin ,WM_PAINT);
            WM_RestartTimer(pMsg->Data.v, 4000);
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
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 5, 0);
    return hWin;
}
