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
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
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
#define ID_TimerTime    0
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
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 630, 0, 80, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_2, 720, 0, 70, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 540, 0, 90, 16, 0, 0x0, 0 },//网络信号强度
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
// USER START (Optionally insert additional static code)
static void Caculate_RTC(WM_MESSAGE *pMsg)
{
    uint8_t i = 1;
    static uint8_t num = 0;
    uint8_t Timer_buf[10];
    EventBits_t uxBitRFID;
    EventBits_t uxBitCharge;
    CON_t *pCON;

    WM_HWIN hWin = pMsg->hWin;

    pCON = CONGetHandle(0);
    uxBitCharge = xEventGroupGetBits(pCON->status.xHandleEventCharge);
    if((uxBitCharge & defEventBitCONStartOK) == defEventBitCONStartOK)
    {
        /** @todo (zshare#1#): 跳转充电界面 */
        WM_DeleteWindow(pMsg->hWin);
        PutOut_Charging();
    }
    else
    {
        /** @todo (zshare#1#): 充电条件未达成 */
    }
    Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);

    xsprintf((char *)Timer_buf, "(%02dS)", wait_timer.card_info);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_5), Timer_buf);

    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                    defEventBitGoodIDReqDisp,
                                    pdTRUE, pdTRUE, 0);
    if((uxBitRFID & defEventBitGoodIDReqDisp) == defEventBitGoodIDReqDisp)
    {
        pCON = CONGetHandle(0);//选择枪的时候获取pCON

        sprintf(Timer_buf, "%.2lf", pRFIDDev->order.dBalance);
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, " ");
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1), &XBF24_Font, Timer_buf);
        xEventGroupSetBits(pRFIDDev->xHandleEventGroupRFID, defEventBitGoodIDReqDispOK);

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
    /*未进GoodID ,BadID和OweID状态时显示内容*/
    if(pCON->status.xPlugState == UNPLUG)
    {
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &XBF36_Font, GUI_RED, "请连接充电插头");
    }
    /*end of 未进GoodID ,BadID和OweID状态时显示内容*/
}
// USER END
/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE *pMsg)
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
    case WM_INIT_DIALOG:
        //
        // Initialization of 'Framewin'
        //
        FrameWin_Init(pMsg, ID_TEXT_1, ID_TEXT_2, ID_TEXT_3, ID_TEXT_4);
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
        case ID_BUTTON_0: // Notifications sent by 'Button'
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                WM_DeleteWindow(pMsg->hWin);
                PutOut_Home();
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                WM_DeleteWindow(pMsg->hWin);
                PutOut_Home();
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
        case ID_BUTTON_1: // Notifications sent by 'Button'
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                WM_DeleteWindow(pMsg->hWin);
                PutOut_RegisterDisp();
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
                WM_DeleteWindow(pMsg->hWin);
                PutOut_RegisterDisp();
                // USER END
                break;
                // USER START (Optionally insert additional code for further notification handling)
                // USER END
            }
            break;
            // USER START (Optionally insert additional code for further Ids)
            // USER END
        }
        break;
        // USER START (Optionally insert additional message handling)
    case WM_TIMER:
        /* 显示时间和日期 */
        Caculate_RTC(pMsg);
        /* 重启定时器 */
        WM_RestartTimer(pMsg->Data.v, REFLASH);
        break;
        // USER END
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
*       CreateCardInfo
*/
WM_HWIN CreateCardInfo(void);
WM_HWIN CreateCardInfo(void)
{
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogCardInfo, GUI_COUNTOF(_aDialogCardInfo), _cbDialog, WM_HBKWIN, 0, 0);
    WM_CreateTimer(WM_GetClientWindow(hWin), ID_TimerTime, 1000, 0);

    return hWin;
}

// USER START (Optionally insert additional public code)

/** @brief
 *  输出卡片信息界面
 * @param
 * @param
 * @return
 *
 */
void PutOut_Card_Info()//(OrderData_t *order)
{
    WM_HWIN hWin;
    wait_timer.card_info = 60;
    hWin = CreateCardInfo();

    while(1)
    {
        GUI_Delay(500);
        dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        if((wait_timer.card_info--) == 0)
        {
            xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件
            //跳出卡片信息页
            WM_DeleteWindow(hWin);
            PutOut_Home();
        }
        vTaskDelay(500);
    }
}
// USER END

/*************************** End of file ****************************/



