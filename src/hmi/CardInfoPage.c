#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "display.h"
#include "utils.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_IMAGE_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x05)
#define ID_TEXT_1 (GUI_ID_USER + 0x06)
#define ID_TEXT_2 (GUI_ID_USER + 0x07)
#define ID_TEXT_3 (GUI_ID_USER + 0x08)
#define ID_IMAGE_2 (GUI_ID_USER + 0x09)

#define ID_IMAGE_0_IMAGE_0 0x00
#define ID_IMAGE_1_IMAGE_0 0x01
#define ID_IMAGE_2_IMAGE_0 0x02

#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3











//刷卡已经不再用这些事件, 为保证编译临时定义, 整改界面流程后删掉

#define defEventBitGoodIDReqDisp        BIT_6
#define defEventBitGoodIDReqDispOK      BIT_7
#define defEventBitBadIDReqDisp        BIT_8
#define defEventBitBadIDReqDispOK      BIT_9
#define defEventBitOweIDReqDisp        BIT_10
#define defEventBitOwdIDReqDispOK      BIT_11

#define defEventBitHMITimeOutToRFID         BIT_0














WM_HWIN _hWinCardInfo;
uint8_t EventFlag = 0;//用于事件发生时，防止图片重复刷新，影响响应速度
uint8_t Timer_buf[32];

