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
#include "HMI_Start.h"

#define BYTES_LEN 1024

#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_CHECKBOX_0   (GUI_ID_USER + 0x01)
#define ID_CHECKBOX_1   (GUI_ID_USER + 0x02)
#define ID_CHECKBOX_2   (GUI_ID_USER + 0x03)
#define ID_LISTVIEW_0   (GUI_ID_USER + 0x04)
#define ID_LISTVIEW_1   (GUI_ID_USER + 0x05)
#define ID_LISTVIEW_2   (GUI_ID_USER + 0x06)

#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3

#define ALARM_COLUMNS   4
#define CHARGE_COLUMNS  20
#define DB_DEBUG    0

#define analogName "模拟量"
#define analogValue "模拟量值"
#define analogACVol "交流电压"
#define analogACCurrent "交流电流"
#define analogCPVol "充电导引电压"
#define analogACFreq "频率"
#define analogACTempL "市电A(L)相温度"
#define analogACTempN "市电N相温度"
#define analogOutTempL "A(L)相温度"
#define analogOutTempN "N相温度"

static WM_HTIMER _timerRTC, _timerData, _timerSignal;
uint16_t column_num, row_num;
WM_HWIN _hWinManagerInfoAnalog;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { WINDOW_CreateIndirect, "window", ID_WINDOW_0, 0, 95, 800, 465, 0, 0x0, 0 },
//    { CHECKBOX_CreateIndirect, "Checkbox_pile", ID_CHECKBOX_0, 15, 110, 100, 40, 0, 0x0, 0 },
//    { CHECKBOX_CreateIndirect, "Checkbox_gun1", ID_CHECKBOX_1, 15, 170, 100, 40, 0, 0x0, 0 },
//    { CHECKBOX_CreateIndirect, "Checkbox_gun2", ID_CHECKBOX_2, 15, 230, 100, 40, 0, 0x0, 0 },
//    { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 115, 10, 670, 550, 0, 0x0, 0 },//560,276
//    { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 115, 10, 670, 550, 0, 0x0, 0 },//560,276
//    { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 115, 10, 670, 550, 0, 0x0, 0 },//560,276
};

static void Status_Content_Analy(WM_MESSAGE *pMsg)
{
    CON_t *pCon;
    uint8_t strTmp[30];
    WM_HWIN hItem;

    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    pCon = CONGetHandle(0);

    sprintf(strTmp, "%.1f", pCon->status.dChargingVoltage);
    strcat(strTmp, "V");
    LISTVIEW_SetItemText(hItem, 1, 0, strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dChargingCurrent);
    strcat(strTmp, "A");
    LISTVIEW_SetItemText(hItem, 3, 0, strTmp);

    /**< 控制导引电压确认 */
    sprintf(strTmp, "%.1f", Sys_samp.DC.CP1);
    strcat(strTmp, "V");
    LISTVIEW_SetItemText(hItem, 1, 1, strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dChargingFrequence);
    strcat(strTmp, "Hz");
    LISTVIEW_SetItemText(hItem, 3, 1, strTmp);

    sprintf(strTmp, "%.1f", pEVSE->status.dAC_A_Temp_IN);
    strcat(strTmp, "℃");
    LISTVIEW_SetItemText(hItem, 1, 2, strTmp);

    sprintf(strTmp, "%.1f", pEVSE->status.dAC_N_Temp_IN);
    strcat(strTmp, "℃");
    LISTVIEW_SetItemText(hItem, 3, 2, strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dACLTemp);//acl or acn
    strcat(strTmp, "℃");
    LISTVIEW_SetItemText(hItem, 1, 3, strTmp);

    sprintf(strTmp, "%.1f", pCon->status.dACNTemp);
    strcat(strTmp, "℃");
    LISTVIEW_SetItemText(hItem, 3, 3, strTmp);
}

static void RADIO3(WM_HWIN hItem1, WM_HWIN hItem2, WM_HWIN hItem3)
{
    CHECKBOX_SetState(hItem1, 1);
    CHECKBOX_SetState(hItem2, 0);
    CHECKBOX_SetState(hItem3, 0);
}

