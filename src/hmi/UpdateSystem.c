#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_2 (GUI_ID_USER + 0x03)
#define ID_TEXT_3 (GUI_ID_USER + 0x04)
#define ID_TEXT_4 (GUI_ID_USER + 0x05)
#define ID_PROGBAR_0 (GUI_ID_USER + 0x06)


#define FontColor GUI_BLACK

#define ID_TimerUpdateSyatem             1

static WM_HTIMER    _timerUpdateSystem; //更新系统进度

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 200, 135, 400, 70, TEXT_CF_HCENTER, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_1, 200, 205, 400, 70, TEXT_CF_HCENTER, 0x0, 0 },
//    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_2, 200, 100, 400, 40, TEXT_CF_HCENTER, 0x0, 0 },
//    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_3, 200, 140, 400, 40, TEXT_CF_HCENTER, 0x0, 0 },
//    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_4, 200, 180, 400, 40, TEXT_CF_HCENTER, 0x0, 0 },
    { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 300, 275, 200, 40, 0, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN      hItem;
    uint8_t temp_buf[200] = { 0 };

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), FontColor);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), FontColor);
       // TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), FontColor);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &fontwryhcg30e);
       // TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &fontwryhcg30e);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
        PROGBAR_SetValue(hItem, 0);
        PROGBAR_SetMinMax(hItem, 0, 100);
        PROGBAR_SetFont(hItem, &fontwryhcg30e);
        break;
    case WM_NOTIFY_PARENT:
        break;
    case WM_PAINT:
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerUpdateSystem)
        { 
            if (pechProto->info.ftp.ucDownloadStart == 0)//系统升级完成或者失败
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            extern EchFtpCfg_t ftpcfg;
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), ftpcfg.strNewFileName); 
            sprintf(temp_buf, "download %d/%d(%d%%)", pechProto->info.ftp.ftp_proc.ulRecvFileSize, ftpcfg.fsize, pechProto->info.ftp.ftp_proc.precent);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), temp_buf); 
//            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), ftpcfg.); 
//            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), ftpcfg.fsize); 
//            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), temp_buf); 
            PROGBAR_SetValue(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0), pechProto->info.ftp.ftp_proc.precent); 
            WM_RestartTimer(pMsg->Data.v, 300);
        }
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

WM_HWIN CreateUpdateSystem(void);
WM_HWIN CreateUpdateSystem(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerUpdateSystem = WM_CreateTimer(hWin, ID_TimerUpdateSyatem, 20, 0);
    return 0;
}

