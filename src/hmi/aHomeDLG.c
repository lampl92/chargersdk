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
//#define ID_BUTTON_2 (GUI_ID_USER + 0x0A)
#define ID_BUTTON_3 (GUI_ID_USER + 0x0B)
#define ID_WINDOW_1 (GUI_ID_USER + 0x0C)
#define ID_TEXT_3 (GUI_ID_USER + 0x0D)
#define ID_TEXT_4 (GUI_ID_USER + 0x0E)
#define ID_TEXT_5 (GUI_ID_USER + 0x0F)
#define ID_WINDOW_2 (GUI_ID_USER + 0x10)
#define ID_WINDOW_3 (GUI_ID_USER + 0x11)
#define ID_WINDOW_4 (GUI_ID_USER + 0x12)
#define ID_PROGBAR_0 (GUI_ID_USER + 0x13)
#define ID_PROGBAR_1 (GUI_ID_USER + 0x14)
#define  ID_TEXT_6  (GUI_ID_USER + 0x15)

//枪状态，信号状态，价格状态
#define ID_Timergunastateflash           1
#define ID_Timergunbstateflash           2
#define ID_Timersignalstateflash        3
#define ID_Timerpriceflash              4
#define ID_Timertimeflash               5
#define ID_Timerinfoflash               6
#define ID_Timerstateflash              7
#define gunstateax  112 //枪A状态图标x位置
#define gunstateay  79 //枪A状态图标y位置
#define gunstatebx  456 //枪B状态图标x位置
#define gunstateby  79 //枪B状态图标y位置
#define signalx  755 //信号位置x
#define signaly  3 //信号位置y
#define infoAx 119 //A枪充电信息位置x
#define infoAy 125 //A枪充电信息位置y
#define infoBx 462 //B枪充电信息位置x
#define infoBy 125 //B枪充电信息位置y
#define helpinfox 62
#define helpinfoy 70

static WM_HWIN      Hwininfo;
static WM_HWIN      Hwingunastate, Hwingunbstate;//枪A枪B的状态窗口
static WM_HWIN      Hwinhelp;
static WM_HTIMER    _timerstateflash;//后台状态
static WM_HTIMER _timergunastateflash, _timergunbstateflash, _timersignalstateflash, _timerpriceflash, _timertimeflash;
static WM_HTIMER _timerinfoflash;
static int i = 1;//函数临时使用
int SignalIntensity;//信号强度
int PreSignalIntensity;//之前的信号强度

