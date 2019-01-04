#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)

#define ID_Timerstateflash    1

#define cartoonDotx   488
#define cartoonDoty   245

static WM_HTIMER _timerstateflash;

int cartoonFlag = 0 ;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 510, 7, 235, 30, TEXT_CF_HCENTER, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    char temp_buf[32] = { 0 };
    WM_HWIN hItem;
    int system_start_percent;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_WHITE);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg30e);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break;   
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevcardinfoback, 0, 0);
        GUI_MEMDEV_WriteAt(Memdevcardinfostartup, 120, 103);
        if (cartoonFlag < 0 || cartoonFlag > 3)
        {
            cartoonFlag = 0;
        }
        if (cartoonFlag == 0)
        {
            ;
        }
        if (cartoonFlag == 1)
        {
            GUI_MEMDEV_WriteAt(Memdevcardinfostartone, cartoonDotx, cartoonDoty);
        }
        else if (cartoonFlag == 2)
        {
            GUI_MEMDEV_WriteAt(Memdevcardinfostatrttwo, cartoonDotx, cartoonDoty);
        }
        else if (cartoonFlag == 3)
        {
            GUI_MEMDEV_WriteAt(Memdevcardinfostatrtthree, cartoonDotx, cartoonDoty);
        }
        cartoonFlag++;
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstateflash)
        { 
            WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            if (pechProto->info.ftp.ucDownloadStart == 1)//系统要升级
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPUpdateSystem);
            }
            extern int flag_read_data;
            if (flag_read_data == 1)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            extern int nums_picture;
            if (pEVSE->info.ucTotalCON == 1)
            {
                system_start_percent =(int)(100.0*( nums_picture-6.0)/49.0);
            }
            else
            {
                system_start_percent = (int)(100.0*( nums_picture-7.0) / 72.0);
            }
            sprintf(temp_buf, "%d%%", system_start_percent);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), temp_buf); 
            WM_RestartTimer(pMsg->Data.v, 300);
        }
        break;
    case MSG_JUMPUpdateSystem:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateUpdateSystem();
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        home();
        pEVSE->status.ulPicOK = 1;
        //NVIC_SystemReset();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreatestartUpDLG(void);
WM_HWIN CreatestartUpDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 100, 0);
    return hWin;
}

