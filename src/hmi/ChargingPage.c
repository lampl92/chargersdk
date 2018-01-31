#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_IMAGE_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x05)
#define ID_TEXT_1 (GUI_ID_USER + 0x06)
#define ID_IMAGE_2 (GUI_ID_USER + 0x07)
#define ID_IMAGE_3 (GUI_ID_USER + 0x08)
#define ID_TEXT_2 (GUI_ID_USER + 0x09)
#define ID_TEXT_3 (GUI_ID_USER + 0x0A)
#define ID_TEXT_4 (GUI_ID_USER + 0x0B)
#define ID_TEXT_5 (GUI_ID_USER + 0x0C)
#define ID_TEXT_6 (GUI_ID_USER + 0x0D)
#define ID_TEXT_7 (GUI_ID_USER + 0x0E)
#define ID_TEXT_8 (GUI_ID_USER + 0x0F)
#define ID_TEXT_9 (GUI_ID_USER + 0x10)
#define ID_TEXT_10 (GUI_ID_USER + 0x11)

#define ID_IMAGE_0_IMAGE_0 0x00
#define ID_IMAGE_1_IMAGE_0 0x01
#define ID_IMAGE_2_IMAGE_0 0x02
#define ID_IMAGE_3_IMAGE_0 0x03

#define ID_TimerTime     1
#define ID_TimerCortoon  2
#define ID_TimerText     3

WM_HWIN _hWinCharging;
uint8_t _secDown[10];
static WM_HTIMER _timerRTC, _timerCortoon,_timerText;
int cartoonflag = 0;//动画接替显示标志

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "ChargingPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "ChargingImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { TEXT_CreateIndirect, "Date", ID_TEXT_0, 4, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Time", ID_TEXT_1, 136, 4, 120, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "CartoonImage", ID_IMAGE_2, 105, 116, 228, 112, 0, 0, 0 },
    { IMAGE_CreateIndirect, "PromptImage", ID_IMAGE_3, 65, 315, 319, 59, 0, 0, 0 },
    { TEXT_CreateIndirect, "Hour", ID_TEXT_2, 168, 262, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "min", ID_TEXT_3, 254, 262, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "sec", ID_TEXT_4, 330, 262, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "ChargingCurrent", ID_TEXT_5, 582, 107, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "ChargingPower", ID_TEXT_6, 582, 158, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalPower", ID_TEXT_7, 582, 213, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalServFee", ID_TEXT_8, 582, 267, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalPowerFee", ID_TEXT_9, 582, 321, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalFee", ID_TEXT_10, 582, 374, 48, 24, 0, 0x0, 0 },
};

static void Data_Flush(WM_MESSAGE *pMsg)
{
    uint8_t temp_buf[32];
    CON_t *pCON;
    time_t now;
    uint32_t diffsec;
    volatile uint8_t sec;
    uint8_t min;
    uint8_t hour;

    WM_HWIN hWin = pMsg->hWin;

    pCON = CONGetHandle(0);///** @todo (zshare#1#): 双枪ID选择 */
    now = time(NULL);

    diffsec = (uint32_t)difftime(now, pCON->order.tStartTime);

    hour = diffsec / 3600;
    min = diffsec % 3600 / 60;
    sec = diffsec % 3600 % 60;

    memset(temp_buf, '\0', sizeof(temp_buf));
    sprintf(temp_buf, "%2d", hour);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_2), temp_buf);//hour
    sprintf(temp_buf, "% 2d", min);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_3), temp_buf);// min
    sprintf(temp_buf, "% 2d", sec);
    Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), &SIF24_Font, GUI_BLUE, temp_buf);
    
//    pCON->status.GetChargingCurrent(pCON);
    sprintf(temp_buf, "%.2f", pCON->status.dChargingCurrent);//充电电流   
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_5), temp_buf);
//    pCON->status.GetChargingPower(pCON);
    sprintf(temp_buf, "%.2f", (pCON->status.dChargingVoltage * pCON->status.dChargingCurrent) / 1000);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), temp_buf);//充电功率
    sprintf(temp_buf, "%.2f", pCON->order.dTotalPower);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), temp_buf);//充入电量
    sprintf(temp_buf, "%.2f", pCON->order.dTotalServFee);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), temp_buf);//当前服务费
    sprintf(temp_buf, "%.2f", pCON->order.dTotalPowerFee);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), temp_buf);//当前电费
    sprintf(temp_buf, "%.2f", pCON->order.dTotalFee);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_10), temp_buf);//消费总额
}

