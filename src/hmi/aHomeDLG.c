#include "DIALOG.h"
#include "HMI_Start.h"
#include "touchtimer.h"
#include "GUI_backstage.h"
#include "homedataflashbackstage.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_TEXT_0 (GUI_ID_USER + 0x02)
#define ID_BUTTON_0 (GUI_ID_USER + 0x06)
#define ID_BUTTON_1 (GUI_ID_USER + 0x07)
#define ID_TEXT_1 (GUI_ID_USER + 0x08)
#define ID_TEXT_2 (GUI_ID_USER + 0x09)
#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)
#define ID_BUTTON_3 (GUI_ID_USER + 0x0B)
#define ID_WINDOW_1 (GUI_ID_USER + 0x0C)
#define ID_TEXT_3 (GUI_ID_USER + 0x0D)
#define ID_TEXT_4 (GUI_ID_USER + 0x0E)
#define ID_TEXT_5 (GUI_ID_USER + 0x0F)
//枪状态，信号状态，价格状态
#define ID_Timergunstateflash           1
#define ID_Timersignalstateflash        2
#define ID_Timerpriceflash              3
#define ID_Timertimeflash               4
#define gunstateax  185 //枪A状态图标x位置
#define gunstateay  100 //枪A状态图标y位置
#define gunstatebx  445 //枪B状态图标x位置
#define gunstateby  100 //枪B状态图标y位置
#define signalx  755 //信号位置x
#define signaly  3 //信号位置y
#define infoAx 140 //A枪充电信息位置x
#define infoAy 90 //A枪充电信息位置y
#define infoBx 400 //B枪充电信息位置x
#define infoBy 90 //B枪充电信息位置y
#define helpinfox 165
#define helpinfoy 70

static WM_HWIN      Hwininfo;
static WM_HTIMER _timergunstateflash, _timersignalstateflash, _timerpriceflash,_timertimeflash;
static int helpflag;//提示帮助标志位
static int i = 1;//函数临时使用
int SignalIntensity;//信号强度
int PreSignalIntensity;//之前的信号强度

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Home", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 14, 9, 240, 24, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun1infobutton", ID_BUTTON_0, 185, 280, 170, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun2infobutton", ID_BUTTON_1, 445, 280, 170, 50, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "electricFeetext", ID_TEXT_1, 264, 384, 100, 44, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "severFeetext", ID_TEXT_2, 484, 384, 100, 44, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 40, 120, 400, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "help", ID_BUTTON_3, helpbuttonx, helpbuttony, 170, 70, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreateinfo[] = {
    { WINDOW_CreateIndirect, "KeyBoard-Window", ID_WINDOW_1, 140, 90, 260, 195, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "chargingfee", ID_TEXT_3, 14, 9, 240, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "chargingcurrent", ID_TEXT_4, 14, 9, 240, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "chargingpower", ID_TEXT_5, 14, 9, 240, 24, 0, 0x0, 0 },
};


static void updategunState(WM_MESSAGE * pMsg)//枪状态刷新函数
{
    WM_HWIN      hItem;
    switch (gunstate[0])
    {
    case GunfreeState:
        GUI_MEMDEV_WriteAt(MemdevhomegunAfree, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAscancode);
        break;
    case GunchargingState:
        GUI_MEMDEV_WriteAt(MemdevhomegunAcharging, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
        break;
    case GunchargedoneState:
        GUI_MEMDEV_WriteAt(MemdevhomegunAchargedone, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
        break;
    case Gunerror:
        GUI_MEMDEV_WriteAt(MemdevhomegunAerror, gunstateax, gunstateay);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
        BUTTON_SetSkin(hItem, SKIN_buttongunAerror);
        break;
    }
    switch (gunstate[1])
    {
    case GunfreeState:
        GUI_MEMDEV_WriteAt(MemdevhomegunBfree, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBscancode);
        break;
    case GunchargingState:
        GUI_MEMDEV_WriteAt(MemdevhomegunBcharging, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
        break;
    case GunchargedoneState:
        GUI_MEMDEV_WriteAt(MemdevhomegunBchargedone, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
        break;
    case Gunerror:
        GUI_MEMDEV_WriteAt(MemdevhomegunBerror, gunstatebx, gunstateby);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
        BUTTON_SetSkin(hItem, SKIN_buttongunBerror);
        break;
    }
}

static void updatesignal(WM_MESSAGE * pMsg)//信号状态刷新函数
{
    SignalIntensity = getSignalIntensity();
    switch (SignalIntensity)
    {
    case 1:
        GUI_MEMDEV_WriteAt(Memdevhomesignal1, signalx, signaly);
        break;
    case 2:
        GUI_MEMDEV_WriteAt(Memdevhomesignal2, signalx, signaly);
        break;
    case 3:
        GUI_MEMDEV_WriteAt(Memdevhomesignal3, signalx, signaly);
        break;
    case 4:
        GUI_MEMDEV_WriteAt(Memdevhomesignal4, signalx, signaly);
        break;
    case 5:
        GUI_MEMDEV_WriteAt(Memdevhomesignal5, signalx, signaly);
        break;
    default:
        GUI_MEMDEV_WriteAt(Memdevhomesignal0, signalx, signaly);
        break;
    }
    PreSignalIntensity = SignalIntensity;
}

static void updateprice(WM_MESSAGE * pMsg, int idpowerfee, int idservidefee, const GUI_FONT * pFont)
{
    uint8_t pos = 0;
    uint8_t ucSegState;
    time_t  now;
    uint8_t strPowerFee[10];
    uint8_t strServiceFee[10];
    now = time(NULL);
    extern OrderSegState_e JudgeSegState(time_t now, echProtocol_t *pProto, uint8_t *ppos);
    ucSegState = (uint8_t)JudgeSegState(now, pechProto, &pos);
    sprintf(strPowerFee, "%5.1f", pechProto->info.dSegPowerFee[ucSegState]);
    sprintf(strServiceFee, "%5.1f", pechProto->info.dSegServFee[ucSegState]);
    TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, idpowerfee), pFont);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, idpowerfee), strPowerFee);/**< 充电费*/
    TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, idservidefee), pFont);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, idservidefee), strServiceFee);/**< 服务费 */  
}