static GUNState_E homegunstate[2];

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "Home", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "datetimetext", ID_TEXT_0, 510, 7, 240, 30, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun1infobutton", ID_BUTTON_0, gunbuttonax, gunbuttonay, 170, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "gun2infobutton", ID_BUTTON_1, gunbuttonbx, gunbuttonby, 170, 50, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "electricFeetext", ID_TEXT_1, 302, 422, 100, 44, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "severFeetext", ID_TEXT_2, 557, 422, 100, 44, 0, 0x0, 0 },
//    { BUTTON_CreateIndirect, "testButton", ID_BUTTON_2, 680, 40, 120, 400, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "help", ID_BUTTON_3, helpbuttonx, helpbuttony, 170, 70, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreateinfo[] = {
    { WINDOW_CreateIndirect, "info-Window", ID_WINDOW_1, infoAx, infoAy, 218, 211, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "chargingfee", ID_TEXT_3, 120, 55, 240, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "chargingcurrent", ID_TEXT_4, 120, 89, 240, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "chargingpower", ID_TEXT_5, 120, 123, 240, 24, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreategunastate[] = {
    { WINDOW_CreateIndirect, "gunbstate-Window", ID_WINDOW_2, gunstateax, gunstateay, 233, 266, 0, 0x0, 0 },
    { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_0, 0, 0, 233, 266, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "kwtext", ID_TEXT_6, 90, 134, 52, 30, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreategunbstate[] = {
    { WINDOW_CreateIndirect, "gunbstate-Window", ID_WINDOW_3, gunstatebx, gunstateby, 233, 266, 0, 0x0, 0 },
   // { PROGBAR_CreateIndirect, "Progbar", ID_PROGBAR_1, gunstatebx, gunstateby, 235, 268, 0, 0x0, 0 },    
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreatehelp[] = {
    { WINDOW_CreateIndirect, "gunbstate-Window", ID_WINDOW_4, 62, 70, 674, 322, 0, 0x0, 0 },
};


static void updategunState(WM_MESSAGE * pMsg)//枪状态刷新函数
{
//    WM_HWIN      hItem;
//    switch (GBSgunstate[0])
//    {
//    case GunfreeState:
//        GUI_MEMDEV_WriteAt(MemdevhomegunAfree, gunstateax, gunstateay);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//        BUTTON_SetSkin(hItem, SKIN_buttongunAscancode);
//        break;
//    case GunchargingState:
//        GUI_MEMDEV_WriteAt(MemdevhomegunAcharging, gunstateax, gunstateay);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//        BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
//        break;
//    case GunchargedoneState:
//        GUI_MEMDEV_WriteAt(MemdevhomegunAchargedone, gunstateax, gunstateay);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//        BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
//        break;
//    case Gunerror:
//        GUI_MEMDEV_WriteAt(MemdevhomegunAerror, gunstateax, gunstateay);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//        BUTTON_SetSkin(hItem, SKIN_buttongunAerror);
//        break;
//    }
//    switch (GBSgunstate[1])
//    {
//    case GunfreeState:
//        GUI_MEMDEV_WriteAt(MemdevhomegunBfree, gunstatebx, gunstateby);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//        BUTTON_SetSkin(hItem, SKIN_buttongunBscancode);
//        break;
//    case GunchargingState:
//        GUI_MEMDEV_WriteAt(MemdevhomegunBcharging, gunstatebx, gunstateby);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//        BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
//        break;
//    case GunchargedoneState:
//        GUI_MEMDEV_WriteAt(MemdevhomegunBchargedone, gunstatebx, gunstateby);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//        BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
//        break;
//    case Gunerror:
//        GUI_MEMDEV_WriteAt(MemdevhomegunBerror, gunstatebx, gunstateby);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//        BUTTON_SetSkin(hItem, SKIN_buttongunBerror);
//        break;
//    }
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

static void updateprice(WM_MESSAGE * pMsg, int idpowerfee, int idservidefee)
{
    uint8_t pos = 0;
    uint8_t ucSegState;
    time_t  now;
    uint8_t strPowerFee[10];
    uint8_t strServiceFee[10];
    now = time(NULL);
    extern OrderSegState_e JudgeSegState(time_t now, echProtocol_t *pProto, uint8_t *ppos);
    ucSegState = (uint8_t)JudgeSegState(now, pechProto, &pos);
    sprintf(strPowerFee, "%5.1f", pechProto->info.dSegEnergyFee[ucSegState]);
    sprintf(strServiceFee, "%5.1f", pechProto->info.dSegServFee[ucSegState]);
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, idpowerfee), strPowerFee);/**< 充电费*/
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, idservidefee), strServiceFee);/**< 服务费 */  
}

static void updatedatetime(WM_MESSAGE *pMsg, uint16_t ID_TEXT)
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

static void updateinfo(WM_MESSAGE *pMsg)//详细信息刷新专用
{
    uint8_t temp_buf[32];
    uint32_t time_charge;
    CON_t *pCON;
    if (BUTTON_IsPressed(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_0)))
    {
        pCON = CONGetHandle(0);
        if (GBSgunstate[0] == GunchargingState)
        {
            sprintf(temp_buf, "%.2f", pCON->order.dTotalEnergy);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), temp_buf);//充入电量
            sprintf(temp_buf, "%.2f", pCON->status.dChargingCurrent);//充电电流   
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), temp_buf);
            sprintf(temp_buf, "%.2f", (pCON->status.dChargingVoltage * pCON->status.dChargingCurrent) / 1000);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), temp_buf);//充电功率     
        }
        if (GBSgunstate[0] == GunchargedoneState)
        {
            sprintf(temp_buf, "%.2f", pCON->order.dTotalEnergy);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), temp_buf);//充入电量
            sprintf(temp_buf, "%.2f", pCON->order.dTotalFee);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), temp_buf);//消费总额 
            time_charge = pCON->order.tStopTime - pCON->order.tStartTime;
            sprintf(temp_buf, "%.2lf", time_charge / 60.0);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), temp_buf);//充电时间
        }
    }
    if (BUTTON_IsPressed(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1)))
    {
        pCON = CONGetHandle(1);
        if (GBSgunstate[1] == GunchargingState)
        {
            sprintf(temp_buf, "%.2f", pCON->order.dTotalEnergy);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), temp_buf);//充入电量
            sprintf(temp_buf, "%.2f", pCON->status.dChargingCurrent);//充电电流   
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), temp_buf);
            sprintf(temp_buf, "%.2f", (pCON->status.dChargingVoltage * pCON->status.dChargingCurrent) / 1000);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), temp_buf);//充电功率   
        }
        if (GBSgunstate[1] == GunchargedoneState)
        {
            sprintf(temp_buf, "%.2f", pCON->order.dTotalEnergy);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), temp_buf);//充入电量
            sprintf(temp_buf, "%.2f", pCON->order.dTotalFee);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), temp_buf);//消费总额 
            time_charge = pCON->order.tStopTime - pCON->order.tStartTime;
            sprintf(temp_buf, "%.2lf", time_charge / 60.0);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), temp_buf);//充电时间
        } 
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) 
{
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        SignalIntensity = getSignalIntensity();
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), GUI_WHITE);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), GUI_WHITE);
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), GUI_WHITE);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &fontwryhcg30e);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &fontwryhcg30e);
        updatedatetime(pMsg, ID_TEXT_0);
        updateprice(pMsg, ID_TEXT_1, ID_TEXT_2);       
        
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
//        BUTTON_SetFocussable(hItem,0);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
//        BUTTON_SetFocussable(hItem,0);
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
//        BUTTON_SetSkin(hItem, SKIN_buttontest);
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
        BUTTON_SetSkin(hItem, SKIN_buttonhelp);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break;              
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevhomeback, 0, 0);
        //updategunState(pMsg);
        updatesignal(pMsg);
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_0: //'gun1infobutton'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                if (GBSgunstate[0] == GunchargingState || GBSgunstate[0] == GunchargedoneState)
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
                if (GBSgunstate[1] == GunchargingState || GBSgunstate[1] == GunchargedoneState)
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
//        case ID_BUTTON_2: //'testButton'
//            switch (NCode) {
//            case WM_NOTIFICATION_CLICKED:
//
//                break;
//            case WM_NOTIFICATION_RELEASED:
//                GUI_EndDialog(pMsg->hWin, 0);
//                CreateselectgunDLG();
//                break;
//            }
//            break;
        case ID_BUTTON_3: //'help'
            switch (NCode) {
            case WM_NOTIFICATION_CLICKED:
                WM_ShowWin(Hwinhelp);              
                break;
            case WM_NOTIFICATION_RELEASED:
                WM_HideWin(Hwinhelp);
                break;
            case WM_NOTIFICATION_MOVED_OUT:
                WM_HideWin(Hwinhelp);
                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timersignalstateflash)
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
            updateprice(pMsg, ID_TEXT_1, ID_TEXT_2);
            WM_RestartTimer(pMsg->Data.v, 1000);
        }
        else if (pMsg->Data.v == _timertimeflash)
        {
            if (!(GBSgunstate[1] == GunchargingState || GBSgunstate[1] == GunchargedoneState \
                || GBSgunstate[0] == GunchargingState || GBSgunstate[0] == GunchargedoneState))
            {
                WM_HideWin(Hwininfo);
            }
            updatedatetime(pMsg, ID_TEXT_0);
            WM_RestartTimer(pMsg->Data.v, 200);
        }
        else if (pMsg->Data.v == _timerstateflash)
        {
            if (gbsstate == StateGetGunInfo)
            {
                WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPSELECTGUN);
            }
            WM_RestartTimer(pMsg->Data.v, 100);
        }
        break;
    case MSG_JUMPSELECTGUN:
        GUI_EndDialog(pMsg->hWin, 0);
        CreateselectgunDLG();
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
            if (GBSgunstate[0] == GunchargingState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomecharginginfo, infoAx, infoAy);
            }
            if (GBSgunstate[0] == GunchargedoneState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomechargedoneinfo, infoAx, infoAy);
            }
        }
        if (BUTTON_IsPressed(WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1)))
        {
            if (GBSgunstate[1] == GunchargingState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomecharginginfo, infoBx, infoBy);
            }
            if (GBSgunstate[1] == GunchargedoneState)
            {
                GUI_MEMDEV_WriteAt(Memdevhomechargedoneinfo, infoBx, infoBy);
            } 
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerinfoflash)
        {
            updateinfo(pMsg);
            WM_RestartTimer(pMsg->Data.v, 200);    
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialoggunastate(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    CON_t *pCON;
    float pkw;//功率
    int pkwpercent;//功率百分比
    uint8_t temp_buf[32];
    
    pCON = CONGetHandle(0);
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG: 
        TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), GUI_WHITE);
        TEXT_SetFont(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &fontwryhcg36e);
        TEXT_SetTextAlign(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), TEXT_CF_HCENTER | TEXT_CF_VCENTER);
        homegunstate[0] = GBSgunstate[0];
        hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0);
        PROGBAR_SetValue(hItem, 0);
        PROGBAR_SetSkin(hItem, SKIN_progbarmeter);
        if (homegunstate[0] != GunchargingState)
        {
            WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6));
            WM_HideWin(hItem); 
        }
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break;    
    case WM_PAINT:
        switch (homegunstate[0])
        {
        case GunfreeState:
            GUI_MEMDEV_WriteAt(MemdevhomegunAfree, gunstateax, gunstateay);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_0);
            BUTTON_SetSkin(hItem, SKIN_buttongunAscancode);
            break;
        case GunchargingState:
            GUI_MEMDEV_WriteAt(MemdevhomegunAcharging, gunstateax, gunstateay);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_0);
            BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
            break;
        case GunchargedoneState:
            GUI_MEMDEV_WriteAt(MemdevhomegunAchargedone, gunstateax, gunstateay);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_0);
            BUTTON_SetSkin(hItem, SKIN_buttongunAlookinfo);
            break;
        case Gunerror:
            GUI_MEMDEV_WriteAt(MemdevhomegunAerror, gunstateax, gunstateay);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_0);
            BUTTON_SetSkin(hItem, SKIN_buttongunAerror);
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timergunastateflash)
        {
            if (homegunstate[0] == GunchargingState)
            {
                pCON = CONGetHandle(0);
                pkw = (pCON->status.dChargingVoltage * pCON->status.dChargingCurrent)/1000;
                sprintf(temp_buf, "%.1f", pkw);
                TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), temp_buf);
                pkwpercent = pkw / 40;
                PROGBAR_SetValue(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0), pkw);               
            }           
            if (homegunstate[0] != GBSgunstate[0]) 
            {           
                homegunstate[0] = GBSgunstate[0];
                if (homegunstate[0] == GunchargingState)
                {
                    WM_ShowWin(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0));
                    WM_ShowWin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6));
                }
                else
                {
                    WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6));
                    WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_0));
                }
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
            }
            WM_RestartTimer(pMsg->Data.v, 100);    
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialoggunbstate(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    CON_t *pCON;
    int pkw;//功率
    pCON = CONGetHandle(0);
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:        
        homegunstate[1] = GBSgunstate[1];
//        hItem = WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1);
//        PROGBAR_SetValue(hItem, 0);
//        PROGBAR_SetSkin(hItem, SKIN_progbarmeter);
//        WM_HideWin(hItem);
        break;
    case MSG_UPDATE:
        WM_InvalidateWindow(pMsg->hWin);
        break;    
    case WM_PAINT:
        switch (homegunstate[1])
        {
        case GunfreeState:
            GUI_MEMDEV_WriteAt(MemdevhomegunBfree, gunstatebx, gunstateby);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1);
            BUTTON_SetSkin(hItem, SKIN_buttongunBscancode);
            break;
        case GunchargingState:
            GUI_MEMDEV_WriteAt(MemdevhomegunBcharging, gunstatebx, gunstateby);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1);
            BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
            break;
        case GunchargedoneState:
            GUI_MEMDEV_WriteAt(MemdevhomegunBchargedone, gunstatebx, gunstateby);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1);
            BUTTON_SetSkin(hItem, SKIN_buttongunBlookinfo);
            break;
        case Gunerror:
            GUI_MEMDEV_WriteAt(MemdevhomegunBerror, gunstatebx, gunstateby);
            hItem = WM_GetDialogItem(WM_GetParent(pMsg->hWin), ID_BUTTON_1);
            BUTTON_SetSkin(hItem, SKIN_buttongunBerror);
            break;
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timergunbstateflash)
        {
//            if (homegunstate[1] == GunchargingState)
//            {
//                pkw = (pCON->status.dChargingVoltage * pCON->status.dChargingCurrent) / 70;
//                PROGBAR_SetValue(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1), pkw);
//                WM_ShowWin(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1));
//            }
//            else
//            {
//                WM_HideWin(WM_GetDialogItem(pMsg->hWin, ID_PROGBAR_1));
//            }
            if (homegunstate[1] != GBSgunstate[1]) 
            {
                //WM_SendMessageNoPara(pMsg->hWin, WM_PAINT);
                WM_SendMessageNoPara(pMsg->hWin, MSG_UPDATE);
                homegunstate[1] = GBSgunstate[1];
            }
            WM_RestartTimer(pMsg->Data.v, 20);    
        }
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

