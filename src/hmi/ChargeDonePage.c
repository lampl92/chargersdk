#include "touchtimer.h"
#include "DIALOG.h"
#include "HMI_Start.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_IMAGE_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x03)
#define ID_TEXT_1 (GUI_ID_USER + 0x04)
#define ID_IMAGE_2 (GUI_ID_USER + 0x05)
#define ID_TEXT_2 (GUI_ID_USER + 0x07)
#define ID_TEXT_3 (GUI_ID_USER + 0x08)
#define ID_TEXT_4 (GUI_ID_USER + 0x09)
#define ID_TEXT_5 (GUI_ID_USER + 0x0C)
#define ID_TEXT_6 (GUI_ID_USER + 0x0D)
#define ID_TEXT_7 (GUI_ID_USER + 0x0E)
#define ID_TEXT_8 (GUI_ID_USER + 0x0F)

#define ID_IMAGE_0_IMAGE_0 0x00
#define ID_IMAGE_1_IMAGE_0 0x01
#define ID_IMAGE_2_IMAGE_0 0x02

#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3

WM_HWIN _hWinChargDone;
static WM_HTIMER _timerRTC, _timerData, _timerSignal;
static uint8_t first_flag = 0;
static uint8_t orderFinish = 0;
uint8_t strCSQ[32];
uint8_t EventChargeDoneFlag = 0;//用于事件发生时，防止图片重复刷新，影响响应速度

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "ChargeDonePage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "ChargeDoneImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { TEXT_CreateIndirect, "Date", ID_TEXT_0, 4, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Time", ID_TEXT_1, 136, 4, 120, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "PromptImage", ID_IMAGE_2, 65, 315, 319, 59, 0, 0, 0 },
    { TEXT_CreateIndirect, "Hour", ID_TEXT_2, 156, 168, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "min", ID_TEXT_3, 256, 168, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "sec", ID_TEXT_4, 342, 168, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalPower", ID_TEXT_5, 161, 231, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalServFee", ID_TEXT_6, 582, 161, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalPowerFee", ID_TEXT_7, 582, 221, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalFee", ID_TEXT_8, 582, 281, 282, 24, 0, 0x0, 0 },
};

static void Data_Process(WM_MESSAGE *pMsg)
{
    static uint8_t timer_count = 0;
    static uint8_t num = 0;
    uint8_t Timer_buf[10];
    uint8_t temp_buf[32];
    CON_t *pCON;
    time_t now;
    static time_t first;
    volatile uint32_t diffsec;
    volatile int8_t sec;
    EventBits_t uxBits;


    WM_HWIN hWin = pMsg->hWin;

    pCON = CONGetHandle(0);
    uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
        defEventBitOrderFinishToHMI,
        pdTRUE,
        pdTRUE,
        0);
    //if (((uxBits & defEventBitOrderFinishToHMI) == defEventBitOrderFinishToHMI) && !bittest(EventChargeDoneFlag,0))//订单上传完成
    if (((uxBits & defEventBitOrderFinishToHMI) == defEventBitOrderFinishToHMI) )//订单上传完成

    {        
        orderFinish = 1;
        bitset(EventChargeDoneFlag, 0);
    }

 //   if ((orderFinish == 1) && pCON->status.xPlugState == UNPLUG)
    if (orderFinish == 1)
    {
        if (first_flag == 0)
        {
            first_flag = 1;
            first = time(NULL);
        }

        now = time(NULL);

        diffsec = (uint32_t)difftime(now, first);

        sec = 3 - diffsec;
        if (sec < 0)
            sec = 0;
        sprintf((char *)_secDown, "(%02dS)", sec);
        if (sec == 0)
        {
            orderFinish = 0;
            first_flag = 0;
            			//跳到HOME
            WM_SendMessageNoPara(hWin, MSG_JUMPHOME);
        }      
    }
}


