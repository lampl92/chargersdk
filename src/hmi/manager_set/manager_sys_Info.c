#include "touchtimer.h"
#include "DIALOG.h"
#include "includes.h"
#include "order.h"
#include "interface.h"
#include "utils.h"
#include "cJSON.h"
#include "evse_config.h"
#include "cfg_parse.h"
#include "stringName.h"
#include "errorcode.h"
#include <string.h>

#define BYTES_LEN 1024

#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x01)
#define ID_TEXT_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_2 (GUI_ID_USER + 0x03)
#define ID_TEXT_3 (GUI_ID_USER + 0x04)
#define ID_TEXT_4 (GUI_ID_USER + 0x05)
#define ID_TEXT_5 (GUI_ID_USER + 0x06)

#define ID_TimerTime    1

#define sysInfoName "       系统信息"
#define sysInfoEVSEName "系统名称:"
#define sysInfoProtoVer "协议版本:"
#define sysInfoVersion "软件版本:"

static WM_HTIMER _timerRTC;
WM_HWIN _hWinManagerSysInfo;
/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { WINDOW_CreateIndirect, "window", ID_WINDOW_0, 10, 95, 780, 370, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "system_version_text", ID_TEXT_0, 100, 70, 180, 40, TEXT_CF_RIGHT, 0x0, 0 },
    { TEXT_CreateIndirect, "system_version_text", ID_TEXT_1, 100, 120, 180, 40, TEXT_CF_RIGHT, 0x0, 0 },
    { TEXT_CreateIndirect, "system_version_text", ID_TEXT_2, 100, 170, 180, 40, TEXT_CF_RIGHT, 0x0, 0 },
    { TEXT_CreateIndirect, "system_version_text", ID_TEXT_3, 280, 70, 240, 40, TEXT_CF_LEFT, 0x0, 0 },
    { TEXT_CreateIndirect, "system_version_text", ID_TEXT_4, 280, 120, 240, 40, TEXT_CF_LEFT, 0x0, 0 },
    { TEXT_CreateIndirect, "system_version_text", ID_TEXT_5, 280, 170, 240, 40, TEXT_CF_LEFT, 0x0, 0 },
};

static void _cbDialog(WM_MESSAGE *pMsg)
{
    const void *pData;
    volatile WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;
    uint16_t     i, _strNum[3];
    volatile HEADER_Handle hHeader;
    uint8_t    buf[50];
    uint8_t    tmp[50];
    CON_t	*pcont;
    GUI_RECT h;
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        break;
    case WM_INIT_DIALOG:
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_BLACK);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), GUI_BLACK);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), GUI_BLACK);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), GUI_BLACK);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), GUI_BLACK);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), GUI_BLACK);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), &fontwryhcg30e);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), "系统名称: ");
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), "协议版本: ");
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), "软件版本: ");
        if (pEVSE->info.ucTotalCON == 1)
        {
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), "单枪交流充电桩");
        }
        else
        {
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), "双枪交流充电桩");
        }
        memset(tmp,'\0',sizeof(tmp));
        sprintf(tmp,"%d",pechProto->info.ucProtoVer);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), tmp);        
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), xSysconf.strVersion);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {

            break;
        }
    case WM_TIMER:
        break;
    case MSG_CREATERRWIN:
        break;
    case MSG_DELERRWIN:
        break;
    case MSG_DELETEMANAGERWIN:
        GUI_EndDialog(_hWinManagerSysInfo, 0);
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/

/** @brief
 *  输出管理员界面里的信息状态量部分
 * @param
 * @param
 * @return
 *       CreateManagerSysInfo
*/
WM_HWIN CreateManagerSysInfo(WM_HWIN srcHwin)
{
    _hWinManagerSysInfo = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_GetClientWindow(srcHwin), 0, 0);
    return _hWinManagerSysInfo;
}
/*************************** End of file ****************************/










