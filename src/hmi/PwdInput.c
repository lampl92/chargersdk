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
#define ID_Timerstateflash 1

static WM_HTIMER _timerstateflash;

#define  INPUTNUMBERLENGTH 7

WM_HWIN HwinKeyboard;
static char strNumber[INPUTNUMBERLENGTH];

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Select-Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    //408+5,95,260,56
    { TEXT_CreateIndirect, "inputInfo", ID_TEXT_0, 413, 95, 260, 56, TEXT_CF_HCENTER | TEXT_CF_VCENTER, 0x0, 0 },
    { BUTTON_CreateIndirect, "quit", ID_BUTTON_14, 51, 422, 147, 52, 0, 0x0, 10 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate1[] = {
    { WINDOW_CreateIndirect, "KeyBoard-Window", ID_WINDOW_1, keyboardx, keyboardy, 275, 242, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Button0", ID_BUTTON_0, 0, 0, 275, 242, 0, 0x0, 2 },
};

static char* getbutton(int id)
{
    switch (id)
    {
    case 1:
        return "1";
        break;
    case 2:
        return "2";
        break;
    case 3:
        return "3";
        break;
    case 4:
        return "bs";
        break;
    case 5:
        return "4";
        break;
    case 6:
        return "5";
        break;
    case 7:
        return "6";
        break;
    case 8:
        return "esc";
        break;
    case 9:
        return "7";
        break;
    case 10:
        return "8";
        break;
    case 11:
        return "9";
        break;
    case 12:
        return "ok";
        break;
    case 13:
        return "0";
        break;
    case 14:
        return ".";
        break;
    default:
        return NULL;
    }
}

static void MakeStrNumber(char *number)
{
    int i = 0;
    if (strcmp(number, "ok") == 0)
    {
        return;
    }
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
        if (strlen(strNumber) < INPUTNUMBERLENGTH - 1)
        {
            strcat(strNumber, number); 
        }
        return;
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg36e, 0xFF08263F, "");
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), 0xFF08263F);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg36e);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14);
        BUTTON_SetSkin(hItem, SKIN_buttonPwdQuit);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break; 
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevselectpatternback, 0, 0);
        GUI_MEMDEV_WriteAt(MemdevPwdPromptPicture, 106, 141);
        GUI_MEMDEV_WriteAt(MemdevPwdInputBox, yuandufenx, yuandufeny);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_14: //'quit'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                break;
            case WM_NOTIFICATION_RELEASED:
                quitflag = 1;
                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerstateflash)
        {
            if (gbsstate == StateTestPwd)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPStateTestPwd);
            }
            if (gbsstate == StateHome)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    case MSG_JUMPStateTestPwd:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatePwdTest();
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        home();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialog1(WM_MESSAGE * pMsg) {
    int NCode;
    int i,j;
    int Id;
    WM_HWIN hItem;
    I16 skbID;
    char str[7];

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
                for (i = 0; strNumber[i] != '\0'; i++)
                {
                    ;   
                }
                if (strcmp(getbutton(skbID), "ok") == 0 && i != 0)
                {
                    memcpy(Tempuserlike.user_like.strPwd, strNumber, 7);
                    Tempuserlike.UserLikeFlag = 1;
                }
                else
                {
                    MakeStrNumber(getbutton(skbID));
                    for (i = 0; strNumber[i] != '\0'; i++)
                    {
                        str[i] = '*';
                    }
                    str[i] = '\0';
                    Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &fontwryhcg36e, 0xFF08263F, str);
                }
                break;
            }
            break;
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

//CreateKeyBoard-Window
WM_HWIN CreatePwdInput(void);
WM_HWIN CreatePwdInput(void) {
    WM_HWIN hWin;
    strNumber[0] = '\0';
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 100, 0);
    HwinKeyboard = GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), _cbDialog1, hWin, 0, 0);
    //WM_HideWindow(HwinKeyboard);
    return hWin;  
}

