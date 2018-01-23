#include "DIALOG.h"
#include "siffontcreate.h"
#include "skin.h"
#include  "HMI_Start.h"

#define  INPUTNUMBERLENGTH 6

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
#define ID_IMAGE_2 (GUI_ID_USER + 0x19)
#define ID_BUTTON_14 (GUI_ID_USER + 0x20)

WM_HWIN HwinKeyboard;
static char strNumber[INPUTNUMBERLENGTH];

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Select-Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "selectpatternback", ID_IMAGE_0, 0, 0, 800, 480, IMAGE_CF_MEMDEV, 0, 0 },
    { IMAGE_CreateIndirect, "yuandufen", ID_IMAGE_2, 630, 75, 30, 30, IMAGE_CF_MEMDEV, 0, 0 },
    { TEXT_CreateIndirect, "inputInfo", ID_TEXT_0, 530, 72, 120, 36, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxfull", ID_CHECKBOX_0, 180, 50, 150, 40, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxtime", ID_CHECKBOX_1, 180, 110, 150, 40, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxmoney", ID_CHECKBOX_2, 180, 170, 150, 40, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxelectric", ID_CHECKBOX_3, 180, 230, 150, 40, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Buttonquit", ID_BUTTON_14, 100, 400, 100, 50, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate1[] = {
    { WINDOW_CreateIndirect, "KeyBoard-Window", ID_WINDOW_1, 400, 110, 260, 260, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "SelectInfoImage", ID_IMAGE_1, 0, 0, 260, 260, 0, 0, 0 },
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
    int i = 0;

    if (strcmp(number, "bs") == 0)
    {
        for (i = 0; strNumber[i] != '\0'; i++)
            ;
        strNumber[i - 1] = '\0';
    }
    else if (strcmp(number, "esc") == 0)
    {
        memset(strNumber, '\0', sizeof(strNumber));
    }
    else
    {
        if (strlen(strNumber) < INPUTNUMBERLENGTH-1)
        {
            if (strcmp(number, ".") == 0)
            {       
                for (i = 0; strNumber[i] != '\0'; i++)
                {
                    if (strNumber[i] == '.')
                    {
                        return;
                    }
                }
            }
            strcat(strNumber, number); 
        }
    }
}


static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF36_Font, GUI_BLACK, "");

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBitmap(hItem, &Bitmapselectpatternback);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
        IMAGE_SetBitmap(hItem, &Bitmapselectpatternunitno);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxfull);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxtime);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxmoney);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxelectric);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14);
        BUTTON_SetText(hItem, "");
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_PRESSED, &BitmapbackQuit, 0, 0);
        BUTTON_SetBitmapEx(hItem, BUTTON_BI_UNPRESSED, &BitmapbackQuitPress, 0, 0);
        
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_CHECKBOX_0:
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                WM_HideWindow(HwinKeyboard);
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
                hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
                IMAGE_SetBitmap(hItem, &Bitmapselectpatternunitno);
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
                memset(strNumber, '\0', sizeof(strNumber));
                Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);

                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
                CHECKBOX_SetState(hItem, 1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
                CHECKBOX_SetState(hItem, 0);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
                IMAGE_SetBitmap(hItem, &Bitmapselectpatternunitfen);
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
                hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
                IMAGE_SetBitmap(hItem, &Bitmapselectpatternunityuan);
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
                WM_ShowWindow(HwinKeyboard);
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
                hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
                IMAGE_SetBitmap(hItem, &Bitmapselectpatternunitdu);
                break;
            case WM_NOTIFICATION_RELEASED:
                break;
            case WM_NOTIFICATION_VALUE_CHANGED:
                break;
            }
            break;
        case ID_BUTTON_14: //'quit'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPSELECTGUN);
                break;
            }
            break;
        }
        break;
    case MSG_JUMPSELECTGUN:
        GUI_EndDialog(pMsg->hWin, 0);
        GUI_EndDialog(HwinKeyboard, 0);
        CreateselectgunDLG();
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
        case ID_BUTTON_0: //'Button1'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("1");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_1: //'Button2'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("2");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_2: //'Button3'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("3");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_3: //'Button-b'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("bs");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_4: //'Button4'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("4");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_5: //'Button5'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("5");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_6: //'Button6'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:               
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("6");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_7: //'Button-cancel'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("esc");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_8: //'Button7'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("7");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_9: //'Button8'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("8");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_10: //'Button9'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("9");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_11: //'Button-ok'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                WM__SendMessageNoPara(pMsg->hWin, MSG_JUMPCARDINFO);
                break;
            }
            break;
        case ID_BUTTON_12: //'Button0'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber("0");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        case ID_BUTTON_13: //'Button-point'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:                
                break;
            case WM_NOTIFICATION_RELEASED:
                MakeStrNumber(".");
                Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                break;
            }
            break;
        }
        break;
    case MSG_JUMPCARDINFO:
        GUI_EndDialog(WM_GetParent(pMsg->hWin), 0);
        CreateCardInfoDLG();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

//CreateKeyBoard-Window
WM_HWIN CreateselectpatternDLG(void);
WM_HWIN CreateselectpatternDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    HwinKeyboard = GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), _cbDialog1, hWin, 0, 0);
    WM_HideWindow(HwinKeyboard);
    return hWin;  
}

