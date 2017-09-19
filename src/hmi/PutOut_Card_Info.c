﻿/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.30                          *
*        Compiled Jul  1 2015, 10:50:32                              *
*        (c) 2015 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "xbffontcreate.h"
#include "touchtimer.h"
#include "interface.h"
#include "HMI_Start.h"
#include "utils.h"
// USER END

#include "DIALOG.h"
WM_HWIN _hWinCardInfo;
uint8_t _secDown[10];
static WM_HTIMER _timerRTC,_timerData,_timerSignal;
static uint8_t first_CardInfo = 0;
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
#define ID_IMAGE_0      (GUI_ID_USER + 0x09)
// USER START (Optionally insert additional defines)
#define ID_BUTTON_1   (GUI_ID_USER + 0x08)
#define ID_TEXT_0     (GUI_ID_USER + 0x0A)
#define ID_TEXT_1     (GUI_ID_USER + 0x0B)
#define ID_TEXT_2     (GUI_ID_USER + 0x0C)
#define ID_TEXT_3     (GUI_ID_USER + 0x0D)
#define ID_TEXT_4     (GUI_ID_USER + 0x0E)
#define ID_TEXT_5     (GUI_ID_USER + 0x05)
#define ID_TEXT_6     (GUI_ID_USER + 0x02)
#define ID_TEXT_7     (GUI_ID_USER + 0x03)
#define ID_TEXT_8     (GUI_ID_USER + 0x04)
#define ID_EDIT_0     (GUI_ID_USER + 0x06)
#define ID_EDIT_1     (GUI_ID_USER + 0x07)
#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3
static time_t first_time = 0;

// USER END
/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCardInfo
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCardInfo[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
    // USER START (Optionally insert additional widgets)
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 789, 459, 0, 0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 630, 0, 80, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_2, 720, 0, 70, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 440, 0, 180, 16, 0, 0x0, 0 },//网络信号强度
    { TEXT_CreateIndirect, "Text", ID_TEXT_4, 225, 367, 300, 20, 0, 0x0, 0 },//最底端的说明
    { BUTTON_CreateIndirect, "退出", ID_BUTTON_0, 554, 317, 100, 36, 0, 0x0, 0 },//倒计时退出
    { BUTTON_CreateIndirect, "确定", ID_BUTTON_1, 554, 272, 100, 36, 0, 0x0, 0 },//枪锁完全锁止后的确认按钮
    { TEXT_CreateIndirect, "Text", ID_TEXT_5, 600, 324, 61, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_6, 223, 228, 500, 61, 0, 0x0, 0 },//是否提示余额不足
    { TEXT_CreateIndirect, "Text", ID_TEXT_0, 222, 149, 80, 30, 0, 0x0, 0 }, //卡号
    { TEXT_CreateIndirect, "Text", ID_TEXT_7, 222, 184, 80, 30, 0, 0x0, 0 }, //余额
    { EDIT_CreateIndirect, "Edit", ID_EDIT_0, 313, 149, 255, 30, 0, 0x64, 0 },//卡号号码
    { EDIT_CreateIndirect, "Edit", ID_EDIT_1, 313, 184, 100, 27, 0, 0x64, 0 },//余额钱
    { TEXT_CreateIndirect, "Text", ID_TEXT_8, 423, 184, 80, 30, 0, 0x0, 0 },//余额单位
    // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
static void Data_Flush(WM_MESSAGE *pMsg)
{
    uint8_t Timer_buf[10];
    EventBits_t uxBitRFID;
    EventBits_t uxBitCharge;
    CON_t *pCON;

    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                defEventBitGoodIDReqDisp,
                                pdTRUE, pdTRUE, 0);
    if((uxBitRFID & defEventBitGoodIDReqDisp) == defEventBitGoodIDReqDisp)
    {
        pCON = CONGetHandle(0);//选择枪的时候获取pCON
        /**< 显示卡余额 */
        sprintf(Timer_buf, "%.2lf", pRFIDDev->order.dBalance);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, " ");
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1), &XBF24_Font, Timer_buf);
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGoodIDReqDispOK);
	    bitset(winInitDone,2);
    }


    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                    defEventBitBadIDReqDisp,
                                    pdTRUE, pdTRUE, 0);
    if((uxBitRFID & defEventBitBadIDReqDisp) == defEventBitBadIDReqDisp)
    {
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "此卡未注册,请查看注册流程.");
        /** @todo (zshare#1#): 未注册卡处理流程 ，处理完成之后，发送DispOK事件*/
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitBadIDReqDispOK);
    }
    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                    defEventBitOweIDReqDisp,
                                    pdTRUE, pdTRUE, 0);
    if((uxBitRFID & defEventBitOweIDReqDisp) == defEventBitOweIDReqDisp)
    {
        sprintf(Timer_buf, "%.2lf", pRFIDDev->order.dBalance);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "此卡已欠费");
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1), &XBF24_Font, Timer_buf);

        /** @todo (zshare#1#): 定10s退出 */
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitOwdIDReqDispOK);
    }
    pCON = CONGetHandle(0);//选择枪的时候获取pCON
    /**未进GoodID ,BadID和OweID状态时显示内容*/
    if(pCON->status.xPlugState == UNPLUG)
    {
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "请连接充电插头");
    }
    /**end of 未进GoodID ,BadID和OweID状态时显示内容*/

}

