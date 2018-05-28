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

#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_CHECKBOX_0   (GUI_ID_USER + 0x01)
#define ID_CHECKBOX_1   (GUI_ID_USER + 0x02)
#define ID_CHECKBOX_2   (GUI_ID_USER + 0x03)
#define ID_WINDOW_1   (GUI_ID_USER + 0x04)
#define ID_WINDOW_2   (GUI_ID_USER + 0x05)
#define ID_WINDOW_3   (GUI_ID_USER + 0x06)

#define ID_TimerData    1
#define ID_TimerPile    2
#define ID_TimerGunA    3
#define ID_TimerGunB    4

#define ALARM_COLUMNS   4
#define CHARGE_COLUMNS  20
#define DB_DEBUG    0

#define JUNCTION_LINE_WIDTH  10

WM_HWIN      hItem_pile, hItem_gunA, hItem_gunB;

static WM_HTIMER _timerData, _timerPile,_timerGunA,_timerGunB;

uint16_t column_num, row_num;
WM_HWIN _hWinManagerInfoAnalog;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { WINDOW_CreateIndirect, "window", ID_WINDOW_0, 10, 95, 780, 370, 0, 0x0, 0 },
};

static void Status_Content_Analy(WM_MESSAGE *pMsg)
{
    CON_t *pCon;
    uint8_t strTmp[30];
    WM_HWIN hItem;
    
 //   hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Scram) == defSignalEVSE_Alarm_Scram)
    {
        LISTVIEW_SetItemBkColor(hItem, 1, 0, LISTVIEW_CI_UNSEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 0, LISTVIEW_CI_SEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 0, LISTVIEW_CI_SELFOCUS, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 0, LISTVIEW_CI_DISABLED, GUI_RED);
        LISTVIEW_SetItemText(hItem, 1, 0, "按下");//"按下");
    }
    else
    {
        LISTVIEW_SetItemText(hItem, 1, 0, "未按下");//"正常");
    }
    if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)
    {
        LISTVIEW_SetItemBkColor(hItem, 1, 1, LISTVIEW_CI_UNSEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 1, LISTVIEW_CI_SEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 1, LISTVIEW_CI_SELFOCUS, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 1, LISTVIEW_CI_DISABLED, GUI_RED);
        LISTVIEW_SetItemText(hItem, 1, 1, "发生故障");//"故障");
    }
    else
    {
        LISTVIEW_SetItemText(hItem, 1, 1, "正常");//"正常");
    }
       /**< 防雷器状态 */
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Arrester) == defSignalEVSE_Alarm_Arrester)
    {
        LISTVIEW_SetItemBkColor(hItem, 1, 2, LISTVIEW_CI_UNSEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 2, LISTVIEW_CI_SEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 2, LISTVIEW_CI_SELFOCUS, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 2, LISTVIEW_CI_DISABLED, GUI_RED);
        LISTVIEW_SetItemText(hItem, 1, 2, "发生故障");//"故障");
    }
    else
    {
        LISTVIEW_SetItemText(hItem, 1, 2, "正常");//"正常");
    }

    sprintf(strTmp, "%.1f", pEVSE->status.dAC_A_Temp_IN);
    strcat(strTmp, "℃");
    if ((pEVSE->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_Temp_Cri) == defSignalCON_Alarm_AC_A_Temp_Cri)
    {
        LISTVIEW_SetItemBkColor(hItem, 1, 3, LISTVIEW_CI_UNSEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 3, LISTVIEW_CI_SEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 3, LISTVIEW_CI_SELFOCUS, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 3, LISTVIEW_CI_DISABLED, GUI_RED);
        strcat(strTmp, "--超限");
    }
    else 
    {
        strcat(strTmp, "--正常");
    }
    LISTVIEW_SetItemText(hItem, 1, 3, strTmp);

    sprintf(strTmp, "%.1f", pEVSE->status.dAC_N_Temp_IN);
    strcat(strTmp, "℃");
    if ((pEVSE->status.ulSignalAlarm & defSignalCON_Alarm_AC_N_Temp_Cri) == defSignalCON_Alarm_AC_N_Temp_Cri)
    {
        LISTVIEW_SetItemBkColor(hItem, 1, 4, LISTVIEW_CI_UNSEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 4, LISTVIEW_CI_SEL, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 4, LISTVIEW_CI_SELFOCUS, GUI_RED);
        LISTVIEW_SetItemBkColor(hItem, 1, 4, LISTVIEW_CI_DISABLED, GUI_RED);
        strcat(strTmp, "--超限");
    }
    else
    {
        strcat(strTmp, "--正常");
    }
    LISTVIEW_SetItemText(hItem, 1, 4, strTmp);
    
    
//    pCon = CONGetHandle(0);
//
//    sprintf(strTmp, "%.1f", pCon->status.dChargingVoltage);
//    strcat(strTmp, "V");
//    LISTVIEW_SetItemText(hItem, 1, 0, strTmp);
//
//    sprintf(strTmp, "%.1f", pCon->status.dChargingCurrent);
//    strcat(strTmp, "A");
//    LISTVIEW_SetItemText(hItem, 3, 0, strTmp);
//
//    /**< 控制导引电压确认 */
//    sprintf(strTmp, "%.1f", Sys_samp.DC.CP1);
//    strcat(strTmp, "V");
//    LISTVIEW_SetItemText(hItem, 1, 1, strTmp);
//
//    sprintf(strTmp, "%.1f", pCon->status.dChargingFrequence);
//    strcat(strTmp, "Hz");
//    LISTVIEW_SetItemText(hItem, 3, 1, strTmp);
//
//
//
//    sprintf(strTmp, "%.1f", pCon->status.dACLTemp);//acl or acn
//    strcat(strTmp, "℃");
//    LISTVIEW_SetItemText(hItem, 1, 3, strTmp);
//
//    sprintf(strTmp, "%.1f", pCon->status.dACNTemp);
//    strcat(strTmp, "℃");
//    LISTVIEW_SetItemText(hItem, 3, 3, strTmp);
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

