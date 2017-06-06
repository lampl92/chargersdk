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
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x00)
#define ID_BUTTON_1     (GUI_ID_USER + 0x1B)
#define ID_IMAGE_0      (GUI_ID_USER + 0x1C)
// USER START (Optionally insert additional defines)
#define ID_TEXT_1     (GUI_ID_USER + 0x01)
#define ID_TEXT_2     (GUI_ID_USER + 0x02)
#define ID_TEXT_3     (GUI_ID_USER + 0x03)
#define ID_TEXT_4     (GUI_ID_USER + 0x04)

#define ID_TEXT_5     (GUI_ID_USER + 0x05)
#define ID_TEXT_6     (GUI_ID_USER + 0x06)
#define ID_TEXT_7     (GUI_ID_USER + 0x07)
#define ID_TEXT_8     (GUI_ID_USER + 0x08)
#define ID_TEXT_9     (GUI_ID_USER + 0x09)
#define ID_TEXT_10    (GUI_ID_USER + 0x0A)
#define ID_EDIT_0     (GUI_ID_USER + 0x0B)
#define ID_EDIT_1     (GUI_ID_USER + 0x0C)
#define ID_EDIT_2     (GUI_ID_USER + 0x0D)
#define ID_EDIT_3     (GUI_ID_USER + 0x0E)
#define ID_TEXT_11    (GUI_ID_USER + 0x0F)
#define ID_TEXT_12    (GUI_ID_USER + 0x10)
#define ID_TEXT_13    (GUI_ID_USER + 0x11)
#define ID_TEXT_14    (GUI_ID_USER + 0x12)
#define ID_EDIT_4     (GUI_ID_USER + 0x13)
#define ID_TEXT_15    (GUI_ID_USER + 0x14)
#define ID_EDIT_5     (GUI_ID_USER + 0x15)
#define ID_TEXT_16    (GUI_ID_USER + 0x16)
#define ID_EDIT_6     (GUI_ID_USER + 0x17)
#define ID_TEXT_17    (GUI_ID_USER + 0x18)
#define ID_EDIT_7     (GUI_ID_USER + 0x19)
#define ID_TEXT_18    (GUI_ID_USER + 0x1A)
#define ID_TEXT_19    (GUI_ID_USER + 0x1B)

