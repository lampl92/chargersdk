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

#define ID_BUTTON_20  (GUI_ID_USER + 0x10)
#define ID_BUTTON_21  (GUI_ID_USER + 0x11)
#define ID_BUTTON_22  (GUI_ID_USER + 0x12)
#define ID_BUTTON_23  (GUI_ID_USER + 0x13)
#define ID_BUTTON_24  (GUI_ID_USER + 0x14)
#define ID_BUTTON_25  (GUI_ID_USER + 0x15)
#define ID_BUTTON_26  (GUI_ID_USER + 0x16)
#define ID_BUTTON_27  (GUI_ID_USER + 0x17)
#define ID_BUTTON_28  (GUI_ID_USER + 0x18)
#define ID_BUTTON_29  (GUI_ID_USER + 0x19)
#define ID_BUTTON_30  (GUI_ID_USER + 0x1A)
#define ID_BUTTON_31  (GUI_ID_USER + 0x1B)
#define ID_BUTTON_32  (GUI_ID_USER + 0x1C)
#define ID_BUTTON_33  (GUI_ID_USER + 0x1D)
#define ID_BUTTON_34  (GUI_ID_USER + 0x1E)
#define ID_BUTTON_35  (GUI_ID_USER + 0x1F)
#define ID_BUTTON_36  (GUI_ID_USER + 0x20)
#define ID_BUTTON_37  (GUI_ID_USER + 0x21)
#define ID_BUTTON_38  (GUI_ID_USER + 0x22)
#define ID_BUTTON_39  (GUI_ID_USER + 0x23)
#define ID_WINDOW_1    (GUI_ID_USER + 0x24)

#define ID_TimerData    1

static WM_HTIMER  _timerData;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { MULTIEDIT_CreateIndirect, "Multiedit", ID_MULTIEDIT_0, 250, 2, 548, 476, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_0, 60, 80, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test", ID_BUTTON_1, 60, 170, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "recover", ID_BUTTON_2, 60, 260, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "reset", ID_BUTTON_3, 60, 350, 130, 50, 0, 0x0, 0 },
    //    { BUTTON_CreateIndirect, "stop", ID_BUTTON_4, 325, 400, 120, 60, 0, 0x0, 0 },
    //    { BUTTON_CreateIndirect, "go", ID_BUTTON_5, 590, 400, 120, 60, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreateTest[] = {
    { WINDOW_CreateIndirect, "Window", ID_WINDOW_1, 0, 0, 800, 480, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test0", ID_BUTTON_20, 35, 23, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test1", ID_BUTTON_21, 235, 23, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test2", ID_BUTTON_22, 435, 23, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test3", ID_BUTTON_23, 635, 23, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test4", ID_BUTTON_24, 35, 119, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test5", ID_BUTTON_25, 235, 119, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test6", ID_BUTTON_26, 435, 119, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test7", ID_BUTTON_27, 635, 119, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test8", ID_BUTTON_28, 35, 215, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test9", ID_BUTTON_29, 235, 215, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test10", ID_BUTTON_30, 435, 215, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test11", ID_BUTTON_31, 635, 215, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test12", ID_BUTTON_32, 35, 311, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test13", ID_BUTTON_33, 235, 311, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test14", ID_BUTTON_34, 435, 311, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test15", ID_BUTTON_35, 635, 311, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test16", ID_BUTTON_36, 35, 407, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test17", ID_BUTTON_37, 235, 407, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test18", ID_BUTTON_38, 435, 407, 130, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "test19", ID_BUTTON_39, 635, 407, 130, 50, 0, 0x0, 0 },
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
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_6);
        BUTTON_SetFont(hItem, &SIF16_Font);
        BUTTON_SetText(hItem, "确定-重启");
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_7);
        BUTTON_SetFont(hItem, &SIF16_Font);
        BUTTON_SetText(hItem, "取消");
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
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_6);
        BUTTON_SetFont(hItem, &SIF16_Font);
        BUTTON_SetText(hItem, "确定-重启");
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_7);
        BUTTON_SetFont(hItem, &SIF16_Font);
        BUTTON_SetText(hItem, "取消");
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