static void get_position(WM_HWIN hItem, int *x, int *x_size, int *y, int *y_size)
{
    *x = WM_GetWindowOrgX(hItem) - WM_GetWindowOrgX(WM_GetParent(hItem));
    *x_size = WM_GetWindowSizeX(hItem);
    *y = WM_GetWindowOrgY(hItem) - WM_GetWindowOrgY(WM_GetParent(hItem));
    *y_size = WM_GetWindowSizeY(hItem);
}

static void window_show_hide3(WM_MESSAGE *pMsg)
{
    if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0)) == 1)
    {
        WM_ShowWindow(hItem_pile);
        WM_HideWindow(hItem_gunA);
        WM_HideWindow(hItem_gunB);
    }
    if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1)) == 1)
    {
        WM_ShowWindow(hItem_gunA);
        WM_HideWindow(hItem_pile);
        WM_HideWindow(hItem_gunB);
    }
    if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2)) == 1)
    {
        WM_ShowWindow(hItem_gunB);
        WM_HideWindow(hItem_gunA);
        WM_HideWindow(hItem_pile);
    }
}

static void window_show_hide2(WM_MESSAGE *pMsg)
{
    if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0)) == 1)
    {
        WM_ShowWindow(hItem_pile);
        WM_HideWindow(hItem_gunA);
    }
    if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1)) == 1)
    {
        WM_ShowWindow(hItem_gunA);
        WM_HideWindow(hItem_pile);
    }
}

