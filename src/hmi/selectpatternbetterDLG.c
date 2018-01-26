#include "DIALOG.h"
#include "siffontcreate.h"
#include "skin.h"
#include  "HMI_Start.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)

#define ID_WINDOW_1 (GUI_ID_USER + 0x0F)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_CHECKBOX_0 (GUI_ID_USER + 0x12)
#define ID_CHECKBOX_1 (GUI_ID_USER + 0x13)
#define ID_CHECKBOX_2 (GUI_ID_USER + 0x14)
#define ID_CHECKBOX_3 (GUI_ID_USER + 0x15)
#define ID_TEXT_0 (GUI_ID_USER + 0x16)
#define ID_IMAGE_0 (GUI_ID_USER + 0x17)
#define ID_IMAGE_2 (GUI_ID_USER + 0x19)
#define ID_BUTTON_14 (GUI_ID_USER + 0x20)

#define  INPUTNUMBERLENGTH 6

WM_HWIN HwinKeyboard;
static char strNumber[INPUTNUMBERLENGTH];

int keyboardx = 480;
int keyboardy = 155;

int backquitx = 280;
int backquity = 370;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Select-Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "selectpatternback", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "yuandufen", ID_IMAGE_2, 630, 75, 30, 30, 0, 0, 0 },
    { TEXT_CreateIndirect, "inputInfo", ID_TEXT_0, 530, 72, 120, 36, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxfull", ID_CHECKBOX_0, 180, 50, 150, 40, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxtime", ID_CHECKBOX_1, 180, 110, 150, 40, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxmoney", ID_CHECKBOX_2, 180, 170, 150, 40, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxelectric", ID_CHECKBOX_3, 180, 230, 150, 40, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Buttonquit", ID_BUTTON_14, 280, 370, 100, 50, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate1[] = {
    { WINDOW_CreateIndirect, "KeyBoard-Window", ID_WINDOW_1, 480, 155, 260, 260, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button0", ID_BUTTON_0, 0, 0, 260, 260, 0, 0x0, 2 },
};

static char* getbutton(int id)
{
    if (id == 0)
    {
        return "1";
    }
    else if (id == 1)
    {
        return "2";
    }
    else if (id == 2)
    {
        return "3";
    }
    else if (id == 3)
    {
        return "bs";
    }
    else if (id == 4)
    {
        return "4";
    }
    else if (id == 5)
    {
        return "5";
    }
    else if (id == 6)
    {
        return "6";
    }
    else if (id == 7)
    {
        return "esc";
    }
    else if (id == 8)
    {
        return "7";
    }
    else if (id == 9)
    {
        return "8";
    }
    else if (id == 10)
    {
        return "9";
    }
    else if (id == 11)
    {
        return "ok";
    }
    else if (id == 12)
    {
        return "0";
    }
    else if (id == 13)
    {
        return ".";
    }
    return NULL;
}

static void MakeStrNumber(char *number)
{
    int i = 0;
    for (i = 0; strNumber[i] != '\0'; i++);
    if ((i == 0)&&((strcmp(number, ".") == 0) || (strcmp(number, "0") == 0)))
    {
        return;
    }
    if (strcmp(number, "bs") == 0)
    {
        for (i = 0; strNumber[i] != '\0'; i++);
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
    int i;
    int Id;
    WM_HWIN hItem;
    I16 skbID;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:    
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttonSKB);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'Button0'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                BUTTON_GetUserData(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), &skbID, 2);
                for (i = 0; strNumber[i] != '\0'; i++);
                if (strcmp(getbutton(skbID), "ok") == 0 && i!=0)
                {
                    WM__SendMessageNoPara(pMsg->hWin, MSG_JUMPCARDINFO);
                }
                MakeStrNumber(getbutton(skbID));
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
WM_HWIN CreateselectpatternbetterDLG(void);
WM_HWIN CreateselectpatternbetterDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    HwinKeyboard = GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), _cbDialog1, hWin, 0, 0);
    WM_HideWindow(HwinKeyboard);
    return hWin;  
}

