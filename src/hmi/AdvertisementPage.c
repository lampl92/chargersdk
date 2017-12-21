#include "DIALOG.h"
#include "touchtimer.h"
#include "HMI_Start.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)

#define ID_IMAGE_0_IMAGE_0 0x00
#define ID_TimerAdver 4
static WM_HTIMER _timerAdver;

WM_HWIN _hWinAdvertizement;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "AdvertisementPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "AdvertisementImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },    
};

static void Data_Process(WM_MESSAGE *pMsg)
{
    CON_t *pCON;
    pCON = CONGetHandle(0);
    if (cur_win == _hWinHome)
    {
        if (pCON->status.xPlugState == PLUG)
        {
            if (AdvertisementRecordFlag == 1)
            {
                WM_HideWindow(_hWinAdvertizement);
                WM_ShowWindow(cur_win);
                AdvertisementRecordFlag = 0;
                GUI_Exec();
            }
        }
    }
    if (cur_win == _hWinCharging)
    {
        if (pCON->status.xPlugState == UNPLUG)
        {
            if (AdvertisementRecordFlag == 1)
            {
                WM_HideWindow(_hWinAdvertizement);
                WM_ShowWindow(cur_win);
                AdvertisementRecordFlag = 0;
                GUI_Exec();
            }
        }
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
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBMP(hItem, AdvertisementImage->pfilestring, AdvertisementImage->pfilesize);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (NCode)
        {
        case WM_NOTIFICATION_CLICKED:
        //case WM_NOTIFICATION_RELEASED:
        case WM_NOTIFICATION_MOVED_OUT:
            switch (Id)
            {
            case ID_IMAGE_0:
                if (AdvertisementRecordFlag == 1)
                {
                    WM_HideWindow(_hWinAdvertizement);
                    WM_ShowWindow(cur_win);
                    AdvertisementRecordFlag = 0;
                    GUI_Exec();
                }
                break;
            default:
                break;
            }
        default:
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerAdver)
        {
            Data_Process(pMsg);
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}
/*
* CreateAdvertisementPage
*/
WM_HWIN CreateAdvertisementPage(void);
WM_HWIN CreateAdvertisementPage(void) {
    _hWinAdvertizement = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerAdver = WM_CreateTimer(WM_GetClientWindow(_hWinAdvertizement), ID_TimerAdver, 100, 0);
    return 0;
}