static void RADIO2(WM_HWIN hItem1, WM_HWIN hItem2)
{
    CHECKBOX_SetState(hItem1, 1);
    CHECKBOX_SetState(hItem2, 0);
}

static void _cbDialog(WM_MESSAGE *pMsg)
{
    const void *pData;
    WM_HWIN      hItem;
    const char *strdouble[3] = { "pile", "gunA", "gunB" };
    const char *strsingle[2] = { "pile", "gun" };
    HEADER_SKINFLEX_PROPS head_skin;
    U32          FileSize;
    int          NCode;
    int          Id;
    uint16_t     i, _strNum[3];
    volatile HEADER_Handle hHeader;
    char    buf[20];
    SCROLLBAR_Handle hScroll;
    SCROLLBAR_Handle wScroll;
    CON_t	*pcont;
    int x, y, x_size, y_size;
    
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        GUI_SetColor(GUI_BLACK);
        if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0)) == 1)
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
            x = WM_GetWindowOrgX(hItem) - WM_GetWindowOrgX(WM_GetParent(hItem));
            x_size = WM_GetWindowSizeX(hItem);
            y = WM_GetWindowOrgY(hItem) - WM_GetWindowOrgY(WM_GetParent(hItem));
            y_size = WM_GetWindowSizeY(hItem);
            GUI_FillRect(x + x_size-4, y, x + 2*x_size, y + y_size-1);
        }
        if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1)) == 1)
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
            x = WM_GetWindowOrgX(hItem) - WM_GetWindowOrgX(WM_GetParent(hItem)) ;
            x_size = WM_GetWindowSizeX(hItem);
            y = WM_GetWindowOrgY(hItem) - WM_GetWindowOrgY(WM_GetParent(hItem));
            y_size = WM_GetWindowSizeY(hItem);
            GUI_FillRect(x + x_size-4, y, x + 2*x_size, y + y_size-1);
        }
        if (pEVSE->info.ucTotalCON == 2)
        {
            if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2)) == 1)
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
                x = WM_GetWindowOrgX(hItem) - WM_GetWindowOrgX(WM_GetParent(hItem));
                x_size = WM_GetWindowSizeX(hItem);
                y = WM_GetWindowOrgY(hItem) - WM_GetWindowOrgY(WM_GetParent(hItem));
                y_size = WM_GetWindowSizeY(hItem);
                GUI_FillRect(x + x_size-4, y, x + 2*x_size, y + y_size-1);
            }
        }
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break; 
    case WM_INIT_DIALOG:
        head_skin.aColorFrame[0] = 0xAAAAAA;
        head_skin.aColorFrame[1] = GUI_BLACK;
        head_skin.aColorLower[0] = GUI_BLACK;
        head_skin.aColorLower[1] = GUI_BLACK;
        head_skin.aColorUpper[0] = GUI_BLACK;
        head_skin.aColorUpper[1] = GUI_BLACK;
        head_skin.ColorArrow = GUI_BLACK;
        HEADER_SetSkinFlexProps(&head_skin, 0);
        HEADER_SetDefaultTextColor(GUI_WHITE);
        HEADER_SetDefaultBorderV(50);
        //LISTVIEW_SetDefaultGridColor(GUI_BLACK);
        if (pEVSE->info.ucTotalCON == 1)
        {
            for (i = 0; i < 2; i++)
            {
                hItem = LISTVIEW_CreateEx(125, 20,400,320, pMsg->hWin, WM_CF_SHOW,0,
                    ID_LISTVIEW_0 + i);
                WM_HideWindow(hItem);
                hItem = CHECKBOX_CreateUser(15,
                    60 + i * 60,
                    100,
                    40,
                    pMsg->hWin,
                    WM_CF_SHOW,
                    0,
                    ID_CHECKBOX_0 + i,
                    10);
                CHECKBOX_SetUserData(hItem, strsingle[i], 10);
                CHECKBOX_SetSkin(hItem, SKIN_checkboxStateInfo);
            }
        }
        else if (pEVSE->info.ucTotalCON == 2)
        {
            for (i = 0; i < 3; i++)
            {
                hItem = LISTVIEW_CreateEx(125,
                    20,
                    400,
                    320,
                    pMsg->hWin,
                    WM_CF_SHOW,
                    0,
                    ID_LISTVIEW_0 + i);
                WM_HideWindow(hItem);
                hItem = CHECKBOX_CreateUser(15,
                    60 + i * 60,
                    100,
                    40,
                    pMsg->hWin,
                    WM_CF_SHOW,
                    0,
                    ID_CHECKBOX_0 + i,
                    10);
                CHECKBOX_SetUserData(hItem, strdouble[i], 10);
                CHECKBOX_SetSkin(hItem, SKIN_checkboxStateInfo);
            }
        }
        WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0));
        hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
        //LISTVIEW_SetHeaderHeight(hItem, 100);
        hHeader = LISTVIEW_GetHeader(hItem);
        //HEADER_SetHeight(hHeader, 50);
        HEADER_SetFont(hHeader, &fontwryhcg30e);
        HEADER_SetHeight(hHeader, 40);
        HEADER_SetDragLimit(hHeader, 0);
        //HEADER_SetTextColor()
        
        LISTVIEW_SetFont(hItem, &SIF16_Font);
        LISTVIEW_SetRowHeight(hItem, 35);
        LISTVIEW_SetGridVis(hItem, 1);

        /*模拟量   模拟值     模拟量     模拟值*/
        LISTVIEW_AddColumn(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0), 200, "充电桩监测项", GUI_TA_HCENTER | GUI_TA_VCENTER);
        LISTVIEW_AddColumn(WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0), 200, "项目状态", GUI_TA_HCENTER | GUI_TA_VCENTER);
        
        LISTVIEW_AddRow(hItem, NULL);//增加一行
        LISTVIEW_SetItemText(hItem, 0, 0, "急停状态");

        LISTVIEW_AddRow(hItem, NULL);//增加一行
        LISTVIEW_SetItemText(hItem, 0, 1, "刷卡板状态");

        LISTVIEW_AddRow(hItem, NULL);//增加一行
        LISTVIEW_SetItemText(hItem, 0, 2, "防雷器状态");
        
        LISTVIEW_AddRow(hItem, NULL);//增加一行
        LISTVIEW_SetItemText(hItem, 0, 3, "市电输入L温度");
        
        LISTVIEW_AddRow(hItem, NULL);//增加一行
        LISTVIEW_SetItemText(hItem, 0, 4, "市电输入N温度");
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_CHECKBOX_0:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                if (pEVSE->info.ucTotalCON == 1)
                {
                    RADIO2(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1));     
                }
                else
                { 
                    RADIO3(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2));
                }                        
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        case ID_CHECKBOX_1:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                if (pEVSE->info.ucTotalCON == 1)
                {
                    RADIO2(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0));     
                }
                else
                {
                    RADIO3(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2)); 
                }             
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        case ID_CHECKBOX_2:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                RADIO3(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2),\
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0));              
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        }
    case WM_TIMER:
        if (pMsg->Data.v == _timerData)
        {
            //Status_Content_Analy(pMsg);
            WM_RestartTimer(pMsg->Data.v, 1000);
        }
        break;
    case MSG_DELETEMANAGERWIN:
        GUI_EndDialog(_hWinManagerInfoAnalog, 0);
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
 *       CreateManagerInfoAnalog
*/
WM_HWIN CreateManagerInfoAnalog(WM_HWIN srcHwin)
{
    _hWinManagerInfoAnalog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_GetClientWindow(srcHwin), 0, 0);
//    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerTime, 20, 0);
    _timerData = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerFlush, 1000, 0);
//    _timerSignal = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerSignal,5000,0);
    return _hWinManagerInfoAnalog;
}
/*************************** End of file ****************************/









