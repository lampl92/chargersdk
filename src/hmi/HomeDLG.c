#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_BUTTON_0 (GUI_ID_USER + 0x06)
#define ID_BUTTON_1 (GUI_ID_USER + 0x07)
#define ID_TEXT_1 (GUI_ID_USER + 0x08)
#define ID_TEXT_2 (GUI_ID_USER + 0x09)
#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)

//枪状态，信号状态，价格状态
#define ID_Timergunstateflash           1
#define ID_Timersignalstateflash        2
#define ID_Timerpriceflash              3
#define ID_Timertimeflash               4

static WM_HTIMER _timergunstateflash, _timersignalstateflash, _timerpriceflash,_timertimeflash;

static int i = 1;//函数临时使用

int gunbuttonax = 185;//画按键皮肤时使用
int gunbuttonay = 280;//画按键皮肤时使用

int gunbuttonbx = 445;//画按键皮肤时使用
int gunbuttonby = 280;//画按键皮肤时使用

static int gunstateax = 185;//枪A状态图标x位置
static int gunstateay = 100;//枪A状态图标y位置

static int gunstatebx = 445;//枪B状态图标x位置
static int gunstateby = 100;//枪B状态图标x位置

static int signalx = 755;//信号位置x
static int signaly = 3;//信号位置y

int SignalIntensity;//信号强度
int PreSignalIntensity;//之前的信号强度


static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Home", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 14, 9, 240, 24, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun1infobutton", ID_BUTTON_0, 185, 280, 170, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun2infobutton", ID_BUTTON_1, 445, 280, 170, 50, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "electricFeetext", ID_TEXT_1, 264, 365, 100, 44, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "severFeetext", ID_TEXT_2, 484, 365, 100, 44, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 40, 120, 400, 0, 0x0, 0 },
};

static void updategunState(WM_MESSAGE * pMsg)//枪状态刷新函数
{
    WM_HWIN      hItem;
    if (gunstate[0] == GunfreeState)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunAfree, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAscancode);
    }
    else if (gunstate[0] == GunchargingState)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunAcharging, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
    }
    else if (gunstate[0] == GunchargedoneState)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunAchargedone, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
    }
    else if (gunstate[0] == Gunerror)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunAerror, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAerror);
    }
            
    if (gunstate[1] == GunfreeState)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunBfree, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBscancode);
    }
    else if (gunstate[1] == GunchargingState)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunBcharging, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
    }
    else if (gunstate[1] == GunchargedoneState)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunBchargedone, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
    }
    else if (gunstate[1] == Gunerror)
    {
        GUI_MEMDEV_WriteAt(MemdevhomegunBerror, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBerror);
    }
}

static void updatesignal(WM_MESSAGE * pMsg)//信号状态刷新函数
{
    SignalIntensity = getSignalIntensity();
    if (SignalIntensity == 1)
    {
        GUI_MEMDEV_WriteAt(Memdevhomesignal1, signalx, signaly);
    }
    else if (SignalIntensity == 2)
    {
        GUI_MEMDEV_WriteAt(Memdevhomesignal2, signalx, signaly);
    }
    else if (SignalIntensity == 3)
    {
        GUI_MEMDEV_WriteAt(Memdevhomesignal3, signalx, signaly);
    }
    else if (SignalIntensity == 4)
    {
        GUI_MEMDEV_WriteAt(Memdevhomesignal4, signalx, signaly);
    }
    else if (SignalIntensity == 5)
    {
        GUI_MEMDEV_WriteAt(Memdevhomesignal5, signalx, signaly);
    }
    else
    {
        GUI_MEMDEV_WriteAt(Memdevhomesignal0, signalx, signaly);
    }
    PreSignalIntensity = SignalIntensity;
}