static void _cbDialog(WM_MESSAGE * pMsg) {
    const void *pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;
    CON_t *pCON;
    uint32_t time_charge;
    uint8_t temp_buf[32];
    volatile int8_t hour;
    volatile int8_t min;
    volatile int8_t sec;
    EventBits_t uxBits;
    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_WHITE, " ");
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBMP(hItem, ChargeDoneImage->pfilestring, ChargeDoneImage->pfilesize);
        
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
        IMAGE_SetBMP(hItem, OrderUploadImage->pfilestring, OrderUploadImage->pfilesize);
        
        pCON = CONGetHandle(0);
        memset(temp_buf, '\0', sizeof(temp_buf));
        sprintf(temp_buf, "%.2f", pCON->order.dTotalPower);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), &SIF24_Font, GUI_WHITE, temp_buf);//充入电量
        sprintf(temp_buf, "%.2f", pCON->order.dTotalServFee);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &SIF24_Font, GUI_WHITE, temp_buf);//服务费
        sprintf(temp_buf, "%.2f", pCON->order.dTotalPowerFee);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), &SIF24_Font, GUI_WHITE, temp_buf);//电费
        sprintf(temp_buf, "%.2f", pCON->order.dTotalFee);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8), &SIF24_Font, GUI_WHITE, temp_buf);//消费总额  
        pCON = CONGetHandle(0);
        uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
            defEventBitOrderMakeFinish,
            pdFALSE,
            pdTRUE,
            65000);
        if ((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)
        {
            time_charge = pCON->order.tStopTime - pCON->order.tStartTime;
        //        time_charge = 0;
            hour = time_charge / 3600;
            min = time_charge % 3600 / 60;
            sec = time_charge % 3600 % 60;

            memset(temp_buf, '\0', strlen(temp_buf));
            sprintf(temp_buf, "% 2d", hour);
            Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_WHITE, temp_buf);//hour
            memset(temp_buf, '\0', strlen(temp_buf));
            sprintf(temp_buf, "% 2d", min);
            Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_WHITE, temp_buf);// min
            memset(temp_buf, '\0', strlen(temp_buf));
            sprintf(temp_buf, "% 2d", sec);
            Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), &SIF24_Font, GUI_WHITE, temp_buf);// sec
            
            xEventGroupSetBits(xHandleEventHMI, defeventBitHMI_ChargeReqDispDoneOK);
            xEventGroupSetBits(pCON->status.xHandleEventOrder, defEventBitOrder_HMIDispOK);
        }
            break;
        case WM_PAINT:

            break;
        case WM_TIMER:
            if (pMsg->Data.v == _timerRTC)
            {
                       // TODO (zshare#1#): 下面的if不起作用.\但是if里嵌套的if起作用,目前先用此来规避不起作用的if
                if ((bittest(winInitDone, 0))&&(_hWinChargDone == cur_win))
                {
                    /**< 数据处理 */
                    Data_Process(pMsg);
                    /**< 信号数据处理 */
                    Signal_Show();
                    /**< 灯光控制 */
                    Led_Show();
                    /**< 如果界面发生了切换 */
                    if (_hWinChargDone == cur_win)
                    {
                        /**< 故障分析 */
                        Err_Analy(pMsg->hWin);
                        /**< 特殊触控点分析 */
                        CaliDone_Analy(pMsg->hWin);
                    }
                }
                /**< 显示时间和日期 */
                Caculate_RTC_Show(pMsg, ID_TEXT_0, ID_TEXT_1);
                
                if ((SignalFlag == 12) || (SignalFlag > 12))
                {
                    SignalIntensity = getSignalIntensity();
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
                SignalFlag++;
                if (orderFinish == 1)
                {
                    pCON = CONGetHandle(0);
                    if (pCON->order.ucStopType == defOrderStopType_Full)
                    {
                        IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2), FullDoneImage->pfilestring, FullDoneImage->pfilesize);
                    }
                    else if ((pCON->order.ucStopType == defOrderStopType_Scram)\
                        || (pCON->order.ucStopType == defOrderStopType_OverCurr)\
                        || (pCON->order.ucStopType == defOrderStopType_Poweroff)\
                        || (pCON->order.ucStopType == defOrderStopType_NetLost)\
                        || (pCON->order.ucStopType == defOrderStopType_Scram))
                    {
                        IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2), DevErrDoneImage->pfilestring, DevErrDoneImage->pfilesize);
                    }
                    else
                    {
                        IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2), NormalDoneImage->pfilestring, NormalDoneImage->pfilesize);
                    }
                }
                
/**< 重启定时器 */
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
                GUI_EndDialog(err_hItem, 0);
                err_hItem = 0;
            }
            break;
        case MSG_JUMPHOME:
            if (AdvertisementRecordFlag == 1)
            {
                WM_HideWindow(_hWinAdvertizement);
                WM_ShowWindow(cur_win);
                AdvertisementRecordFlag = 0;
                GUI_Exec();
            }
            first_flag = 0;
            EventChargeDoneFlag = 0;
            _deleteWin(_hWinChargDone);
            _hWinChargDone = 0;
            CreateHomePage();
            current_page = _HOMEPAGE;
            bitset(winInitDone, 7);
            break;
        default:
            WM_DefaultProc(pMsg);
            break;
        }   
}
/*
*
*       CreateChargeDonePage
*/
WM_HWIN CreateChargeDonePage(void);
WM_HWIN CreateChargeDonePage(void) 
{
    _hWinChargDone = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    cur_win = _hWinChargDone;
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinChargDone), ID_TimerTime, 300, 0);
    bitset(winInitDone, 0);
    return 0;
}