#define ID_TimerTime    0
// USER END
static uint8_t first_flag = 0;
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
*       _aDialogChargeDone
*/
static const GUI_WIDGET_CREATE_INFO _aDialogChargeDone[] =
{
    { FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 800, 480, 0, 0x64, 0 },
    { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 789, 459, 0, 0, 0 },
    // USER START (Optionally insert additional widgets)
    { TEXT_CreateIndirect, "Text", ID_TEXT_1, 630, 0, 80, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_2, 720, 0, 70, 16, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_3, 540, 0, 90, 16, 0, 0x0, 0 },//网络信号强度
    { TEXT_CreateIndirect, "Text", ID_TEXT_4, 225, 367, 300, 20, 0, 0x0, 0 },//最底端的说明
    //{ BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 627, 342, 100, 36, 0, 0x0, 0 },//停止充电

    { BUTTON_CreateIndirect, "退出", ID_BUTTON_1, 554, 307, 100, 36, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_18, 600, 314, 61, 24, 0, 0x0, 0 },


    { TEXT_CreateIndirect, "Text", ID_TEXT_5, 300, 40, 326, 35, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_6, 190, 100, 120, 35, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_7, 222, 159, 100, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_8, 222, 199, 120, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_9, 222, 237, 100, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_10, 222, 275, 100, 30, 0, 0x0, 0 },
    { EDIT_CreateIndirect, "Edit", ID_EDIT_0, 360, 159, 88, 30, 0, 0x64, 0 },
    { EDIT_CreateIndirect, "Edit", ID_EDIT_1, 360, 199, 88, 30, 0, 0x64, 0 },
    { EDIT_CreateIndirect, "Edit", ID_EDIT_2, 360, 237, 88, 30, 0, 0x64, 0 },
    { EDIT_CreateIndirect, "Edit", ID_EDIT_3, 360, 275, 88, 30, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_11, 460, 159, 80, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_12, 460, 199, 80, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_13, 460, 237, 80, 30, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Text", ID_TEXT_14, 460, 275, 80, 30, 0, 0x0, 0 },
    { EDIT_CreateIndirect, "Edit", ID_EDIT_4, 290, 100, 45, 35, 0, 0x64, 0 },//小时数值
    { TEXT_CreateIndirect, "Text", ID_TEXT_15, 335, 100, 70, 35, 0, 0x0, 0 },//小时
    { EDIT_CreateIndirect, "Edit", ID_EDIT_5, 405, 100, 45, 35, 0, 0x64, 0 },//分钟数值
    { TEXT_CreateIndirect, "Text", ID_TEXT_16, 450, 100, 70, 35, 0, 0x0, 0 },//分钟
    { EDIT_CreateIndirect, "Edit", ID_EDIT_6, 500, 100, 45, 35, 0, 0x64, 0 },//秒数值
    { TEXT_CreateIndirect, "Text", ID_TEXT_17, 545, 100, 70, 35, 0, 0x0, 0 },//秒钟
//    { TEXT_CreateIndirect, "Text", ID_TEXT_19, 300, 314, 100, 35, 0, 0x0, 0 },//正在结费中
    // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
// USER START (Optionally insert additional static code)
static void Timer_Process(WM_MESSAGE *pMsg)
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



    WM_HWIN hWin = pMsg->hWin;
    CaliDone_Analy(hWin);
    Caculate_RTC_Show(pMsg,ID_TEXT_1,ID_TEXT_2);

    if(first_flag == 0)
    {
        first_flag = 1;
        first = time(NULL);
    }

//    pCON = CONGetHandle(0);
//    now = time(NULL);
//    diffsec = (uint32_t)difftime(now, pCON->order.tStartTime);
//    if(diffsec > 86400)
//    {
//        diffsec = 86400;
//    }
//    hour = diffsec / 3600;
//    min = diffsec % 3600 / 60;
//    sec = diffsec % 3600 % 60;
#if 0
    sprintf(temp_buf, "%02d", hour);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_4), temp_buf);//已充电时间小时
    sprintf(temp_buf, "%02d", min);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_5), temp_buf);// min
    sprintf(temp_buf, "%02d", sec);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_6), temp_buf);// sec

    sprintf(temp_buf, "%.2lf",  pCON->order.dTotalPower);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_0), temp_buf);
    sprintf(temp_buf, "%.2lf", pCON->order.dTotalServiceFee);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_1), temp_buf);
    sprintf(temp_buf, "%.2lf", pCON->order.dTotalPowerFee);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_2), temp_buf);
    sprintf(temp_buf, "%.2lf", pCON->order.dTotalFee);
    EDIT_SetText(WM_GetDialogItem(hWin, ID_EDIT_3), temp_buf);
#endif

//    if((60-(uint32_t)difftime(now, first)) == 0)
//    {
//        WM_DeleteWindow(hWin);
//        PutOut_Home();
//    }
    now = time(NULL);
//    if(first_flag == 0)
//    {
//        first_flag = 1;
//        first = now;
//    }
    diffsec = (uint32_t)difftime(now, first);
//    if(diffsec > 86400)
//    {
//        diffsec = 86400;
//    }
//    hour = diffsec / 3600;
//    min = diffsec % 3600 / 60;
//    sec = diffsec % 3600 % 60;
    sec = 10 - diffsec;
    if(sec < 0)
        sec = 0;
    xsprintf((char *)Timer_buf, "(%02dS)", sec);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_18), Timer_buf);
    if(sec == 0)
    {
        first_flag = 0;
        xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件
        //跳出卡片信息页
        WM_DeleteWindow(hWin);

        //PutOut_SelAOrB();
        PutOut_Home();
    }
    ErrWindow_Show(hWin);
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
    CON_t *pCON;
    uint32_t time_charge;
    uint8_t temp_buf[32];
    volatile int8_t hour;
    volatile int8_t min;
    volatile int8_t sec;
    EventBits_t uxBits;


    // USER START (Optionally insert additional variables)
    // USER END

    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        //
        //设置本页焦点
        //
        WM_SetFocus(pMsg->hWin);
        //
        // Initialization of 'Framewin'
        //
        FrameWin_Init(pMsg, ID_TEXT_1, ID_TEXT_2, ID_TEXT_3, ID_TEXT_4,ID_IMAGE_0);

        //Initialization of 'Text'

        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5),&XBF36_Font,GUI_BLACK,"充电完成");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6),&XBF24_Font,GUI_BLACK,"充电时长");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7),&XBF24_Font,GUI_BLACK,"充入电量");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8),&XBF24_Font,GUI_BLACK,"当前服务费");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_9),&XBF24_Font,GUI_BLACK,"当前电费");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_10),&XBF24_Font,GUI_BLACK,"消费总额");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_11),&XBF24_Font,GUI_BLACK,"度");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_12),&XBF24_Font,GUI_BLACK,"元/单");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_13),&XBF24_Font,GUI_BLACK,"元/度");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_14),&XBF24_Font,GUI_BLACK,"元");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_15),&XBF24_Font,GUI_BLACK,"小时");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_16),&XBF24_Font,GUI_BLACK,"分");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_17),&XBF24_Font,GUI_BLACK,"秒");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_18),&XBF24_Font,GUI_BLACK,"(00S)");