static void Data_Process(WM_MESSAGE *pMsg)
{
    CON_t *pCON;
    EventBits_t uxBitHMI;

    pCON = CONGetHandle(0);

    uxBitHMI = xEventGroupWaitBits(xHandleEventHMI,
        defEventBitHMI_ChargeReqDispDone,
        pdTRUE,
        pdTRUE,
        0);
    if ((uxBitHMI & defEventBitHMI_ChargeReqDispDone) == defEventBitHMI_ChargeReqDispDone)
    {
        /**< 跳到充电完成 */
        WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPCHARGEDONE);
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    CON_t *pCON;
    pCON = CONGetHandle(0);

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBMP(hItem, ChargingImage->pfilestring, ChargingImage->pfilesize);
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
        SignalIntensity = getSignalIntensity();
        if (SignalIntensity == 1)
        {
            IMAGE_SetBMP(hItem, SignalImage1->pfilestring, SignalImage1->pfilesize);
        }
        else if (SignalIntensity == 2)
        {
            IMAGE_SetBMP(hItem, SignalImage2->pfilestring, SignalImage2->pfilesize);
        }
        else if (SignalIntensity == 3)
        {
            IMAGE_SetBMP(hItem, SignalImage3->pfilestring, SignalImage3->pfilesize);
        }
        else if (SignalIntensity == 4)
        {
            IMAGE_SetBMP(hItem, SignalImage4->pfilestring, SignalImage4->pfilesize);
        }
        else if (SignalIntensity == 5)
        {
            IMAGE_SetBMP(hItem, SignalImage5->pfilestring, SignalImage5->pfilesize);
        }
        else
        {
            IMAGE_SetBMP(hItem, SignalImage0->pfilestring, SignalImage0->pfilesize);
        }
        PreSignalIntensity = SignalIntensity;

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
        IMAGE_SetBMP(hItem, cartoonImage0->pfilestring, cartoonImage0->pfilesize);

        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_3);
        if (pCON->order.ucStartType == 4)
        {
            IMAGE_SetBMP(hItem, StopByCardImage->pfilestring, StopByCardImage->pfilesize);
        }
        else if (pCON->order.ucStartType == 5)
        {
            IMAGE_SetBMP(hItem, StopByQRImage->pfilestring, StopByQRImage->pfilesize);
        }
        else
        {
            IMAGE_SetBMP(hItem, ChargingVoidImage->pfilestring, ChargingVoidImage->pfilesize);
        }      
      
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), &SIF24_Font, GUI_WHITE," ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_9), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_10), &SIF24_Font, GUI_WHITE, " ");
        break;
    case WM_PAINT://MSG_UPDATEDATA:

        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerRTC)
        {
            /**< 显示时间和日期 */
            Caculate_RTC_Show(pMsg, ID_TEXT_0, ID_TEXT_1);
            SignalIntensity = getSignalIntensity();              
            if ((SignalFlag == 4) || (SignalFlag > 4))
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);          
                if (SignalIntensity != PreSignalIntensity)
                {
                    if (SignalIntensity == 1)
                    {
                        IMAGE_SetBMP(hItem, SignalImage1->pfilestring, SignalImage1->pfilesize);
                    }
                    else if (SignalIntensity == 2)
                    {
                        IMAGE_SetBMP(hItem, SignalImage2->pfilestring, SignalImage2->pfilesize);
                    }
                    else if (SignalIntensity == 3)
                    {
                        IMAGE_SetBMP(hItem, SignalImage3->pfilestring, SignalImage3->pfilesize);
                    }
                    else if (SignalIntensity == 4)
                    {
                        IMAGE_SetBMP(hItem, SignalImage4->pfilestring, SignalImage4->pfilesize);
                    }
                    else if (SignalIntensity == 5)
                    {
                        IMAGE_SetBMP(hItem, SignalImage5->pfilestring, SignalImage5->pfilesize);
                    }
                    else
                    {
                        IMAGE_SetBMP(hItem, SignalImage0->pfilestring, SignalImage0->pfilesize);
                    }
                }                         
                PreSignalIntensity = SignalIntensity;
                SignalFlag = 0;
            }
            /**< 重启定时器 */
            SignalFlag++;
            WM_RestartTimer(pMsg->Data.v, 300);
        }
        if (pMsg->Data.v == _timerCortoon)
        {
            hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
            if ((cartoonflag == 6) || (cartoonflag > 5))
            {
                cartoonflag = 0;
            }
            if (cartoonflag == 0)
            {
                IMAGE_SetBMP(hItem, cartoonImage0->pfilestring, cartoonImage0->pfilesize);
            }
            else if (cartoonflag == 1)
            {
                IMAGE_SetBMP(hItem, cartoonImage1->pfilestring, cartoonImage1->pfilesize);
            }
            else if (cartoonflag == 2)
            {
                IMAGE_SetBMP(hItem, cartoonImage2->pfilestring, cartoonImage2->pfilesize);
            }
            else if (cartoonflag == 3)
            {
                IMAGE_SetBMP(hItem, cartoonImage3->pfilestring, cartoonImage3->pfilesize);
            }
            else if(cartoonflag == 4)
            {
                IMAGE_SetBMP(hItem, cartoonImage4->pfilestring, cartoonImage4->pfilesize);
            }
            else if(cartoonflag == 5)
            {
                IMAGE_SetBMP(hItem, cartoonImage5->pfilestring, cartoonImage5->pfilesize);
            }
            cartoonflag++;    
            WM_RestartTimer(pMsg->Data.v, 500);
        }
        if (pMsg->Data.v == _timerText)
        {
                /// TODO (zshare#1#): 下面的if不起作用.\但是if里嵌套的if起作用,目前先用此来规避不起作用的if
            if ((bittest(winInitDone, 0))&&(_hWinCharging == cur_win))
            {
                /**< 数据处理 */
                Data_Process(pMsg);
                /**< 信号数据处理 */
                Signal_Show();
                /**< 灯光控制 */
                Led_Show();
                /**< 如果界面发生了切换 */
                if (_hWinCharging == cur_win)
                {
                    /**< 故障分析 */
                    Err_Analy(pMsg->hWin);
                    /**< 特殊触控点分析 */
                    CaliDone_Analy(pMsg->hWin);
                }
                 Data_Flush(pMsg);
     //            Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);
            }
            WM_RestartTimer(pMsg->Data.v, 50);
        }
        break;
    case MSG_CREATERRWIN:
        /**< 故障界面不存在则创建,存在则刷新告警 */
        err_window(pMsg->hWin);
        break;
    case MSG_DELERRWIN:
        /**< 故障界面存在则删除故障界面 */
        if (bittest(winCreateFlag, 0))
        {
            bitclr(winCreateFlag, 0);
            //WM_DeleteWindow(err_hItem);
            GUI_EndDialog(err_hItem, 0);
            err_hItem = 0;
        }
        break;
    case MSG_JUMPCHARGEDONE:
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
            GUI_Exec();
        }
        bitclr(winInitDone, 0);
        _deleteWin(_hWinCharging);
        _hWinCharging = 0;
        CreateChargeDonePage();
        current_page = _CHARGEDONEPAGE;
        bitset(winInitDone, 7);
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}
/*
*
*       CreateChargingPage
*/
WM_HWIN CreateChargingPage(void);
WM_HWIN CreateChargingPage(void) {
    _hWinCharging = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    cur_win = _hWinCharging;
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinCharging), ID_TimerTime, 300, 0);
    _timerCortoon = WM_CreateTimer(WM_GetClientWindow(_hWinCharging), ID_TimerCortoon, 1000, 0);
    _timerText = WM_CreateTimer(WM_GetClientWindow(_hWinCharging), ID_TimerText, 50, 0);
    bitset(winInitDone, 0);
    return 0;
}