static void datetime_Show(WM_MESSAGE *pMsg, uint16_t ID_TEXT)
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    WM_HWIN hWin = pMsg->hWin;
    uint8_t Date_buf[30];
    uint8_t Time_buf[30];
    uint8_t date_time_buf[100];

    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
    sprintf((char *)Date_buf, "20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);

    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    sprintf((char *)Time_buf, "%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    
    sprintf((char*)date_time_buf, "%s   %s", Date_buf, Time_buf);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT), date_time_buf);
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;
    uint8_t pos = 0;
    uint8_t ucSegState;
    time_t  now;
    uint8_t strPowerFee[10];
    uint8_t strServiceFee[10];

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        gunstate[0] = 1;
        gunstate[1] = 1;
        SignalIntensity = getSignalIntensity();
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg24e, GUI_BLACK, "");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &fontwryhcg24e, GUI_BLACK, "");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &fontwryhcg24e, GUI_BLACK, "");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetFocussable(hItem,0);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetFocussable(hItem,0);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetSkin(hItem, SKIN_buttontest);

        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break;              
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevhomeback, 0, 0);
        updategunState(pMsg);
        updatesignal(pMsg);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'gun1infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:
                if (gunstate[0] == GunchargingState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatecharginginfoDLG();
                }
                else if (gunstate[0] == GunchargedoneState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatechargedoneinfoDLG();
                }
                break;
            }
            break;
        case ID_BUTTON_1: //'gun2infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
          
                break;
            case WM_NOTIFICATION_RELEASED:
                if (gunstate[1] == GunchargingState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatecharginginfoDLG();
                }
                else if (gunstate[1] == GunchargedoneState)
                {
                    GUI_EndDialog(pMsg->hWin, 0);
                    CreatechargedoneinfoDLG();
                }
                break;
            }
            break;
        case ID_BUTTON_2: //'testButton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:

                break;
            case WM_NOTIFICATION_RELEASED:
                GUI_EndDialog(pMsg->hWin, 0);
                CreateselectgunDLG();
                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timergunstateflash)
        {           
            //枪状态更新
            if (i > 4 || i < 1) i = 1;
            gunstate[0] = i;
            gunstate[1] = i;
            i++;
            SignalIntensity = i;
            //WM_SendMessageNoPara(pMsg->hWin, WM_PAINT);
            WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            WM_RestartTimer(pMsg->Data.v, 4000);
        }
        else if (pMsg->Data.v == _timersignalstateflash)
        {
           //信号的更新
            SignalIntensity = getSignalIntensity();
            if (SignalIntensity != PreSignalIntensity)
            {
                //WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                PreSignalIntensity = SignalIntensity;
            }
            WM_RestartTimer(pMsg->Data.v, 1000);
        }
        else if (pMsg->Data.v == _timerpriceflash)
        {
            now = time(NULL);
            extern OrderSegState_e JudgeSegState(time_t now, echProtocol_t *pProto, uint8_t *ppos);
            ucSegState = (uint8_t)JudgeSegState(now, pechProto, &pos);
            sprintf(strPowerFee, "%5.1f", pechProto->info.dSegPowerFee[ucSegState]);
            sprintf(strServiceFee, "%5.1f", pechProto->info.dSegServFee[ucSegState]);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), strPowerFee);/**< 充电费*/
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), strServiceFee);/**< 服务费 */  
            WM_RestartTimer(pMsg->Data.v, 1000);
        }
        else if (pMsg->Data.v = _timertimeflash)
        {
            datetime_Show(pMsg, ID_TEXT_0);
            WM_RestartTimer(pMsg->Data.v, 200);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreateHomeDLG(void);
WM_HWIN CreateHomeDLG(void) {
    WM_HWIN hWin;
    hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    _timergunstateflash = WM_CreateTimer(hWin, ID_Timergunstateflash, 4000, 0);
    _timersignalstateflash = WM_CreateTimer(hWin, ID_Timersignalstateflash, 1000, 0);
    _timerpriceflash = WM_CreateTimer(hWin, ID_Timerpriceflash, 2000, 0);
    _timertimeflash = WM_CreateTimer(hWin,ID_Timertimeflash, 200, 0);
    return hWin;
}