static WM_HTIMER _timerRTC, _timerData, _timerSignal;
static uint8_t first_CardInfo = 0;
static time_t first_time = 0;
static int InfoOkFlag = 0;

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "CardInfoPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "CardInfoImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { TEXT_CreateIndirect, "Date", ID_TEXT_0, 4, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Time", ID_TEXT_1, 136, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "CardNumber", ID_TEXT_2, 326, 165, 280, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Balance", ID_TEXT_3, 326, 223, 280, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "PromptImage", ID_IMAGE_2, 240, 316, 319, 59, 0, 0, 0 },
};
static void Data_Flush(WM_MESSAGE *pMsg)
{
    EventBits_t uxBitRFID;
    EventBits_t uxBitCharge;
    CON_t *pCON;
    pCON = CONGetHandle(0);//选择枪的时候获取pCON
    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
        defEventBitGoodIDReqDisp,
        pdTRUE,
        pdTRUE,
        0);
    if (((uxBitRFID & defEventBitGoodIDReqDisp) == defEventBitGoodIDReqDisp) && (!bittest(EventFlag,0)))
    {
        /**< 显示卡余额 */
        sprintf(Timer_buf, "%.2lf", pRFIDDev->order.dBalance);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), Timer_buf);
        if (pCON->status.xPlugState == UNPLUG)
        {
            IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2), PleaseConnectPlugImage->pfilestring, PleaseConnectPlugImage->pfilesize);
        }
        else if (pCON->status.xPlugState == PLUG)
        {    
            IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2), CardInfoVoidImage->pfilestring, CardInfoVoidImage->pfilesize);
        }
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGoodIDReqDispOK);
        bitset(winInitDone, 2);
        bitset(EventFlag, 0);
    }
    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
        defEventBitBadIDReqDisp,
        pdTRUE,
        pdTRUE,
        0);
    if (((uxBitRFID & defEventBitBadIDReqDisp) == defEventBitBadIDReqDisp) && (!bittest(EventFlag, 1)))
    {
        IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2), CardUnregisteredImage->pfilestring, CardUnregisteredImage->pfilesize);
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitBadIDReqDispOK);
        bitset(EventFlag, 1);
    }
    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
        defEventBitOweIDReqDisp,
        pdTRUE,
        pdTRUE,
        0);
    if (((uxBitRFID & defEventBitOweIDReqDisp) == defEventBitOweIDReqDisp) && (!bittest(EventFlag, 2)))
    {
        sprintf(Timer_buf, "%.2lf", pRFIDDev->order.dBalance);
        IMAGE_SetBMP(WM_GetDialogItem(pMsg->MsgId, ID_IMAGE_2), CardArrearsImage->pfilestring, CardArrearsImage->pfilesize);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_RED, Timer_buf);
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitOwdIDReqDispOK);
        bitset(EventFlag, 2);
    }   
}
static void Data_Process(WM_MESSAGE *pMsg)
{
    EventBits_t uxBit;
    CON_t *pCON;
    time_t now;

    volatile uint32_t diffsec;

    WM_HWIN hWin = pMsg->hWin;

    pCON = CONGetHandle(0);
    uxBit = xEventGroupGetBits(pCON->status.xHandleEventOrder);
    if ((uxBit & defEventBitOrderMakeOK) == defEventBitOrderMakeOK)
    {
        /** 跳转充电界面 */
        IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2),CardInfoVoidImage->pfilestring, CardInfoVoidImage->pfilesize);        
        //bitclr(winInitDone, 0);
        first_CardInfo = 0;
        bitset(winInitDone, 3);
        /**< 跳到充电中 */
    }
    else
    {
        ;//Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "充电条件不满足.");/** @todo (zshare#1#): 充电条件未达成 */
    }

    now = time(NULL);
    if (first_CardInfo == 0)
    {
        first_CardInfo = 1;
        first_time = now;
    }
    diffsec = (uint32_t)difftime(now, first_time);
	//printf_safe("now = %d, first_time = %d, sec = %d ,winInitDone = %d ,first_CardInfo = %d\n", now, first_time, sec, winInitDone, first_CardInfo);
    if (diffsec > 60)
    {
        bitclr(winInitDone, 0);
        first_CardInfo = 0;
        xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件
        //跳到HOME
        WM_SendMessageNoPara(hWin, MSG_JUMPHOME);
    }   
}
static void _cbDialog(WM_MESSAGE * pMsg) {
    const void * pData;
    WM_HWIN      hItem;
    U32          FileSize;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
      //
      // Initialization of 'CardInfoImage'
      //
        first_CardInfo = 0;
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_WHITE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLACK, pRFIDDev->order.strCardID);//卡号
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_BLACK, "?");        
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);   
        IMAGE_SetBMP(hItem, CardInfoImage->pfilestring, CardInfoImage->pfilesize);
        //
        // Initialization of 'SignalImage'
        //
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
        //
        // Initialization of 'PromptImage'
        //
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_2);
        IMAGE_SetBMP(hItem, GetCardInfoImage->pfilestring, GetCardInfoImage->pfilesize);
        break;
    case WM_PAINT://MSG_UPDATEDATA:

        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerRTC)
        {
            /**< 显示时间和日期 */
            Caculate_RTC_Show(pMsg, ID_TEXT_0, ID_TEXT_1);
            if ((SignalFlag == 4) || (SignalFlag > 4))
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
            /**< 重启定时器 */
            WM_RestartTimer(pMsg->Data.v, 300);
        }
        if (pMsg->Data.v == _timerSignal)
        {        /// TODO (zshare#1#): 下面的if不起作用.但是if里嵌套的if起作用,目前先用此来规避不起作用的if
            if ((bittest(winInitDone, 0))&&(_hWinCardInfo == cur_win))
            //if(_hWinCardInfo == cur_win)
            {
                /**< 信号数据处理 */
                Signal_Show();
                /**< 灯光控制 */
                Led_Show();
                /**< 如果界面发生了切换 */
                if ((bittest(winInitDone, 0))&&(_hWinCardInfo == cur_win))
                //if(_hWinCardInfo == cur_win)
                {
                    /**< 故障分析 */
                    Err_Analy(pMsg->hWin);
                    /**< 特殊触控点分析 */
                    CaliDone_Analy(pMsg->hWin);
                    /**< 数据处理 */
                    //Data_Flush(pMsg);
                    Data_Process(pMsg);

                }
                //            CaliDone_Analy(pMsg->hWin);
            }
            if (bittest(winInitDone, 2) && bittest(winInitDone, 3))
            {
                
                if (InfoOkFlag == 1)
                {
                    bitclr(winInitDone, 2);
                    bitclr(winInitDone, 3);
                    WM_SendMessageNoPara(pMsg->hWin, MSG_JUMPCHAING);
                    InfoOkFlag = 0;
                }
                else
                {
                    InfoOkFlag = 1;                                  
                    WM_RestartTimer(pMsg->Data.v, 500);                   
                }              
            }
            else
            {
                WM_RestartTimer(pMsg->Data.v, 100);
            }
        }
        if (pMsg->Data.v == _timerData)
        {
            Data_Flush(pMsg);
            WM_RestartTimer(pMsg->Data.v, 300);
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
    case MSG_JUMPHOME:
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
            GUI_Exec();
        }
//        current_page = _HOMEPAGE;
        bitset(winInitDone, 7);
        EventFlag = 0;
        _deleteWin(_hWinCardInfo);
        _hWinCardInfo = 0;
        CreateHomePage();
        break;
    case MSG_JUMPCHAING:
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
            GUI_Exec();
        }
        current_page = _CHARGINGPAGE;
        bitset(winInitDone, 7);
        EventFlag = 0;
        _deleteWin(_hWinCardInfo);
        _hWinCardInfo = 0;
        CreateChargingPage();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/*
*  CreateCardInfoPage
*       
*/
WM_HWIN CreateCardInfoPage(void);
WM_HWIN CreateCardInfoPage(void) {
    _hWinCardInfo = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    cur_win = _hWinCardInfo;
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinCardInfo), ID_TimerTime, 20, 0);
    _timerData = WM_CreateTimer(WM_GetClientWindow(_hWinCardInfo), ID_TimerFlush, 10, 0);
    _timerSignal = WM_CreateTimer(WM_GetClientWindow(_hWinCardInfo), ID_TimerSignal, 300, 0);
    bitset(winInitDone, 0);
    return 0;
}