static void _cbDialogTest(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    uint32_t sizeMul = 0;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:      
        break;
    case WM_PAINT:
        GUI_SetColor(GUI_BLACK);    
        GUI_FillRect(0, 0, 800, 480);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_20: // test0
          switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;
        case ID_BUTTON_21: // test1
			switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;
        case ID_BUTTON_22: // test2
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;
        case ID_BUTTON_23: // test3
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;
        case ID_BUTTON_24: // test4
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;
        case ID_BUTTON_25: // test5
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_26: // test6
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_27: // test7
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_28: // test8
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;    
        case ID_BUTTON_29: // test9
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_30: // test10
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_31: // test11
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_32: // test12
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_33: // test13
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;    
        case ID_BUTTON_34: // test14
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        case ID_BUTTON_35: // test15
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;  
        case ID_BUTTON_36: // test16
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;       
        case ID_BUTTON_37: // test17
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;    
        case ID_BUTTON_38: // test18
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;     
        case ID_BUTTON_39: // test19
            switch(NCode) {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)

                // USER END
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                // USER END
                break;
            }
            break;        
        }
    case WM_TIMER :
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    WM_HWIN hItem;
    int     NCode;
    int     Id;
    uint32_t sizeMul = 0;
    static char strShow[3000] = { 0 };
    static char strTermCtx[105] = { 0 };
    SCROLLBAR_Handle hScrollMul;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        memset(strShow, 0, sizeof(strShow));
        hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
        //MULTIEDIT_SetReadOnly(hItem, 1);
        MULTIEDIT_SetBkColor(hItem, MULTIEDIT_CI_READONLY, GUI_BLACK);
        MULTIEDIT_SetTextColor(hItem, MULTIEDIT_CI_READONLY, GUI_WHITE);
        MULTIEDIT_SetFont(hItem, &SIF12_Font);
        MULTIEDIT_SetWrapWord(hItem);
        MULTIEDIT_SetBufferSize(hItem, 500);
        MULTIEDIT_SetMaxNumChars(hItem, 4000);
        MULTIEDIT_SetInsertMode(hItem, 0);        
        hScrollMul = SCROLLBAR_CreateAttached(hItem, GUI_ID_VSCROLL);
        SCROLLBAR_SetWidth(hScrollMul, 50);        
        break;
    case WM_PAINT:
        GUI_SetColor(GUI_BLACK);    
        GUI_FillRect(0, 0, 800, 480);
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
                GUI_CreateDialogBox(_aDialogCreateTest, GUI_COUNTOF(_aDialogCreateTest), _cbDialogTest, pMsg->hWin, 0, 0);
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
//            if (ring_buffer_len(term_rb) > 0)
//            {
//                //ring_buffer_get(term_rb, strTermCtx, ring_buffer_len(term_rb));
//                ring_buffer_get(term_rb, strTermCtx, 50);
//                
//                hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
//                MULTIEDIT_AddText(hItem, strTermCtx);  
//                //MULTIEDIT_AddText(hItem, "sahdjasiodibasdbasddsadasdasdasdasdasdasd\n"); 
//            
//                sizeMul = MULTIEDIT_GetTextSize(hItem);
//                if (sizeMul > 1500)
//                {
//                    MULTIEDIT_SetBufferSize(hItem, 500);
//                }
//                memset(strTermCtx, 0, sizeof(strTermCtx));
//            }                
           // if(ring_buffer_len(term_rb) > 0)
            //{
                //ring_buffer_get(term_rb, strTermCtx, ring_buffer_len(term_rb));
               // ring_buffer_get(term_rb, strTermCtx, 50);
                //strcat(strShow, strTermCtx);
                strcat(strShow, "sahdjasiodibasdbasddsadasdasdasdasdasdasd\n");
                hItem = WM_GetDialogItem(pMsg->hWin, ID_MULTIEDIT_0);
                //MULTIEDIT_SetText(hItem, strShow);  
                MULTIEDIT_AddText(hItem, "sahdjasiodibasdbasddsadasdasdasdasdasdasd\n"); 
				//MULTIEDIT_AddText(hItem, strTermCtx);
                
                if (strlen(strShow) > 2900)
                {
                    memset(strShow, 0, sizeof(strShow));
                    MULTIEDIT_SetBufferSize(hItem, 500);
                }
                memset(strTermCtx, 0, sizeof(strTermCtx));
           // } 

            
            
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}



WM_HWIN CreateSuperManager(void);
WM_HWIN CreateSuperManager(void) {
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerData = WM_CreateTimer(hWin, ID_TimerData, 1000, 0);
    return hWin;
}
