#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_2 (GUI_ID_USER + 0x07)
#define ID_TEXT_3 (GUI_ID_USER + 0x08)
#define ID_BUTTON_0 (GUI_ID_USER + 0x0A)

#define ID_promptTime    1

static WM_HTIMER _timerprompt;

typedef enum promptEnum
{
    promptGetInfoState = 1,    
    promptUnavailableState,
    promptReadyChargingState,
    promptPleasePlugState,
    promptstartfailState,
}PROMPTSTATE_E;

PROMPTSTATE_E promptstate;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "CardNumber", ID_TEXT_2, 326, 165, 280, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Balance", ID_TEXT_3, 326, 223, 280, 24, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_0, 50, 350, 170, 70, 0, 0x0, 0 },
};

static void showprompt()
{
//    switch (promptstate)
//    {
//    case promptGetInfoState:
//        GUI_MEMDEV_WriteAt(Memdevcardinfoget, 240, 316);
//        break;
//    case promptUnavailableState:
//        GUI_MEMDEV_WriteAt(MemdevcardinfoUnavailable, 240, 316);
//        break;
//    case promptReadyChargingState:
//        GUI_MEMDEV_WriteAt(Memdevcardinforeadycharging, 240, 316);
//        break;
//    case promptPleasePlugState:
//        GUI_MEMDEV_WriteAt(Memdevcardinfoplug, 240, 316);
//        break;
//    case promptstartfailState:
//        GUI_MEMDEV_WriteAt(Memdevcardinfostartfail, 240, 316);
//        break;
//    }
}

static void transformprompt(PROMPTSTATE_E state)
{
    switch (state)
    {
    case promptGetInfoState:
//        if (gbsstate != StateTestChargeCondition)
//        {
//            
//        }
        break;
    case promptUnavailableState:
        
        break;
    case promptReadyChargingState:
       
        break;
    case promptPleasePlugState:
       
        break; 
    case promptstartfailState:

        break;
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN      hItem;

    CON_t *pCON;
    pCON = CONGetHandle(0);//选择枪的时候获取pCON
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLACK, "1234567890");//卡号
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_BLACK, "?");        
        promptstate = promptGetInfoState;
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem ,SKIN_buttonquit);
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
        GUI_MEMDEV_WriteAt(Memdevcardinfoback, 0, 0);
        showprompt();
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerprompt)
        { 
//            if (gbsstate == StateTestChargeCondition)
//            {
//                
//            }
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
    _timerprompt = WM_CreateTimer(hWin, ID_promptTime, 20, 0);
    return 0;
}