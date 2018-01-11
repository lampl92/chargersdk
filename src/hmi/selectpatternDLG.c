#include "DIALOG.h"
#include "siffontcreate.h"
#include "skin.h"
#include  "HMI_Start.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)

#define ID_WINDOW_1 (GUI_ID_USER + 0x0F)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_BUTTON_2 (GUI_ID_USER + 0x03)
#define ID_BUTTON_3 (GUI_ID_USER + 0x04)
#define ID_BUTTON_4 (GUI_ID_USER + 0x05)
#define ID_BUTTON_5 (GUI_ID_USER + 0x06)
#define ID_BUTTON_6 (GUI_ID_USER + 0x07)
#define ID_BUTTON_7 (GUI_ID_USER + 0x08)
#define ID_BUTTON_8 (GUI_ID_USER + 0x09)
#define ID_BUTTON_9 (GUI_ID_USER + 0x0A)
#define ID_BUTTON_10 (GUI_ID_USER + 0x0B)
#define ID_BUTTON_11 (GUI_ID_USER + 0x0C)
#define ID_BUTTON_12 (GUI_ID_USER + 0x0D)
#define ID_BUTTON_13 (GUI_ID_USER + 0x0E)
#define ID_CHECKBOX_0 (GUI_ID_USER + 0x12)
#define ID_CHECKBOX_1 (GUI_ID_USER + 0x13)
#define ID_CHECKBOX_2 (GUI_ID_USER + 0x14)
#define ID_CHECKBOX_3 (GUI_ID_USER + 0x15)
#define ID_TEXT_0 (GUI_ID_USER + 0x16)
#define ID_IMAGE_0 (GUI_ID_USER + 0x17)
#define ID_IMAGE_1 (GUI_ID_USER + 0x18)

int FlagDisableKeyboard = 0;
WM_HWIN HwinKeyboard;
static char strNumber[10];

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Select-Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "SelectInfoImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { TEXT_CreateIndirect, "inputInfo", ID_TEXT_0, 400, 150, 100, 24, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkbox", ID_CHECKBOX_0, 100, 120, 100, 45, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkbox", ID_CHECKBOX_1, 100, 195, 100, 45, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkbox", ID_CHECKBOX_2, 100, 270, 100, 45, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkbox", ID_CHECKBOX_3, 100, 345, 100, 45, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate1[] = {
    { WINDOW_CreateIndirect, "KeyBoard-Window", ID_WINDOW_1, 400, 200, 260, 260, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "SelectInfoImage", ID_IMAGE_1, 0, 0, 800, 480, 0, 0, 0 },
    { BUTTON_CreateIndirect, "Button1", ID_BUTTON_0, 15, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button2", ID_BUTTON_1, 75, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button3", ID_BUTTON_2, 135, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-b", ID_BUTTON_3, 195, 15, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button4", ID_BUTTON_4, 15, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button5", ID_BUTTON_5, 75, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button6", ID_BUTTON_6, 135, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-cancel", ID_BUTTON_7, 195, 75, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button7", ID_BUTTON_8, 15, 135, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button8", ID_BUTTON_9, 75, 135, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button9", ID_BUTTON_10, 135, 135, 50, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-ok", ID_BUTTON_11, 195, 135, 50, 110, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button0", ID_BUTTON_12, 15, 195, 110, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button-point", ID_BUTTON_13, 135, 195, 50, 50, 0, 0x0, 0 },
};

static void MakeStrNumber(char *number)
{
    strcat(strNumber,number);
}


static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, "");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBitmap(hItem,&BitmapBeijing);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
        CHECKBOX_SetSkin(hItem, SKIN_checkbox);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
        CHECKBOX_SetSkin(hItem, SKIN_checkbox);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
        CHECKBOX_SetSkin(hItem, SKIN_checkbox);
        
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_CHECKBOX_0:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                WM_ShowWindow(HwinKeyboard);
                memset(strNumber, '\0', sizeof(strNumber));
                Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
                CHECKBOX_SetState(hItem, 1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
                CHECKBOX_SetState(hItem, 0);
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
                WM_ShowWindow(HwinKeyboard);
                memset(strNumber,'\0', sizeof(strNumber));
                Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
                CHECKBOX_SetState(hItem, 1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
                CHECKBOX_SetState(hItem, 0);

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
                WM_ShowWindow(HwinKeyboard);
                memset(strNumber, '\0', sizeof(strNumber));
                Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
                CHECKBOX_SetState(hItem, 1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
                CHECKBOX_SetState(hItem, 0);

                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        case ID_CHECKBOX_3:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                WM_HideWindow(HwinKeyboard);
                memset(strNumber, '\0', sizeof(strNumber));
                Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
                CHECKBOX_SetState(hItem, 1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
                CHECKBOX_SetState(hItem, 0);
                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        }
        break;
    case WM_PAINT: 
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialog1(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
        IMAGE_SetBitmap(hItem, &BitmapKeyboardback);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard1press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard1, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard2press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard2, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard3press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard3, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboardBackspacepress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboardBackspace, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard4press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard4, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard5press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard5, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard6press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard6, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboardescpress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboardesc, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_8);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard7press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard7, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_9);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard8press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard8, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_10);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard9press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard9, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_11);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboardokpress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboardok, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_12);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboard0press, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboard0, 0, 0);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_13);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapKeyboardpointpress, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapKeyboardpoint, 0, 0);  
        
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: // Notifications sent by 'Button1'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("1");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_1: // Notifications sent by 'Button2'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("2");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_2: // Notifications sent by 'Button3'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("3");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_3: // Notifications sent by 'Button-b'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                //MakeStrNumber("3");
                break;
            case WM_NOTIFICATION_RELEASED:
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_4: // Notifications sent by 'Button4'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("4");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_5: // Notifications sent by 'Button5'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("5");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_6: // Notifications sent by 'Button6'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("6");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_7: // Notifications sent by 'Button-cancel'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                //MakeStrNumber("7");
                break;
            case WM_NOTIFICATION_RELEASED:
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_8: // Notifications sent by 'Button7'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("7");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_9: // Notifications sent by 'Button8'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("8");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_10: // Notifications sent by 'Button9'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("9");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_11: // Notifications sent by 'Button-ok'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
              // USER START (Optionally insert code for reacting on notification message)
              // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_12: // Notifications sent by 'Button0'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("0");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
        case ID_BUTTON_13: // Notifications sent by 'Button-point'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber(".");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                break;
  // USER START (Optionally insert additional code for further notification handling)
  // USER END
            }
            break;
// USER START (Optionally insert additional code for further Ids)
// USER END
        }
        break;
      // USER START (Optionally insert additional message handling)
      // USER END
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
    *       CreateKeyBoard-Window
    */
WM_HWIN CreateKeyBoardWindow(void);
WM_HWIN CreateKeyBoardWindow(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    HwinKeyboard = GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), _cbDialog1, hWin, 0, 0);
    WM_HideWindow(HwinKeyboard);
    return hWin;  
}

    // USER START (Optionally insert additional public code)
    // USER END

    /*************************** End of file ****************************/