static void _cbDialoghelp(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:        
        break;    
    case WM_PAINT:
        GUI_MEMDEV_WriteAt(Memdevhomehelpinfo, helpinfox, helpinfoy);
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
    _timerstateflash = WM_CreateTimer(hWin, ID_Timerstateflash, 100, 0);
    _timersignalstateflash = WM_CreateTimer(hWin, ID_Timersignalstateflash, 100, 0);
    _timerpriceflash = WM_CreateTimer(hWin, ID_Timerpriceflash, 500, 0);
    _timertimeflash = WM_CreateTimer(hWin, ID_Timertimeflash, 200, 0);
    
    Hwingunastate = GUI_CreateDialogBox(_aDialogCreategunastate, GUI_COUNTOF(_aDialogCreategunastate), _cbDialoggunastate, hWin, 0, 0);
    _timergunastateflash = WM_CreateTimer(Hwingunastate, ID_Timergunastateflash, 100, 0);
    
    Hwingunbstate = GUI_CreateDialogBox(_aDialogCreategunbstate, GUI_COUNTOF(_aDialogCreategunbstate), _cbDialoggunbstate, hWin, 0, 0);
    _timergunbstateflash = WM_CreateTimer(Hwingunbstate, ID_Timergunbstateflash, 100, 0);
    
    Hwininfo = GUI_CreateDialogBox(_aDialogCreateinfo, GUI_COUNTOF(_aDialogCreateinfo), _cbDialoginfo, hWin, 0, 0);
    _timerinfoflash = WM_CreateTimer(Hwininfo, ID_Timerinfoflash, 200, 0);
    WM_HideWin(Hwininfo);
    
    Hwinhelp = GUI_CreateDialogBox(_aDialogCreatehelp, GUI_COUNTOF(_aDialogCreatehelp), _cbDialoghelp, hWin, 0, 0);
    WM_HideWin(Hwinhelp);

    return hWin;
}
