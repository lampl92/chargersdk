#include "DIALOG.h"
#include "touchtimer.h"
#include "HMI_Start.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)

#define ID_IMAGE_0_IMAGE_0 0x00

#define ID_TimerTime    1

static WM_HTIMER _timerRTC;

uint8_t AdvertisementRecordFlag;//记录广告来源，0位为home页，1位为charging页
WM_HWIN _hWinAdvertizement;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "AdvertisementPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "AdvertisementImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
};

static void Data_Process(WM_MESSAGE *pMsg)
{
    CON_t *pCON;
    EventBits_t uxBitRFID;
    EventBits_t uxBitHMI;
    WM_HWIN hWin = pMsg->hWin;
    pCON = CONGetHandle(0);
    
    SignalIntensity = getSignalIntensity();
    
    if (bittest(AdvertisementRecordFlag, 0))
    {
        uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
            defEventBitGotIDtoHMI,
            pdTRUE,
            pdTRUE,
            0);
        if (((uxBitRFID & defEventBitGotIDtoHMI) == defEventBitGotIDtoHMI)  && (SignalIntensity > 0))
        {
            WM_SendMessageNoPara(hWin, MSG_JUMPCARDINFO);
        }
        if (pCON->status.xPlugState == PLUG)
        {
            WM_SendMessageNoPara(hWin, MSG_JUMPHOME);
        }
    }
    if (bittest(AdvertisementRecordFlag, 1))
    {
        uxBitHMI = xEventGroupWaitBits(xHandleEventHMI,
            defEventBitHMI_ChargeReqDispDone,
            pdTRUE,
            pdTRUE,
            0);
        if ((uxBitHMI & defEventBitHMI_ChargeReqDispDone) == defEventBitHMI_ChargeReqDispDone)
        {
            /**< 跳到充电完成 */
            WM_SendMessageNoPara(hWin, MSG_JUMPCHARGEDONE);
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
        case WM_NOTIFICATION_RELEASED:
        case WM_NOTIFICATION_MOVED_OUT:
            switch (Id)
            {
            case ID_IMAGE_0:
                prePowerFee = 0;
                preServiceFee = 0;
                AdvertisementRecordFlag = 0;
                _deleteWin(_hWinAdvertizement);
                _hWinAdvertizement = 0;
                CreateHomePage();
                break;
            default:
                break;
            }
        default:
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerRTC)
        {
            Data_Process(pMsg);
            WM_RestartTimer(pMsg->Data.v, 100);
        }
    case MSG_CREATERRWIN:
/**< 故障界面不存在则创建,存在则刷新告警 */
        err_window(pMsg->hWin);
        break;
    case MSG_DELERRWIN:
        /**< 故障界面存在则删除故障界面 */
        if (bittest(winCreateFlag, 0))
        {
            bitclr(winCreateFlag, 0);
            //WM_DeleteWindow(err_hItem);
            GUI_EndDialog(err_hItem, 0);
            err_hItem = 0;
        }
        break;
    case MSG_JUMPHOME:
        prePowerFee = 0;
        preServiceFee = 0;
        bitclr(winInitDone, 0);
        AdvertisementRecordFlag = 0;
       _deleteWin(_hWinAdvertizement);
        _hWinAdvertizement= 0;
        CreateHomePage();
        break;
    case MSG_JUMPCARDINFO:
        bitclr(winInitDone, 0);
        AdvertisementRecordFlag = 0;
        _deleteWin(_hWinAdvertizement);
        CreateCardInfoPage();
        break;
    case MSG_JUMPCHARGEDONE:
        bitclr(winInitDone, 0);
        AdvertisementRecordFlag = 0;
        _deleteWin(_hWinAdvertizement);
        _hWinAdvertizement = 0;
        CreateChargeDonePage();
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
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinAdvertizement), ID_TimerTime, 100, 0);
    cur_win = _hWinAdvertizement;
    bitset(winInitDone, 0);
    return 0;
}


