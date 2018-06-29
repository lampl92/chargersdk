#include "DIALOG.h"
#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"
#include "bsp_rtc.h"
#include "bsp_gpio.h"
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
#include "yaffsfs.h"

#define ID_WINDOW_0    (GUI_ID_USER + 0x00)
#define ID_MULTIEDIT_0    (GUI_ID_USER + 0x01)
#define ID_BUTTON_0    (GUI_ID_USER + 0x02)
#define ID_BUTTON_1    (GUI_ID_USER + 0x03)
#define ID_BUTTON_2    (GUI_ID_USER + 0x04)
#define ID_BUTTON_3    (GUI_ID_USER + 0x05)
#define ID_BUTTON_4    (GUI_ID_USER + 0x06)
#define ID_BUTTON_5    (GUI_ID_USER + 0x07)

#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x08)
#define ID_TEXT_1     (GUI_ID_USER + 0x09)
#define ID_BUTTON_6  (GUI_ID_USER + 0x0A)
#define ID_BUTTON_7  (GUI_ID_USER + 0x0B)

#define ID_TimerData    1

static WM_HTIMER  _timerData;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_0, 250, 20, 530, 360, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_0, 60, 60, 130, 60, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test", ID_BUTTON_1, 60, 180, 130, 60, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "recover", ID_BUTTON_2, 60, 300, 130, 60, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "reset", ID_BUTTON_3, 60, 420, 130, 60, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "stop", ID_BUTTON_4, 325, 400, 120, 60, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "go", ID_BUTTON_5, 590, 400, 120, 60, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreateFrame[] =
{
    { FRAMEWIN_CreateIndirect, "!!!!", ID_FRAMEWIN_0, 365, 100, 300, 200, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 0, 50, 300, 50, TEXT_CF_HCENTER, 0x0, 0 },
    { BUTTON_CreateIndirect, "确定-重启", ID_BUTTON_6, 50, 110, 80, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "取消", ID_BUTTON_7, 200, 110, 80, 50, 0, 0x0, 0 },
};

static void _cbDialog_frame_record(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    char buff[10];
    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        FRAMEWIN_SetFont(pMsg->hWin, &SIF24_Font);
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_TEXT_1);
        TEXT_SetFont(hItem, &SIF16_Font);
        TEXT_SetText(hItem, "将删除所有记录文件!");
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
        BUTTON_SetFont(WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_3), &SIF16_Font);
        BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3), "确定-重启");
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
        BUTTON_SetFont(WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_4), &SIF16_Font);
        BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4), "取消");
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_6:
            switch (NCode)
            {
            case WM_NOTIFICATION_RELEASED:
                yaffs_unlink(pathOrder);
                yaffs_unlink(pathOrderTmp);
                yaffs_unlink(pathEVSELog);
                NVIC_SystemReset();
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_7:
            switch (NCode)
            {
            case WM_NOTIFICATION_RELEASED:
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        }
    }
}

static void _cbDialog_frame_default(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    char buff[10];
    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        FRAMEWIN_SetFont(pMsg->hWin, &SIF24_Font);
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_TEXT_1);
        TEXT_SetFont(hItem, &SIF16_Font);
        TEXT_SetText(hItem, "将清空所有配置和记录文件!");
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
        BUTTON_SetFont(WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_6), &SIF16_Font);
        BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6), "确定-重启");
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
        BUTTON_SetFont(WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_7), &SIF16_Font);
        BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7), "取消");
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_6:
            switch (NCode)
            {
            case WM_NOTIFICATION_RELEASED:
                yaffs_unlink(pathOrder);
                yaffs_unlink(pathOrderTmp);
                yaffs_unlink(pathEVSELog);
                yaffs_unlink(pathEVSECfg);
                yaffs_unlink(pathSysCfg);
                yaffs_unlink(pathFTPCfg);
                yaffs_unlink(pathProtoCfg);
                NVIC_SystemReset();
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_7:
            switch (NCode)
            {
            case WM_NOTIFICATION_RELEASED:
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        }
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    uint32_t sizeMul = 0;
    static char strTermCtx[1500];
    SCROLLBAR_Handle hScrollMul;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
        //MULTIEDIT_SetText(hItem, "Multiedit");
        MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_READONLY, GUI_WHITE);
        MULTIEDIT_SetFont(hItem, &SIF16_Font);
        //MULTIEDIT_SetAutoScrollH(hItem, 1);
        //        MULTIEDIT_SetAutoScrollV(hItem, 1);
        MULTIEDIT_SetWrapWord(hItem);
        MULTIEDIT_SetBufferSize(hItem, 10000);
        MULTIEDIT_SetMaxNumChars(hItem, 10000);
        //        MULTIEDIT_SetHBorder(hItem,80);//设置边界
        MULTIEDIT_SetInsertMode(hItem, 0);        
        hScrollMul = SCROLLBAR_CreateAttached(hItem, GUI_ID_VSCROLL);
        SCROLLBAR_SetWidth(hScrollMul, 30);        
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_MULTIEDIT_0: // Notifications sent by 'Multiedit'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_0: // Notifications sent by 'Button'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                GUI_EndDialog(pMsg->hWin, 0);
                home();
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_1: // Notifications sent by 'Button'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_2: // Notifications sent by 'Button'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                GUI_CreateDialogBox(_aDialogCreateFrame, GUI_COUNTOF(_aDialogCreateFrame), _cbDialog_frame_record, WM_GetClientWindow(pMsg->hWin), 0, 0);
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_3: // Notifications sent by 'Button'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                GUI_CreateDialogBox(_aDialogCreateFrame, GUI_COUNTOF(_aDialogCreateFrame), _cbDialog_frame_default, WM_GetClientWindow(pMsg->hWin), 0, 0);
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_4: // Notifications sent by 'Button'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_5: // Notifications sent by 'Button'
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        }
        break;
    case WM_TIMER :
        if (pMsg->Data.v == _timerData)
        {      
            extern ring_buffer_s *term_rb;
            if (ring_buffer_len(term_rb) > 0)
            {
                ring_buffer_get(term_rb, strTermCtx, ring_buffer_len(term_rb));
            }
                
            hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
            MULTIEDIT_AddText(hItem, strTermCtx);
            
            sizeMul = MULTIEDIT_GetTextSize(hItem);
            if (sizeMul > 10000)
            {
                MULTIEDIT_SetBufferSize(hItem, 10000);
                MULTIEDIT_AddText(hItem, strTermCtx);
            }
            memset(strTermCtx, 0, sizeof(strTermCtx));
            
            WM_RestartTimer(pMsg->Data.v, 500);
        }
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
/*********************************************************************
*
*       CreateWindow
*/
WM_HWIN CreateSuperManager(void);
WM_HWIN CreateSuperManager(void) {
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerData = WM_CreateTimer(hWin, ID_TimerData, 1000, 0);
    return hWin;
}