//        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_19),&XBF24_Font,GUI_RED,"正在结费中...");
        //
        // Initialization of 'Edit'
        //
        pCON = CONGetHandle(0);
        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%.2lf",  pCON->order.dTotalPower);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_0),&XBF24_Font,temp_buf);
        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%.2lf", pCON->order.dTotalServiceFee);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_1),&XBF24_Font,temp_buf);
        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%.2lf", pCON->order.dTotalPowerFee);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_2),&XBF24_Font,temp_buf);
        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%.2lf", pCON->order.dTotalFee);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_3),&XBF24_Font,temp_buf);

        uxBits = xEventGroupWaitBits(pCON->status.xHandleEventOrder,
                                     defEventBitOrderMakeFinish,
                                     pdFALSE, pdTRUE, portMAX_DELAY);
        //if((uxBits & defEventBitOrderMakeFinish) == defEventBitOrderMakeFinish)xxx
        time_charge = pCON->order.tStopTime - pCON->order.tStartTime;
        hour = time_charge / 3600;
        min = time_charge % 3600 / 60;
        sec = time_charge % 3600 % 60;

        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%02d", hour);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_4),&XBF24_Font,temp_buf);//小时
        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%02d", min);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_5),&XBF24_Font,temp_buf);//分
        memset(temp_buf,'\0',strlen(temp_buf));
        sprintf(temp_buf, "%02d", sec);
        Edit_Show(WM_GetDialogItem(pMsg->hWin, ID_EDIT_6),&XBF24_Font,temp_buf);//秒

        Button_Show(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1),GUI_TA_LEFT|GUI_TA_VCENTER,
                    &XBF24_Font,BUTTON_CI_DISABLED,GUI_BLUE,BUTTON_CI_DISABLED,GUI_BLUE,"退出");
        xEventGroupSetBits(xHandleEventHMI,defeventBitHMI_ChargeReqDispDoneOK);
        // USER END
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch(Id)
        {
        case ID_BUTTON_1: // Notifications sent by 'Button'
            switch(NCode)
            {
            case WM_NOTIFICATION_CLICKED:
                // USER START (Optionally insert code for reacting on notification message)
                first_flag = 0;
                WM_DeleteWindow(pMsg->hWin);
                xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件

                //PutOut_SelAOrB();
                PutOut_Home();
                // USER END
                break;
            case WM_NOTIFICATION_RELEASED:
                // USER START (Optionally insert code for reacting on notification message)
//                first_flag = 0;
//                WM_DeleteWindow(pMsg->hWin);
//                xEventGroupSetBits(xHandleEventHMI, defEventBitHMITimeOutToRFID);//发送HMI显示延时到事件
//                vTaskDelay(500);
//                //PutOut_SelAOrB();
//                PutOut_Home();
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
        Timer_Process(pMsg);
        /* 重启定时器 */
        WM_RestartTimer(pMsg->Data.v, 200);
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
*       CreateChargeDone
*/
WM_HWIN CreateChargeDone(void);
WM_HWIN CreateChargeDone(void)
{
    WM_HWIN hWin;

    hWin = GUI_CreateDialogBox(_aDialogChargeDone, GUI_COUNTOF(_aDialogChargeDone), _cbDialog, WM_HBKWIN, 0, 0);
    WM_CreateTimer(WM_GetClientWindow(hWin), ID_TimerTime, 1000, 0);

    return hWin;
}

// USER START (Optionally insert additional public code)

/** @brief
 *  输出充电完成界面
 * @param
 * @param
 * @return
 *
 */
void PutOut_Charge_Done()
{
    WM_HWIN hWin;

    hWin = CreateChargeDone();
    led_ctrl(1,green,keep_on);
    while(1)
    {
        GUI_Delay(500);
        dispbmp("system/dpc.bmp", 0, 5, 5, 1, 1);
        vTaskDelay(500);
    }
}
// USER END

/*************************** End of file ****************************/




