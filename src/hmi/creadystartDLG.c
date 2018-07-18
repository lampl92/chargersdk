#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
//#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)

#define ID_Timerstateflash    1

static WM_HTIMER _timerstateflash;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
//    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 40, 120, 400, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
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
//                CreatepleaseplugDLG();
//                break;
//            }
//            break;
//        }
//        break;
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevcardinfoback,0,0);
        GUI_MEMDEV_WriteAt(Memdevcardinforeadystart, 120, 103);
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstateflash)
        { 
            if (gbsstate == StateNetTimeout)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPNetTimeout);
            }
            if (gbsstate == StatePwd)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPStatePwd);
            }
            if (gbsstate == StateCardconditionNotOk)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPCardconditionNotOk);
            }
            if (gbsstate == StatePleasePlug)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPPleasePlug);
            }
            if (gbsstate == StateChargingOk)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPChargingOk);
            }
            if (gbsstate == StateCantChargeOfDevice)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPStateEquipmentFailureNoStart);
            }
            if (gbsstate == StateHome)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            WM_RestartTimer(pMsg->Data.v,100);
        }
        break;
    case MSG_JUMPStatePwd:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatePwdInput();
        break;
    case MSG_JUMPNetTimeout:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatenettimeoutDLG();
        break;
    case MSG_JUMPCardconditionNotOk:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatecardconditionnotokDLG();
        break;
    case MSG_JUMPPleasePlug:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatepleaseplugDLG();
        break;
    case MSG_JUMPChargingOk:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatechargingokDLG();
        break;
    case MSG_JUMPStateEquipmentFailureNoStart:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateEquipmentFailureNoStartDLG();
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        home();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreatereadystartDLG(void);
WM_HWIN CreatereadystartDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 100, 0);
    return 0;
}