static void pile_draw_grids(WM_MESSAGE *pMsg)
{
    GUI_RECT rect_str;
    int x, x_size, y, y_size;
    int high = 56;
    int head_high = 60;
    GUI_SetBkColor(GUI_WHITE);
    get_position(pMsg->hWin, &x, &x_size, &y, &y_size);
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(0, 0, x_size, y_size);
    GUI_SetColor(GUI_BLACK);
    GUI_FillRoundedRect(0, 0, x_size, y_size, 10);
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(10, head_high, x_size - 10, y_size - 10);
    GUI_SetColor(0xAAAAAA);
    GUI_DrawGradientV(x_size / 2, 10, x_size / 2, head_high, GUI_BLACK, 0xAAAAAA);
    
    GUI_DrawVLine(10, head_high, y_size - 10);
    GUI_DrawVLine(x_size / 2, head_high, y_size - 10);
    GUI_DrawVLine(x_size - 10, head_high, y_size - 10);
    
    GUI_DrawHLine(head_high, 10, x_size - 10);

    GUI_DrawHLine(head_high + high - 1, 10, x_size - 10);
    GUI_DrawHLine(head_high + high, 10, x_size - 10);

    GUI_DrawHLine(head_high + high * 2 - 1, 10, x_size - 10);
    GUI_DrawHLine(head_high + high * 2, 10, x_size - 10);

    GUI_DrawHLine(head_high + high * 3 - 1, 10, x_size - 10);
    GUI_DrawHLine(head_high + high * 3, 10, x_size - 10);

    GUI_DrawHLine(head_high + high * 4 - 1, 10, x_size - 10);
    GUI_DrawHLine(head_high + high * 4, 10, x_size - 10);

    GUI_DrawHLine(head_high + high * 5, 10, x_size - 10); 
    
    GUI_SetColor(GUI_WHITE);
    rect_str.x0 = 10;
    rect_str.y0 = 0;
    rect_str.x1 = x_size / 2;
    rect_str.y1 = head_high;
    GUI_SetFont(&fontwryhcg30e);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("充电桩监测项", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);   
    rect_str.x0 = x_size / 2;
    rect_str.y0 = 0;
    rect_str.x1 = x_size;
    rect_str.y1 = head_high;
    GUI_SetFont(&fontwryhcg30e);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("项目状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER); 
}

static void pile_draw_data(WM_MESSAGE *pMsg)
{
    GUI_RECT rect_str;
    int x, x_size, y, y_size;
    int high = 56;
    int head_high = 60;
    uint8_t strTmp[30];
    get_position(pMsg->hWin, &x, &x_size, &y, &y_size);
    GUI_SetFont(&SIF16_Font);
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
    //急停
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Scram) == defSignalEVSE_Alarm_Scram)
    {
        GUI_SetColor(GUI_RED);
        rect_str.x0 = 10;
        rect_str.y0 = head_high;
        rect_str.x1 = x_size / 2 / 3;
        rect_str.y1 = head_high + high;
        GUI_DispStringInRect("!!", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = 10;
        rect_str.y0 = head_high;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high;
        GUI_DispStringInRect("急停状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high;
        GUI_DispStringInRect("按下", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_SetColor(GUI_BLACK);
    }
    else
    {   
        rect_str.x0 = 10;
        rect_str.y0 = head_high;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high;
        GUI_DispStringInRect("急停状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high;
        GUI_DispStringInRect("未按下", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    //刷卡板
    if ((pEVSE->status.ulSignalFault & defSignalEVSE_Fault_RFID) == defSignalEVSE_Fault_RFID)
    {
        GUI_SetColor(GUI_RED);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high;
        rect_str.x1 = x_size / 2 / 3;
        rect_str.y1 = head_high + high * 2;
        GUI_DispStringInRect("!!", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 2;
        GUI_DispStringInRect("刷卡板状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 2;
        GUI_DispStringInRect("发生故障", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 2;
        GUI_DispStringInRect("刷卡板状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 2;
        GUI_DispStringInRect("正常", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    // 防雷器
    if ((pEVSE->status.ulSignalAlarm & defSignalEVSE_Alarm_Arrester) == defSignalEVSE_Alarm_Arrester)
    {
        GUI_SetColor(GUI_RED);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 2;
        rect_str.x1 = x_size / 2 / 3;
        rect_str.y1 = head_high + high * 3;
        GUI_DispStringInRect("!!", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 2;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 3;
        GUI_DispStringInRect("防雷器状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high * 2;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 3;
        GUI_DispStringInRect("发生故障", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 2;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 3;
        GUI_DispStringInRect("防雷器状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high * 2;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 3;
        GUI_DispStringInRect("正常", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    //市电A项温度
    sprintf(strTmp, "%.1f", pEVSE->status.dAC_A_Temp_IN);
    strcat(strTmp, "℃");
    if ((pEVSE->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_Temp_Cri) == defSignalCON_Alarm_AC_A_Temp_Cri)
    {
        GUI_SetColor(GUI_RED);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 3;
        rect_str.x1 = x_size / 2 / 3;
        rect_str.y1 = head_high + high * 4;
        GUI_DispStringInRect("!!", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 3;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 4;
        GUI_DispStringInRect("市电输入A温度", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high * 3;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 4;
        strcat(strTmp, "--超限");
        GUI_DispStringInRect(strTmp, &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_SetColor(GUI_BLACK);
    }
    else 
    {
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 3;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 4;
        GUI_DispStringInRect("市电输入A温度", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high * 3;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 4;
        strcat(strTmp, "--正常");
        GUI_DispStringInRect(strTmp, &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    //市电N项温度
    sprintf(strTmp, "%.1f", pEVSE->status.dAC_N_Temp_IN);
    strcat(strTmp, "℃");
    if ((pEVSE->status.ulSignalAlarm & defSignalCON_Alarm_AC_N_Temp_Cri) == defSignalCON_Alarm_AC_N_Temp_Cri)
    {
        GUI_SetColor(GUI_RED);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 4;
        rect_str.x1 = x_size / 2 / 3;
        rect_str.y1 = head_high + high * 5;
        GUI_DispStringInRect("!!", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 4;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 5;
        GUI_DispStringInRect("市电输入N温度", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high * 4;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 5;
        strcat(strTmp, "--超限");
        GUI_DispStringInRect(strTmp, &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_SetColor(GUI_BLACK);
    }
    else
    {        
        rect_str.x0 = 10;
        rect_str.y0 = head_high + high * 4;
        rect_str.x1 = x_size / 2;
        rect_str.y1 = head_high + high * 5;
        GUI_DispStringInRect("市电输入N温度", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
        rect_str.x0 = x_size / 2;
        rect_str.y0 = head_high + high * 4;
        rect_str.x1 = x_size - 10;
        rect_str.y1 = head_high + high * 5;
        strcat(strTmp, "--正常");
        GUI_DispStringInRect(strTmp, &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
}

static void _cbDialog_pile(WM_MESSAGE *pMsg)
{
    switch (pMsg->MsgId)
    {
    case WM_PAINT:        
        pile_draw_grids(pMsg);
        pile_draw_data(pMsg);        
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        WM_Paint(pMsg->hWin);
        break; 
    case WM_INIT_DIALOG:
        break;
    case WM_NOTIFY_PARENT:
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerPile)
        {
            WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            WM_RestartTimer(pMsg->Data.v, 20);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void gunA_B_draw_grids(WM_MESSAGE *pMsg)
{
    GUI_RECT rect_str;
    int x, x_size, y, y_size;
    int high = 41;
    int width_part = 8;
    int head_high = 53;
    
    int width_part_big;
    GUI_SetBkColor(GUI_WHITE);
    get_position(pMsg->hWin, &x, &x_size, &y, &y_size);
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(0, 0, x_size, y_size);
    GUI_SetColor(GUI_BLACK);
    GUI_FillRoundedRect(0, 0, x_size, y_size, 10);
    GUI_SetColor(GUI_WHITE);
    GUI_FillRect(10, head_high, x_size - 10, y_size - 10);

    GUI_SetColor(0xAAAAAA);
    width_part_big = (x_size-20-2*width_part)/3;
    GUI_DrawGradientV(10 + width_part_big / 2, 10, 10 + width_part_big / 2, head_high, GUI_BLACK, 0xAAAAAA);
    GUI_DrawVLine(10 + width_part_big / 2, head_high, y_size - 10);
    
    GUI_DrawGradientV(10 + width_part_big / 2 + width_part_big + width_part, 10, 10 + width_part_big / 2 + width_part_big + width_part, head_high, GUI_BLACK, 0xAAAAAA);
    GUI_DrawVLine(10 + width_part_big / 2 + width_part_big + width_part, head_high, y_size - 10);
    
    GUI_DrawGradientV(10 + width_part_big / 2 + 2*width_part_big + 2*width_part, 10, 10 + width_part_big / 2 + 2*width_part_big +2* width_part, head_high, GUI_BLACK, 0xAAAAAA);
    GUI_DrawVLine(10 + width_part_big / 2 + 2*width_part_big + 2*width_part, head_high, y_size - 10);

    GUI_DrawHLine(head_high+high, 10, x_size - 10);
    GUI_DrawHLine(head_high + 2*high, 10, x_size - 10);
    GUI_DrawHLine(head_high + 3*high, 10, x_size - 10);
    GUI_DrawHLine(head_high + 4*high, 10, x_size - 10);
    GUI_DrawHLine(head_high + 5*high, 10, x_size - 10);
    GUI_DrawHLine(head_high + 6*high, 10, x_size - 10);
    
    GUI_SetColor(GUI_BLACK);
    GUI_FillRect(10 + width_part_big, head_high, 10 + width_part_big + width_part, y_size - 10);
    GUI_FillRect(10 + width_part_big * 2  + width_part, head_high, 10 + width_part_big * 2  + 2*width_part, y_size - 10);
    
    GUI_SetColor(GUI_WHITE);
    rect_str.x0 = 10;
    rect_str.y0 = 0;
    rect_str.x1 = 10 + width_part_big/2;
    rect_str.y1 = head_high;
    GUI_SetFont(&SIF16_Font);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("电枪监测项", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);  
    GUI_SetColor(GUI_WHITE);
    rect_str.x0 = 10 + width_part_big / 2;
    rect_str.y0 = 0;
    rect_str.x1 = 10 + width_part_big ;
    rect_str.y1 = head_high;
    GUI_SetFont(&SIF16_Font);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("项目状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER);
    
    rect_str.x0 = 10 + width_part_big + width_part;
    rect_str.y0 = 0;
    rect_str.x1 = 10 + width_part_big + width_part_big/2  + width_part;
    rect_str.y1 = head_high;
    GUI_SetFont(&SIF16_Font);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("电枪监测项", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    rect_str.x0 = 10 + width_part_big + width_part_big / 2  + width_part;
    rect_str.y0 = 0;
    rect_str.x1 = 10 + width_part_big *2  + width_part;
    rect_str.y1 = head_high;
    GUI_SetFont(&SIF16_Font);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("项目状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER); 

    rect_str.x0 = 10 + width_part_big * 2  + 2*width_part;
    rect_str.y0 = 0;
    rect_str.x1 = 10 + width_part_big * 2  + 2*width_part + width_part_big/2;
    rect_str.y1 = head_high;
    GUI_SetFont(&SIF16_Font);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("电枪监测项", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    rect_str.x0 = 10 + width_part_big * 2  + 2*width_part + width_part_big / 2;
    rect_str.y0 = 0;
    rect_str.x1 = 10 + width_part_big * 3  + 2*width_part ;
    rect_str.y1 = head_high;
    GUI_SetFont(&SIF16_Font);
    GUI_SetBkColor(GUI_BLACK);
    GUI_DispStringInRect("项目状态", &rect_str, GUI_TA_VCENTER | GUI_TA_HCENTER); 
}

static void gunA_B_draw_data(WM_MESSAGE *pMsg, CON_t *pcont)
{
    GUI_RECT rect_item,rect_state,rect_alarm;
    int x, x_size, y, y_size;
    int high = 41;//每行高度
    int width_part = 8;//间隔黑条宽度
    int head_high = 53;//标题高度
    int alarm_width = 10;//感叹号宽度
    
    int width_part_big;//每部分宽度
    
    uint8_t strTmp[30];
    get_position(pMsg->hWin, &x, &x_size, &y, &y_size);
    width_part_big = (x_size - 20 - 2*width_part) / 3;
    GUI_SetFont(&SIF12_Font);
    GUI_SetColor(GUI_BLACK);
    GUI_SetBkColor(GUI_WHITE);
    
    //枪的工作状态
    rect_item.x0 = 10;
    rect_item.y0 = head_high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + high;
    if ((pcont->status.ulSignalState & defSignalCON_State_Working) == defSignalCON_State_Working)
    {
        GUI_DispStringInRect("工作状态", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("充电中", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    else if ((pcont->status.ulSignalState & defSignalCON_State_Standby) == defSignalCON_State_Standby)
    {
        GUI_DispStringInRect("工作状态", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("空闲", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    else if ((pcont->status.ulSignalState & defSignalCON_State_Fault) == defSignalCON_State_Fault)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10;
        rect_alarm.y0 = head_high;
        rect_alarm.x1 = 10 + alarm_width;
        rect_alarm.y1 = head_high + high;
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("工作状态", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("故障", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_SetColor(GUI_BLACK);
    }
    
    //枪的交流电压
    rect_item.x0 = 10;
    rect_item.y0 = head_high + high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + 2*high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high + high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + 2* high;
    sprintf(strTmp, "%.1f", pcont->status.dChargingVoltage);
    strcat(strTmp, "V");
    if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltUp) == defSignalCON_Alarm_AC_A_VoltUp)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10;
        rect_alarm.y0 = head_high + high;
        rect_alarm.x1 = 10 + alarm_width;
        rect_alarm.y1 = head_high + 2*high;
        strcat(strTmp, "-过压");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("电压", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_VoltLow) == defSignalCON_Alarm_AC_A_VoltLow)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10;
        rect_alarm.y0 = head_high + high;
        rect_alarm.x1 = 10 + alarm_width;
        rect_alarm.y1 = head_high + 2*high;
        strcat(strTmp, "-欠压");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("电压", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("电压", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    
    //枪的充电电流    
    rect_item.x0 = 10;
    rect_item.y0 = head_high + 2*high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + 3*high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high + 2*high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + 3* high;
    sprintf(strTmp, "%.1f", pcont->status.dChargingCurrent);
    strcat(strTmp, "A");
    if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_CurrUp_Cri) == defSignalCON_Alarm_AC_A_CurrUp_Cri)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10;
        rect_alarm.y0 = head_high + 2*high;
        rect_alarm.x1 = 10 + alarm_width;
        rect_alarm.y1 = head_high + 3*high;
        strcat(strTmp, "-过流");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("电流", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("电流", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    
    //充电频率
    rect_item.x0 = 10;
    rect_item.y0 = head_high + 3*high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + 4*high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high + 3*high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + 4* high;
    sprintf(strTmp, "%.1f", pcont->status.dChargingFrequence);
    strcat(strTmp, "Hz");
    if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_AC_Freq_Cri) == defSignalCON_Alarm_AC_Freq_Cri)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10;
        rect_alarm.y0 = head_high + 3*high;
        rect_alarm.x1 = 10 + alarm_width;
        rect_alarm.y1 = head_high + 4*high;
        strcat(strTmp, "-异常");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("频率", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("频率", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    
    //cp电压
    rect_item.x0 = 10;
    rect_item.y0 = head_high + 4*high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + 5*high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high + 4*high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + 5* high;
    sprintf(strTmp, "%.1f", pcont->status.dCPVolt);
    strcat(strTmp, "V");
    if ((pcont->status.ulSignalFault & defSignalCON_Fault_CP) == defSignalCON_Fault_CP)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10;
        rect_alarm.y0 = head_high + 4*high;
        rect_alarm.x1 = 10 + alarm_width;
        rect_alarm.y1 = head_high + 5*high;
        strcat(strTmp, "-故障");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("CP电压", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("CP电压", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    
    //功率
    rect_item.x0 = 10;
    rect_item.y0 = head_high + 5*high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + 6*high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high + 5*high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + 6* high;
    sprintf(strTmp, "%.1f", pcont->status.dChargingPower);
    strcat(strTmp, "kW");
    GUI_DispStringInRect("当前功率", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
    GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    
    //负载百分比
    rect_item.x0 = 10;
    rect_item.y0 = head_high + 6*high;
    rect_item.x1 = 10 + width_part_big / 2;
    rect_item.y1 = head_high + 7*high;
    rect_state.x0 = 10 + width_part_big / 2;
    rect_state.y0 =  head_high + 6*high;
    rect_state.x1 = 10 + width_part_big;
    rect_state.y1 = head_high + 7* high;
    sprintf(strTmp, "%d", pcont->status.ucLoadPercent);
    strcat(strTmp, "%");
    GUI_DispStringInRect("负载百分比", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
    GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);   
    
    /**< 继电器输出L温度 */
    rect_item.x0 = 10 + width_part_big + width_part;
    rect_item.y0 = head_high ;
    rect_item.x1 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_item.y1 = head_high + high;
    rect_state.x0 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_state.y0 =  head_high ;
    rect_state.x1 = 10 + width_part_big + width_part + width_part_big;
    rect_state.y1 = head_high +  high;
    sprintf(strTmp, "%.1f", pcont->status.dACLTemp);//acl or acn
    strcat(strTmp, "℃");
    if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_AC_A_Temp_Cri) == defSignalCON_Alarm_AC_A_Temp_Cri)
    {
        GUI_SetColor(GUI_RED);
        rect_item.x0 = 10 + width_part_big + width_part;
        rect_item.y0 = head_high;
        rect_item.x1 = 10 + width_part_big + width_part + alarm_width;
        rect_item.y1 = head_high + high;
        strcat(strTmp, "-超限");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("输出(L)温度", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("输出(L)温度", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }    

    /**< 输出继电器N温度 */
    rect_item.x0 = 10 + width_part_big + width_part;
    rect_item.y0 = head_high + high;
    rect_item.x1 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_item.y1 = head_high + 2*high;
    rect_state.x0 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_state.y0 =  head_high+ high;
    rect_state.x1 = 10 + width_part_big + width_part + width_part_big;
    rect_state.y1 = head_high +  2*high;
    sprintf(strTmp, "%.1f", pcont->status.dACNTemp);
    strcat(strTmp, "℃");
    if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_AC_N_Temp_Cri) == defSignalCON_Alarm_AC_N_Temp_Cri)
    {
        GUI_SetColor(GUI_RED);
        rect_item.x0 = 10 + width_part_big + width_part;
        rect_item.y0 = head_high + high;
        rect_item.x1 = 10 + width_part_big + width_part + alarm_width;
        rect_item.y1 = head_high + 2*high;
        strcat(strTmp, "-超限");
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("输出(N)温度", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("输出(N)温度", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    /**< 电表通信 */
    rect_item.x0 = 10 + width_part_big + width_part;
    rect_item.y0 = head_high + 2*high;
    rect_item.x1 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_item.y1 = head_high + 3*high;
    rect_state.x0 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_state.y0 =  head_high + 2*high;
    rect_state.x1 = 10 + width_part_big + width_part + width_part_big;
    rect_state.y1 = head_high +  3*high;
    if ((pcont->status.ulSignalFault & defSignalCON_Fault_Meter) == defSignalCON_Fault_Meter)
    {
        GUI_SetColor(GUI_RED);
        rect_item.x0 = 10 + width_part_big + width_part;
        rect_item.y0 = head_high + 2*high;
        rect_item.x1 = 10 + width_part_big + width_part + alarm_width;
        rect_item.y1 = head_high + 3*high;
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("电表通信", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("故障", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("电表通信", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("正常", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);     
    }
    
    //电表读数
    rect_item.x0 = 10 + width_part_big + width_part;
    rect_item.y0 = head_high + 3*high;
    rect_item.x1 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_item.y1 = head_high + 4*high;
    rect_state.x0 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_state.y0 =  head_high + 3*high;
    rect_state.x1 = 10 + width_part_big + width_part + width_part_big;
    rect_state.y1 = head_high +  4*high;
    sprintf(strTmp, "%.1f", pcont->status.dChargingEnergy);
    strcat(strTmp, "kWh");
    GUI_DispStringInRect("电表读数", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
    GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    
    /**< 继电器黏连 */
    rect_item.x0 = 10 + width_part_big + width_part;
    rect_item.y0 = head_high + 4*high;
    rect_item.x1 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_item.y1 = head_high + 5*high;
    rect_state.x0 = 10 + width_part_big + width_part + width_part_big / 2;
    rect_state.y0 =  head_high + 4*high;
    rect_state.x1 = 10 + width_part_big + width_part + width_part_big;
    rect_state.y1 = head_high +  5*high;
    if ((pcont->status.ulSignalFault & defSignalCON_Fault_RelayPaste) == defSignalCON_Fault_RelayPaste)
    {
        GUI_SetColor(GUI_RED);
        rect_alarm.x0 = 10 + width_part_big + width_part;
        rect_alarm.y0 = head_high + 4*high;
        rect_alarm.x1 = 10 + width_part_big + width_part + alarm_width;
        rect_alarm.y1 = head_high + 5*high;
        GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("黏连", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("发生", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
        GUI_SetColor(GUI_BLACK);
    }
    else
    {
        GUI_DispStringInRect("黏连", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("未发生", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    
    /**< 输出继电器L状态 */
    rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
    rect_item.y0 = head_high + 0*high;
    rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
    rect_item.y1 = head_high + 1*high;
    rect_state.x0 = 10 + 2*width_part_big + 2*width_part+ width_part_big / 2;
    rect_state.y0 =  head_high + 0*high;
    rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
    rect_state.y1 = head_high +  1*high;
    if ((pcont->status.ulSignalState & defSignalCON_State_AC_A_Relay) == defSignalCON_State_AC_A_Relay)
    {
        GUI_DispStringInRect("继电器L", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("闭合", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    else
    {
        GUI_DispStringInRect("继电器L", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("断开", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    
    /**< 输出继电器N状态 */
    rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
    rect_item.y0 = head_high + 1*high;
    rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
    rect_item.y1 = head_high + 2*high;
    rect_state.x0 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
    rect_state.y0 =  head_high + 1*high;
    rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
    rect_state.y1 = head_high +  2*high;
    if ((pcont->status.ulSignalState & defSignalCON_State_AC_N_Relay) == defSignalCON_State_AC_N_Relay)
    {
        GUI_DispStringInRect("继电器N", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("闭合", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    else
    {
        GUI_DispStringInRect("继电器N", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("断开", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
    }
    //枪连接状态
    rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
    rect_item.y0 = head_high + 2*high;
    rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
    rect_item.y1 = head_high + 3*high;
    rect_state.x0 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
    rect_state.y0 =  head_high + 2*high;
    rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
    rect_state.y1 = head_high +  3*high;
    if (pcont->status.xPlugState == PLUG)
    {
        GUI_DispStringInRect("枪连接", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("已连接", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    else
    {
        GUI_DispStringInRect("枪连接", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
        GUI_DispStringInRect("未连接", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
    }
    
    if (pcont->info.ucSocketType == defSocketTypeB)
    {
        rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
        rect_item.y0 = head_high + 3*high;
        rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_item.y1 = head_high + 4*high;
        rect_state.x0 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_state.y0 =  head_high + 3*high;
        rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
        rect_state.y1 = head_high +  4*high;
        if (pcont->status.xCCState == CC_PE)
        {
            GUI_DispStringInRect("CC检测", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect("CC_PE", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);    
        }
        else
        {
            GUI_DispStringInRect("CC检测", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect("CC_NO", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);   
        }
            /**< A插座温度1 */
        rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
        rect_item.y0 = head_high + 4*high;
        rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_item.y1 = head_high + 5*high;
        rect_state.x0 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_state.y0 =  head_high + 4*high;
        rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
        rect_state.y1 = head_high +  5*high;
        sprintf(strTmp, "%.1f", pcont->status.dBTypeSocketTemp1);
        strcat(strTmp, "℃");
        if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp1_Cri) == defSignalCON_Alarm_SocketTemp1_Cri)
        {
            GUI_SetColor(GUI_RED);
            rect_alarm.x0 = 10 + 2*width_part_big + 2*width_part;
            rect_alarm.y0 = head_high + 4*high;
            rect_alarm.x1 = 10 + 2*width_part_big + 2*width_part + alarm_width;
            rect_alarm.y1 = head_high + 5*high;
            GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
            strcat(strTmp, "-超限");
            GUI_DispStringInRect("插座温度1", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
            GUI_SetColor(GUI_BLACK);
        }
        else
        {
            GUI_DispStringInRect("插座温度1", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER); 
        }

            /**< A插座温度2 */
        rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
        rect_item.y0 = head_high + 5*high;
        rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_item.y1 = head_high + 6*high;
        rect_state.x0 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_state.y0 =  head_high + 5*high;
        rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
        rect_state.y1 = head_high +  6*high;
        sprintf(strTmp, "%.1f", pcont->status.dBTypeSocketTemp2);
        strcat(strTmp, "℃");
        if ((pcont->status.ulSignalAlarm & defSignalCON_Alarm_SocketTemp2_Cri) == defSignalCON_Alarm_SocketTemp2_Cri)
        {
            GUI_SetColor(GUI_RED);
            rect_alarm.x0 = 10 + 2*width_part_big + 2*width_part;
            rect_alarm.y0 = head_high + 5*high;
            rect_alarm.x1 = 10 + 2*width_part_big + 2*width_part + alarm_width;
            rect_alarm.y1 = head_high + 6*high;
            GUI_DispStringInRect("!", &rect_alarm, GUI_TA_VCENTER | GUI_TA_HCENTER);
            strcat(strTmp, "-超限");
            GUI_DispStringInRect("插座温度2", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);        
            GUI_SetColor(GUI_BLACK);
        }
        else
        {
            GUI_DispStringInRect("插座温度2", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect(strTmp, &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);  
        }
        
        
        rect_item.x0 = 10 + 2*width_part_big + 2*width_part;
        rect_item.y0 = head_high + 6*high;
        rect_item.x1 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_item.y1 = head_high + 7*high;
        rect_state.x0 = 10 + 2*width_part_big + 2*width_part + width_part_big / 2;
        rect_state.y0 =  head_high + 6*high;
        rect_state.x1 = 10 + 3*width_part_big + 2*width_part;
        rect_state.y1 = head_high +  7*high;
        if ((pcont->status.ulSignalState & defSignalCON_State_SocketLock) == defSignalCON_State_SocketLock)
        {
            GUI_DispStringInRect("枪锁", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect("锁止", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
        else
        {
            GUI_DispStringInRect("枪锁", &rect_item, GUI_TA_VCENTER | GUI_TA_HCENTER);
            GUI_DispStringInRect("未锁止", &rect_state, GUI_TA_VCENTER | GUI_TA_HCENTER);
        }
    }
}

static void _cbDialog_gunA(WM_MESSAGE *pMsg)
{
    CON_t *pCon;
    int x, x_size, y, y_size;
    pCon = CONGetHandle(0);
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        gunA_B_draw_grids(pMsg);
        gunA_B_draw_data(pMsg, pCon);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        WM_Paint(pMsg->hWin);
        break; 
    case WM_INIT_DIALOG:
        break;
    case WM_NOTIFY_PARENT:
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerGunA)
        {
            WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            WM_RestartTimer(pMsg->Data.v, 20);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialog_gunB(WM_MESSAGE *pMsg)
{
    CON_t *pCon;
    int x, x_size, y, y_size;
    pCon = CONGetHandle(1);
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        gunA_B_draw_grids(pMsg);
        gunA_B_draw_data(pMsg, pCon);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        WM_Paint(pMsg->hWin);
        break; 
    case WM_INIT_DIALOG:
        break;
    case WM_NOTIFY_PARENT:
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerGunB)
        {
            WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            WM_RestartTimer(pMsg->Data.v, 20);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

typedef struct
{
    GUI_POINT left_bottom_point;
    GUI_POINT left_top_point;
    GUI_POINT right_bottom_point;
    GUI_POINT right_top_point;
}STRUCT_JUNCTION_LINE;

static void draw_junction(STRUCT_JUNCTION_LINE junction_line)
{
    int x, y;
    int x1, y1;
    x = (junction_line.right_bottom_point.x + junction_line.left_bottom_point.x) / 2 - JUNCTION_LINE_WIDTH / 2;
    y = junction_line.right_top_point.y;
    x1 = (junction_line.right_bottom_point.x + junction_line.left_bottom_point.x) / 2 + JUNCTION_LINE_WIDTH / 2;
    y1 = junction_line.left_bottom_point.y;
    GUI_FillRoundedRect(x, y, x1, y1, 5);
    GUI_FillRoundedRect(x, y, junction_line.right_bottom_point.x + 10, junction_line.right_bottom_point.y, 5);
    GUI_FillRoundedRect(junction_line.left_top_point.x - 10, junction_line.left_top_point.y, x1, y1, 5);
}

static void _cbDialog(WM_MESSAGE *pMsg)
{
    const void *pData;
    WM_HWIN      hItem;
    const char *strdouble[3] = { "pile", "gunA", "gunB" };
    const char *strsingle[2] = { "pile", "gun" };
    HEADER_SKINFLEX_PROPS head_skin;
    int i;
    int NCode;
    int Id;
    int x, x_size, y, y_size;
    STRUCT_JUNCTION_LINE junction_line;
    
    switch (pMsg->MsgId)
    {
    case WM_PAINT:
        GUI_SetColor(GUI_BLACK);
        if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0)) == 1)
        {
            get_position(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0), &x, &x_size, &y, &y_size);
            junction_line.left_bottom_point.x = x + x_size;
            junction_line.left_bottom_point.y = (y + y + y_size) / 2 + JUNCTION_LINE_WIDTH / 2;
            junction_line.left_top_point.x = x + x_size;
            junction_line.left_top_point.y = (y + y + y_size) / 2 - JUNCTION_LINE_WIDTH / 2;             
            //GUI_FillRect(x + x_size - 4, y, x + 3*x_size, y + y_size - 1);
            get_position(hItem_pile, &x, &x_size, &y, &y_size);
            junction_line.right_bottom_point.x = x;
            junction_line.right_bottom_point.y = y + 15 + JUNCTION_LINE_WIDTH;
            junction_line.right_top_point.x = x;
            junction_line.right_top_point.y = y + 15;
            draw_junction(junction_line);
           // GUI_FillRoundedRect(x - 4, y - 4, x + x_size + 4, y + y_size + 4, 4);
        }
        if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1)) == 1)
        {
            get_position(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1), &x, &x_size, &y, &y_size);
            junction_line.left_bottom_point.x = x + x_size;
            junction_line.left_bottom_point.y = (y + y + y_size) / 2 + JUNCTION_LINE_WIDTH / 2;
            junction_line.left_top_point.x = x + x_size;
            junction_line.left_top_point.y = (y + y + y_size) / 2 - JUNCTION_LINE_WIDTH / 2;
            get_position(hItem_gunA, &x, &x_size, &y, &y_size);
            junction_line.right_bottom_point.x = x;
            junction_line.right_bottom_point.y = y + 15 + JUNCTION_LINE_WIDTH;
            junction_line.right_top_point.x = x;
            junction_line.right_top_point.y = y + 15;
            draw_junction(junction_line);
        }
        if (pEVSE->info.ucTotalCON == 2)
        {
            if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2)) == 1)
            {
                get_position(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2), &x, &x_size, &y, &y_size);
                junction_line.left_bottom_point.x = x + x_size;
                junction_line.left_bottom_point.y = (y + y + y_size) / 2 + JUNCTION_LINE_WIDTH / 2;
                junction_line.left_top_point.x = x + x_size;
                junction_line.left_top_point.y = (y + y + y_size) / 2 - JUNCTION_LINE_WIDTH / 2;
                get_position(hItem_gunB, &x, &x_size, &y, &y_size);
                junction_line.right_bottom_point.x = x;
                junction_line.right_bottom_point.y = y + 15 + JUNCTION_LINE_WIDTH;
                junction_line.right_top_point.x = x;
                junction_line.right_top_point.y = y + 15;
                draw_junction(junction_line);
            }
        }
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break; 
    case WM_INIT_DIALOG:
        hItem_pile = WINDOW_CreateEx(175, 10, 520, 350, pMsg->hWin, WM_CF_SHOW, 0, ID_WINDOW_1, _cbDialog_pile);
        WINDOW_SetBkColor(hItem_pile, GUI_BLACK);
        _timerPile = WM_CreateTimer(hItem_pile, ID_TimerPile, 20, 0);
        WM_HideWindow(hItem);
        hItem_gunA = WINDOW_CreateEx(140, 10, 630, 350, pMsg->hWin, WM_CF_SHOW, 0, ID_WINDOW_2, _cbDialog_gunA);
        _timerGunA = WM_CreateTimer(hItem_gunA, ID_TimerGunA, 20, 0);
        WM_HideWindow(hItem_gunA);
        if (pEVSE->info.ucTotalCON == 1)
        {
            for (i = 0; i < 2; i++)
            {
                hItem = CHECKBOX_CreateUser(10, 60 + i * 90, 100, 60, pMsg->hWin, WM_CF_SHOW, 0, ID_CHECKBOX_0 + i, 10);
                CHECKBOX_SetUserData(hItem, strsingle[i], 10);
                CHECKBOX_SetSkin(hItem, SKIN_checkboxStateInfo);
            }
            RADIO2(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1)); 
            window_show_hide2(pMsg);
        }
        else if (pEVSE->info.ucTotalCON == 2)
        {
            hItem_gunB = WINDOW_CreateEx(140, 10, 630, 350, pMsg->hWin, WM_CF_SHOW, 0, ID_WINDOW_3, _cbDialog_gunB);
            _timerGunB = WM_CreateTimer(hItem_gunB, ID_TimerGunB, 20, 0);
            WM_HideWindow(hItem_gunB);
            for (i = 0; i < 3; i++)
            {
                hItem = CHECKBOX_CreateUser(10, 60 + i * 90, 100, 60, pMsg->hWin, WM_CF_SHOW, 0, ID_CHECKBOX_0 + i, 10);
                CHECKBOX_SetUserData(hItem, strdouble[i], 10);
                CHECKBOX_SetSkin(hItem, SKIN_checkboxStateInfo);
            }
            RADIO3(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2));
            window_show_hide3(pMsg);
        }
       //WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_WINDOW_1));
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
                    window_show_hide2(pMsg);
                }
                else
                { 
                    RADIO3(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2));
                    window_show_hide3(pMsg);
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
                    window_show_hide2(pMsg);
                }
                else
                {
                    RADIO3(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                        WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2)); 
                    window_show_hide3(pMsg);
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
                window_show_hide3(pMsg);
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerData)
        {
           // Status_Content_Analy(pMsg);
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


WM_HWIN CreateManagerInfoAnalog(WM_HWIN srcHwin)
{
    _hWinManagerInfoAnalog = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_GetClientWindow(srcHwin), 0, 0);
//    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerTime, 20, 0);
    _timerData = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerData, 1000, 0);
//    _timerSignal = WM_CreateTimer(WM_GetClientWindow(_hWinManagerInfoAnalog), ID_TimerSignal,5000,0);
    return _hWinManagerInfoAnalog;
}
/*************************** End of file ****************************/