static void Data_Process(WM_MESSAGE *pMsg)
{
    EventBits_t uxBit;
    CON_t *pCON;
    time_t now;
    
    volatile uint32_t diffsec;
    volatile uint8_t sec;
    volatile uint8_t min;
    volatile uint8_t hour;

    WM_HWIN hWin = pMsg->hWin;

    pCON = CONGetHandle(0);
    uxBit = xEventGroupGetBits(pCON->status.xHandleEventOrder);
	if ((uxBit & defEventBitOrderMakeOK) == defEventBitOrderMakeOK)
    {
        /** 跳转充电界面 */
	    bitclr(winInitDone, 0);
	    first_CardInfo = 0;
	    bitset(winInitDone, 3);
        /**< 跳到充电中 */
    }
    else
    {
        ;//Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "充电条件不满足.");/** @todo (zshare#1#): 充电条件未达成 */
    }

    now = time(NULL);
    if(first_CardInfo == 0)
    {
        first_CardInfo = 1;
        first_time = now;
    }
    diffsec = (uint32_t)difftime(now, first_time);
    if(diffsec > 86400)
    {
        diffsec = 86400;
    }
    hour = diffsec / 3600;
    min = diffsec % 3600 / 60;
    sec = diffsec % 3600 % 60;

    xsprintf((char *)_secDown, "(%02dS)", (60 - sec));
	printf_safe("now = %d, first_time = %d, sec = %d ,winInitDone = %d ,first_CardInfo = %d\n", now, first_time, sec, winInitDone, first_CardInfo);
    if(sec == 59)
    {
	    bitclr(winInitDone,0);
        first_CardInfo = 0;
        xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件
        //跳到HOME
        WM_SendMessageNoPara(hWin,MSG_JUMPHOME);
    }
	if (bittest(winInitDone, 2)&&bittest(winInitDone, 3))
	{
		bitclr(winInitDone, 2);
		bitclr(winInitDone, 3);
		WM_SendMessageNoPara(hWin, MSG_JUMPCHAING);		
	}
}
// USER END
/*********************************************************************
*
*       _cbDialog
*/
static void _cbCardDialog(WM_MESSAGE *pMsg)
{
    const void *pData;
    WM_HWIN      hItem;
    U32          FileSize;
    int          NCode;
    int          Id;
    // USER START (Optionally insert additional variables)
    uint8_t temp_buf[32];
    int i;
    // USER END

    switch (pMsg->MsgId)
    {
    case WM_PAINT://MSG_UPDATEDATA:
        /// TODO (zshare#1#): 下面的if不起作用.\
        但是if里嵌套的if起作用,目前先用此来规避不起作用的if
        //if((bittest(winInitDone,0))&&(_hWinCardInfo == cur_win))
        if(_hWinCardInfo == cur_win)
	    {
            /**< 信号数据处理 */
            Signal_Show();
            /**< 灯光控制 */
            Led_Show();
            /**< 如果界面发生了切换 */
	        //if((bittest(winInitDone, 0))&&(_hWinCardInfo == cur_win))
		    if(_hWinCardInfo == cur_win)
			{
                /**< 故障分析 */
                Err_Analy(pMsg->hWin);
                /**< 特殊触控点分析 */
                CaliDone_Analy(pMsg->hWin);
				/**< 数据处理 */
	            Data_Process(pMsg);
            
            }
//            CaliDone_Analy(pMsg->hWin);
        }
        break;
    case WM_INIT_DIALOG:
        first_CardInfo = 0;
        //
        // Initialization of 'Framewin'
        //
        FrameWin_Init(pMsg, ID_TEXT_1, ID_TEXT_2, ID_TEXT_3, ID_TEXT_4,ID_IMAGE_0);
        //
        // Initialization of 'Text'
        //
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &XBF24_Font, GUI_BLACK, "卡号");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), &XBF24_Font, GUI_BLACK, "余额");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8), &XBF24_Font, GUI_BLACK, "元");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), &XBF24_Font, GUI_BLACK, "(60S)");
        //
        // Initialization of 'Edit'
        //

        HexToStr(pRFIDDev->order.ucCardID, temp_buf, defCardIDLength);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_0), &XBF24_Font, temp_buf);         //卡号
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1), &XBF24_Font, "?");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "正在获取账户信息...");

        //
        // Initialization of 'Button'
        //
        Button_Show(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1), GUI_TA_LEFT | GUI_TA_VCENTER,
                    &XBF24_Font, BUTTON_CI_UNPRESSED, GUI_BLUE, BUTTON_CI_UNPRESSED, GUI_BLUE, "注册流程");

        Button_Show(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0), GUI_TA_LEFT | GUI_TA_VCENTER,
                    &XBF24_Font, BUTTON_CI_UNPRESSED, GUI_BLUE, BUTTON_CI_UNPRESSED, GUI_BLUE, "退出");
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id)
        {
        case ID_BUTTON_0:
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                first_CardInfo = 0;
                xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件
                _deleteWin(_hWinCardInfo);
                CreateHome();
                break;
            case WM_NOTIFICATION_RELEASED:

                break;
            }
            break;
        case ID_BUTTON_1:
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                _deleteWin(_hWinCardInfo);
                CreateRegiterDisp();
                break;
            case WM_NOTIFICATION_RELEASED:

                break;
            }
            break;
        }
        break;
    case WM_TIMER:
        if(pMsg->Data.v == _timerRTC)
        {
            /**< 显示时间和日期 */
            Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), _secDown);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), strCSQ);
            /**< 重启定时器 */
            WM_RestartTimer(pMsg->Data.v, 20);
        }
        if(pMsg->Data.v == _timerSignal)
        {
            WM_RestartTimer(pMsg->Data.v, 2000);
        }
        if(pMsg->Data.v == _timerData)
        {
            Data_Flush(pMsg);
            WM_RestartTimer(pMsg->Data.v,100);
        }
        break;
    case MSG_CREATERRWIN:
        /**< 故障界面不存在则创建,存在则刷新告警 */
        err_window(pMsg->hWin);
        break;
    case MSG_DELERRWIN:
        /**< 故障界面存在则删除故障界面 */
        if(bittest(winCreateFlag,0))
        {
            bitclr(winCreateFlag,0);
            //WM_DeleteWindow(err_hItem);
            GUI_EndDialog(err_hItem,0);
            err_hItem = 0;
        }
        break;
    case MSG_JUMPHOME:
        _deleteWin(_hWinCardInfo);
        _hWinCardInfo = 0;
        CreateHome();
        break;
    case MSG_JUMPCHAING:
        _deleteWin(_hWinCardInfo);
        _hWinCardInfo = 0;
        CreateCharging();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
/** @brief
*  输出卡片信息界面
* @param
* @param
* @return
*  CreateCardInfo
*/
WM_HWIN CreateCardInfo(void);
WM_HWIN CreateCardInfo(void)
{
    _hWinCardInfo = GUI_CreateDialogBox(_aDialogCardInfo, GUI_COUNTOF(_aDialogCardInfo), _cbCardDialog, WM_HBKWIN, 0, 0);
    cur_win = _hWinCardInfo;
    _timerRTC = WM_CreateTimer(WM_GetClientWindow(_hWinCardInfo), ID_TimerTime, 20, 0);
    _timerData = WM_CreateTimer(WM_GetClientWindow(_hWinCardInfo), ID_TimerFlush,1000,0);
    _timerSignal = WM_CreateTimer(WM_GetClientWindow(_hWinCardInfo), ID_TimerSignal,5000,0);
	bitset(winInitDone,0);
	return 0;
}

// USER START (Optionally insert additional public code)




/*************************** End of file ****************************/