static void updatedatetime(WM_MESSAGE *pMsg, uint16_t ID_TEXT, const GUI_FONT * pFont)
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
    TEXT_SetFont(WM_GetDialogItem(hWin, ID_TEXT), pFont);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT), date_time_buf);
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        helpflag = 0;
        gunstate[0] = 1;
        gunstate[1] = 1;
        SignalIntensity = getSignalIntensity();
        updatedatetime(pMsg, ID_TEXT_0,&fontwryhcg24e);
        updateprice(pMsg, ID_TEXT_1, ID_TEXT_2, &fontwryhcg24e);
        
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//        BUTTON_SetFocussable(hItem,0);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//        BUTTON_SetFocussable(hItem,0);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
        BUTTON_SetSkin(hItem, SKIN_buttontest);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
        BUTTON_SetSkin(hItem, SKIN_buttonhelp);

        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break;              
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevhomeback, 0, 0);
        updategunState(pMsg);
        updatesignal(pMsg);
        if (helpflag == 1)
        {
            GUI_MEMDEV_WriteAt(Memdevhomehelpinfo, helpinfox, helpinfoy);
        }
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'gun1infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                if (gunstate[0] == GunchargingState || gunstate[0] == GunchargedoneState)
                {
                    WM_MoveChildTo(Hwininfo, infoAx, infoAy);
                    WM_ShowWin(Hwininfo);   
                }
                break;
            case WM_NOTIFICATION_RELEASED:
//                if (gunstate[0] == GunchargingState)
//                {
//                    GUI_EndDialog(pMsg->hWin, 0);
//                    CreatecharginginfoDLG();
//                }
//                else if (gunstate[0] == GunchargedoneState)
//                {
//                    GUI_EndDialog(pMsg->hWin, 0);
//                    CreatechargedoneinfoDLG();
//                }
                WM_HideWin(Hwininfo);
                break;
            case WM_NOTIFICATION_MOVED_OUT:
                WM_HideWin(Hwininfo);
                break;
            }
            break;
        case ID_BUTTON_1: //'gun2infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                if (gunstate[1] == GunchargingState || gunstate[1] == GunchargedoneState)
                {
                    WM_MoveChildTo(Hwininfo, infoBx, infoBy);
                    WM_ShowWin(Hwininfo);   
                }
                break;
            case WM_NOTIFICATION_RELEASED:
//                if (gunstate[1] == GunchargingState)
//                {
//                    GUI_EndDialog(pMsg->hWin, 0);
//                    CreatecharginginfoDLG();
//                }
//                else if (gunstate[1] == GunchargedoneState)
//                {
//                    GUI_EndDialog(pMsg->hWin, 0);
//                    CreatechargedoneinfoDLG();
//                }
                WM_HideWin(Hwininfo);
            case WM_NOTIFICATION_MOVED_OUT:
                WM_HideWin(Hwininfo);
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
        case ID_BUTTON_3: //'quit'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                helpflag = 1;
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
                WM_HideWin(hItem);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
                WM_HideWin(hItem);
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_RELEASED:
                helpflag = 0;
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
                WM_ShowWin(hItem);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
                WM_ShowWin(hItem);
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                break;
            case WM_NOTIFICATION_MOVED_OUT:
                helpflag = 0;
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
                WM_ShowWin(hItem);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
                WM_ShowWin(hItem);
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
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
            updateprice(pMsg, ID_TEXT_1, ID_TEXT_2, &fontwryhcg24e);
            WM_RestartTimer(pMsg->Data.v, 1000);
        }
        else if (pMsg->Data.v == _timertimeflash)
        {
            if (!(gunstate[1] == GunchargingState || gunstate[1] == GunchargedoneState \
                || gunstate[0] == GunchargingState || gunstate[0] == GunchargedoneState))
            {
                WM_HideWin(Hwininfo);
            }
            updatedatetime(pMsg, ID_TEXT_0, &fontwryhcg24e);
            WM_RestartTimer(pMsg->Data.v, 200);
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialoginfo(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:        
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_BLACK, "");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), &SIF24_Font, GUI_BLACK, "");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), &SIF24_Font, GUI_BLACK, "");        
        break;
    case WM_PAINT:
        if (BUTTON_IsPressed(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_0)))
        {
            if (gunstate[0] == GunchargingState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomecharginginfo,infoAx,infoAy);
            }
            if (gunstate[0] == GunchargedoneState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomechargedoneinfo, infoAx, infoAy);
            }
        }
        if (BUTTON_IsPressed(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1)))
        {
            if (gunstate[1] == GunchargingState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomecharginginfo, infoBx, infoBy);
            }
            if (gunstate[1] == GunchargedoneState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomechargedoneinfo, infoBx, infoBy);
            } 
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
    Hwininfo = GUI_CreateDialogBox(_aDialogCreateinfo, GUI_COUNTOF(_aDialogCreateinfo), _cbDialoginfo, hWin, 0, 0);
    WM_HideWin(Hwininfo);
    return hWin;
}
