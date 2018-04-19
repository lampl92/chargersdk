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

#define  INPUTNUMBERLENGTH 6

WM_HWIN HwinKeyboard;
static char strNumber[INPUTNUMBERLENGTH];

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Select-Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
//    { IMAGE_CreateIndirect, "selectpatternback", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
//    { IMAGE_CreateIndirect, "yuandufen", ID_IMAGE_2, 630, 75, 30, 30, 0, 0, 0 },
    { TEXT_CreateIndirect, "inputInfo", ID_TEXT_0, 410, 100, 100, 36, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxfull", ID_CHECKBOX_0, Checkboxfullx, Checkboxfully, 249, 74, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxtime", ID_CHECKBOX_1, Checkboxtimex, Checkboxtimey, 249, 74, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxmoney", ID_CHECKBOX_2, Checkboxmonex, Checkboxmoney, 249, 74, 0, 0x0, 0 },
    { CHECKBOX_CreateIndirect, "Checkboxelectric", ID_CHECKBOX_3, Checkboxelectricx, Checkboxelectricy, 249, 74, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "Buttonquit", ID_BUTTON_14, backquitx, backquity, 171, 59, 0, 0x0, 0 },
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
    for (i = 0; strNumber[i] != '\0'; i++);
    if ((i == 0)&&((strcmp(number, ".") == 0) || (strcmp(number, "0") == 0)))
    {
        return;
    }
    if (strcmp(number, "ok") == 0)
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

static void RADIO(WM_HWIN hItem1, WM_HWIN hItem2, WM_HWIN hItem3, WM_HWIN hItem4)
{
    CHECKBOX_SetState(hItem1, 1);
    CHECKBOX_SetState(hItem2, 0);
    CHECKBOX_SetState(hItem3, 0);
    CHECKBOX_SetState(hItem4, 0);
}

static void paintyuandufen(WM_MESSAGE * pMsg)
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (CHECKBOX_GetState(WM_GetDialogItem(pMsg->hWin, GUI_ID_USER + 0x12 + i)) == 1)
        {
            break;
        }
    }
    switch (i)
    {
    case 0:
        GUI_MEMDEV_WriteAt(Memdevselectpatternunitno, yuandufenx, yuandufeny);
        break;
    case 1:
        GUI_MEMDEV_WriteAt(Memdevselectpatternunitfen, yuandufenx, yuandufeny);
        break;
    case 2:
        GUI_MEMDEV_WriteAt(Memdevselectpatternunityuan, yuandufenx, yuandufeny);
        break;
    case 3:
        GUI_MEMDEV_WriteAt(Memdevselectpatternunitdu, yuandufenx, yuandufeny);
        break;
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    int NCode;
    int Id;
    WM_HWIN hItem;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF36_Font, GUI_BLACK, "");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxfull);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxtime);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxmoney);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
        CHECKBOX_SetSkin(hItem, SKIN_checkboxelectric);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_14);
        BUTTON_SetSkin(hItem, SKIN_buttonbackquit);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break; 
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevselectpatternback, 0, 0);
        paintyuandufen(pMsg);
        break;
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
                RADIO(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3));   
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_RELEASED:
                Tempuserlike.user_like.dLimitFee = 0;
                Tempuserlike.user_like.dLimitEnergy = 0;
                Tempuserlike.user_like.ulLimitTime = 0;
                Tempuserlike.UserLikeFlag = 1;
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
                RADIO(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0),\
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3));              
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
                WM_ShowWindow(HwinKeyboard);
                memset(strNumber, '\0', sizeof(strNumber));
                Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
                RADIO(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3));              
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
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
                RADIO(WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1),\
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2), \
                    WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0));
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
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
    case WM_TIMER:
        if (pMsg->Data.v == _timerstateflash)
        {
            if (gbsstate == StateReadyStart)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_READYSTART);
            }
            if (gbsstate == StateHome)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPHOME);
            }
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    case MSG_READYSTART:
        GUI_EndDialog(pMsg->hWin, 0);
        CreatereadystartDLG();
        break;
    case MSG_JUMPSELECTGUN:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateselectgunDLG();
        break;
    case MSG_JUMPHOME:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateHomeDLG();
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
                if (strcmp(getbutton(skbID), "ok") == 0 && i != 0)
                {
                    for (i = 0; i < 3; i++)
                    {
                        if (CHECKBOX_GetState(WM_GetDialogItem(WM_GetParent(pMsg->hWin), GUI_ID_USER + 0x13 + i)) == 1)
                        {
                            break;
                        }
                    }
                    switch (i)
                    {
                    case 0:
                        GUI_MEMDEV_WriteAt(Memdevselectpatternunitfen, yuandufenx, yuandufeny);
                        Tempuserlike.user_like.dLimitFee = 0;
                        Tempuserlike.user_like.dLimitEnergy = 0;
                        Tempuserlike.user_like.ulLimitTime = atof(strNumber) * 60;
                        Tempuserlike.UserLikeFlag = 1;
                        break;
                    case 1:
                        GUI_MEMDEV_WriteAt(Memdevselectpatternunityuan, yuandufenx, yuandufeny);
                        Tempuserlike.user_like.dLimitFee = atof(strNumber);
                        Tempuserlike.user_like.dLimitEnergy = 0;
                        Tempuserlike.user_like.ulLimitTime = 0;
                        Tempuserlike.UserLikeFlag = 1;
                        break;
                    case 2:
                        GUI_MEMDEV_WriteAt(Memdevselectpatternunitdu, yuandufenx, yuandufeny);
                        Tempuserlike.user_like.dLimitFee = 0;
                        Tempuserlike.user_like.dLimitEnergy = atof(strNumber);
                        Tempuserlike.user_like.ulLimitTime = 0;
                        Tempuserlike.UserLikeFlag = 1;
                        break;
                    }
                }
                else
                {
                    MakeStrNumber(getbutton(skbID));
                    Text_Show(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_TEXT_0), &SIF24_Font, GUI_RED, strNumber);
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
WM_HWIN CreateselectpatternbetterDLG(void);
WM_HWIN CreateselectpatternbetterDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 100, 0);
    HwinKeyboard = GUI_CreateDialogBox(_aDialogCreate1, GUI_COUNTOF(_aDialogCreate1), _cbDialog1, hWin, 0, 0);
    WM_HideWindow(HwinKeyboard);
    return hWin;  
}

