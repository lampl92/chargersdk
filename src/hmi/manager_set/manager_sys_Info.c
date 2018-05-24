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

#define ID_TimerTime    1

#define sysInfoName "       系统信息"
#define sysInfoEVSEName "系统名称:7kW交流充电桩"
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
};

static void Status_Content_Analy(WM_MESSAGE *pMsg)
{
//    CON_t *pCon;
//    uint8_t tmp[10];
//    uint8_t buf[50];
//    WM_HWIN hItem;
//
//    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
//    /**< 协议版本 */
//    memset(buf,'\0',sizeof(buf));
//    strcpy(buf,sysInfoProtoVer);
//    memset(tmp,'\0',sizeof(tmp));
//    sprintf(tmp,"%d",pechProto->info.ucProtoVer);
//    strcat(buf,tmp);
//    LISTVIEW_SetItemText(hItem, 0, 1, buf);
//    LISTVIEW_AddRow(hItem, NULL);//增加一行
//    /**< 软件版本 */
//    memset(buf,'\0',sizeof(buf));
//    strcpy(buf,sysInfoVersion);
//    strcat(buf,xSysconf.strVersion);
//    LISTVIEW_SetItemText(hItem, 0, 2, buf);
}

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
    uint8_t    tmp[10];
    CON_t	*pcont;

    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        GUI_SetColor(GUI_WHITE);
        GUI_FillRect(195, 85, 390, 200);
        GUI_SetColor(GUI_BLACK);
        GUI_FillRoundedRect(195, 85, 390, 200,10);
        GUI_SetColor(GUI_WHITE);
        GUI_FillRect(10, 50, 390 - 10, 200 - 10);
        break;
    case WM_INIT_DIALOG:
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
//        /* 设置列表控件中header控件的所显示文本的字体 */
//        hHeader = LISTVIEW_GetHeader(hItem);
//        HEADER_SetFont(hHeader, &SIF36_Font);
//
//        LISTVIEW_SetFont(hItem, &SIF24_Font);
//        /* 设置列表控件表格可见 */
//        LISTVIEW_SetGridVis(hItem, 1);
//
//        column_num = LISTVIEW_GetNumColumns(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
//        for(i = 0;i < column_num;i++)
//        {
//            LISTVIEW_DeleteColumn(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0),0);
//        }
//
//        /*增加一列*/
//        LISTVIEW_AddColumn(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0), GUI_MANAGERSYSINFO_XLENTH, sysInfoName, GUI_TA_VCENTER | GUI_TA_LEFT);
////        LISTVIEW_SetTextAlign(hItem, 1, GUI_TA_CENTER | GUI_TA_LEFT);
//
//        LISTVIEW_AddRow(hItem, NULL);//增加一行
//        /**< 7kW交流充电桩 */
//        xSysconf.GetSysCfg((void *)&xSysconf, NULL);
//        LISTVIEW_SetItemText(hItem, 0, 0, sysInfoEVSEName);
//        LISTVIEW_AddRow(hItem, NULL);//增加一行
//        /**< 协议版本 */
//        memset(buf,'\0',sizeof(buf));
//        strcpy(buf,sysInfoProtoVer);
//        memset(tmp,'\0',sizeof(tmp));
//        sprintf(tmp,"%d",pechProto->info.ucProtoVer);
//        strcat(buf,tmp);
//        LISTVIEW_SetItemText(hItem, 0, 1, buf);
//        LISTVIEW_AddRow(hItem, NULL);//增加一行
//        /**< 软件版本 */
//        memset(buf,'\0',sizeof(buf));
//        strcpy(buf,sysInfoVersion);
//        strcat(buf,xSysconf.strVersion);
//        LISTVIEW_SetItemText(hItem, 0, 2, buf);

        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {

            break;
        }
    case WM_TIMER:
//        if(pMsg->Data.v == _timerRTC)
//        {
//            /**< 显示时间和日期 */
//            Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);
//           // TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), strCSQ);
//            /**< 重启定时器 */
//            WM_RestartTimer(pMsg->Data.v, 20);
//        }
//        if(pMsg->Data.v == _timerSignal)
//        {
//
//            WM_RestartTimer(pMsg->Data.v, 2000);
//        }
        if (pMsg->Data.v == _timerRTC)
        {
            Status_Content_Analy(pMsg);
            WM_RestartTimer(pMsg->Data.v, 3000);
        }
        break;
    case MSG_CREATERRWIN:
        /**< 故障界面不存在则创建,存在则刷新告警 */
        err_window(pMsg->hWin);
        break;
    case MSG_DELERRWIN:
        /**< 故障界面存在则删除故障界面 */
        if (bittest(winCreateFlag, 0))
        {
            bitclr(winCreateFlag, 0);
            GUI_EndDialog(err_hItem, 0);
            err_hItem = 0;
        }
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
//    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinManagerSysInfo), ID_TimerTime, 20, 0);
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinManagerSysInfo), ID_TimerTime, 1000, 0);
//    _timerSignal = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerSignal,5000,0);
    return _hWinManagerSysInfo;
}
/*************************** End of file ****************************/










