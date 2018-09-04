#include "touchtimer.h"
#include "SCROLLBAR.h"
// USER END

#include "DIALOG.h"

typedef enum power_current
{
    PC_P,  //功率电流之功率
    PC_C,  //功率电流之电流
}PC_OPT;

PC_OPT pc_opt1 = PC_P;

/*编辑窗口14行1列，状态项14个*/
#define _SYSEDIT_MAX_X 2
#define _SYSEDIT_MAX_Y 11
#define _SYSSTATUE_LINE 11
#define _SYSSTATUE_CAL 3
//后续将编辑和文本的滚轮方式用链表进行封装
#define _FONT_WIDTH 24
#define _WORD_WIDTH 20
static uint8_t _aahSysSet[_SYSEDIT_MAX_Y];
static EDIT_Handle   _aahEdit[_SYSEDIT_MAX_Y][_SYSEDIT_MAX_X];
static TEXT_Handle   _aahText[_SYSSTATUE_LINE][_SYSSTATUE_CAL];
static BUTTON_Handle _aahButton[1][1];
static int _x, _y;
static uint8_t manualType = 0;
/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_IMAGE_0     (GUI_ID_USER + 0x1C)
// USER START (Optionally insert additional defines)
#define ID_TEXT_1     (GUI_ID_USER + 0x0B)
#define ID_TEXT_2     (GUI_ID_USER + 0x0C)
#define ID_TEXT_3     (GUI_ID_USER + 0x0D)
#define ID_TEXT_4     (GUI_ID_USER + 0x0E)

#define ID_TEXT_5  (GUI_ID_USER + 0x06)//
#define ID_EDIT_0  (GUI_ID_USER + 0x07)//
#define ID_TEXT_8  (GUI_ID_USER + 0x0A)//
#define ID_TEXT_9  (GUI_ID_USER + 0x0F)//
#define ID_TEXT_10  (GUI_ID_USER + 0x10)//
#define ID_TEXT_11  (GUI_ID_USER + 0x11)//
#define ID_EDIT_1  (GUI_ID_USER + 0x12)//
#define ID_EDIT_2  (GUI_ID_USER + 0x13)//
#define ID_EDIT_3  (GUI_ID_USER + 0x14)//
#define ID_EDIT_4  (GUI_ID_USER + 0x15)//
#define ID_EDIT_5  (GUI_ID_USER + 0x16)//
#define ID_EDIT_6  (GUI_ID_USER + 0x17)//
#define ID_MULTIEDIT_0 (GUI_ID_USER + 0x18)

#define ID_FRAMEWIN_0     (GUI_ID_USER + 0x28)
#define ID_TEXT_6     (GUI_ID_USER + 0x29)
#define ID_TEXT_7  (GUI_ID_USER + 0x2C)//
#define ID_BUTTON_6  (GUI_ID_USER + 0x2A)
#define ID_BUTTON_7  (GUI_ID_USER + 0x2B)

#define ID_TimerTime    1
#define ID_TimerFlush   2
#define ID_TimerSignal  3
// USER END

//#define conID "ID"
#define conType "枪类型"
#define conSocketType "枪锁类型"
#define conVolatageUpperLimits "电压上限"
#define conVolatageLowerLimits "电压下限"
#define conACCurrentUpperLimits "电流上限"
#define conACTempUpperLimits "输出端子温度上限"
#define conACTempLowerLimits "输出端子温度下限"
#define conSocketTempUpperLimits "输入端子温度上限"
#define conSocketTempLowerLimits "输入端子温度下限"
#define conRatedCurrent "额定电流"
#define conRatedPower "额定功率"
#define conQRCode "二维码"
#define conManual "测试充电"

#define MANUALSTART 40

static WM_HWIN hWindow;
WM_HWIN _hWinManagerConSet1;
WM_HWIN _hWin_hard1;
static WM_HTIMER _timerRTC, _timerData, _timerSignal;
extern int manual_charge(void *pvCON, int onoff);

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] =
{
    { WINDOW_CreateIndirect, "window", ID_WINDOW_0, 10, 95, 780, 370, 0, 0x0, 0 },
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreateFrame[] =
{
    { FRAMEWIN_CreateIndirect, "!!!!", ID_FRAMEWIN_0, 240, 45, 300, 200, 0, 0x64, 0 },
    { TEXT_CreateIndirect, "Text6", ID_TEXT_6, 0, 20, 300, 45, TEXT_CF_HCENTER, 0x0, 0 },
    { TEXT_CreateIndirect, "Text7", ID_TEXT_7, 0, 65, 300, 45, TEXT_CF_HCENTER, 0x0, 0 },
    { BUTTON_CreateIndirect, "确定", ID_BUTTON_6, 50, 110, 80, 50, 0, 0x0, 0 },
    { BUTTON_CreateIndirect, "取消", ID_BUTTON_7, 200, 110, 80, 50, 0, 0x0, 0 },
};

static void _cbDialog_frame_changePC(WM_MESSAGE *pMsg)
{
    WM_HWIN      hItem;
    int          NCode;
    int          Id;
    char buff[10];
    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        FRAMEWIN_SetFont(pMsg->hWin, &SIF24_Font);
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_TEXT_6);
        TEXT_SetFont(hItem, &SIF16_Font);
        switch (pc_opt1)
        {
        case PC_C:
            TEXT_SetText(hItem, "电流设置下次充电时生效!");
            break;
        case PC_P:
            TEXT_SetText(hItem, "功率设置下次充电时生效!");
            break;
        default:
            break;
        }
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_TEXT_7);
        TEXT_SetFont(hItem, &SIF16_Font);
        switch (pc_opt1)
        {
        case PC_C:
            TEXT_SetText(hItem, "单相:16-32 三相:16-63");
            break;
        case PC_P:
            TEXT_SetText(hItem, "单相:3.52-7.04 三相:10.56-41.58");
            break;
        default:
            break;
        }
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_6);
        BUTTON_SetFont(hItem, &SIF16_Font);
        BUTTON_SetText(hItem, "继续");
        hItem = WM_GetDialogItem(WM_GetClientWindow(pMsg->hWin), ID_BUTTON_7);
        BUTTON_SetFont(hItem, &SIF16_Font);
        BUTTON_SetText(hItem, "取消");
        break;
    case WM_NOTIFY_PARENT:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        switch (Id) {
        case ID_BUTTON_6:
            switch (NCode)
            {
            case WM_NOTIFICATION_RELEASED:
                WM_HideWindow(_hWinManagerConSet);
                WM_HideWindow(_hWinManagerCommon);
                switch (pc_opt1)
                {
                case PC_C:
                    Keypad_GetValueTest(CONSET_VALUE, 25, _hWinManagerConSet1, _hWinManagerCommon, conRatedCurrent, "单位:A");
                    break;
                case PC_P:
                    Keypad_GetValueTest(CONSET_VALUE, 26, _hWinManagerConSet1, _hWinManagerCommon, conRatedPower, "单位:kW");
                    break;
                default:
                    break;
                }
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        case ID_BUTTON_7:
            switch (NCode)
            {
            case WM_NOTIFICATION_RELEASED:
                GUI_EndDialog(pMsg->hWin, 0);
                break;
            default:
                break;
            }
            break;
        }
    }
}

static void Data_Flush()
{
    CON_t *pCon;
    uint8_t _tmpBuff[50];

    pCon = CONGetHandle(0);

    EDIT_SetText(_aahEdit[0][0], pCon->info.strQRCode);

    memset(_tmpBuff, '\0', sizeof(_tmpBuff));
    sprintf(_tmpBuff, "%d", pCon->info.ucSocketType);
    EDIT_SetText(_aahEdit[1][0], _tmpBuff);

    EDIT_SetText(_aahEdit[2][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageLowerLimits);
    EDIT_SetText(_aahEdit[3][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedCurrent);
    EDIT_SetText(_aahEdit[4][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dACTempUpperLimits);
    EDIT_SetText(_aahEdit[5][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dACTempLowerLimits);
    EDIT_SetText(_aahEdit[6][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dSocketTempUpperLimits);
    EDIT_SetText(_aahEdit[7][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dSocketTempLowerLimits);
    EDIT_SetText(_aahEdit[8][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedCurrent);
    EDIT_SetText(_aahEdit[9][0], _tmpBuff);

    sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedPower);
    EDIT_SetText(_aahEdit[10][0], _tmpBuff);

}

static void _cbWindow(WM_MESSAGE *pMsg) {
    WM_SCROLL_STATE ScrollState;
    WM_HWIN      hItem;
    int x, y;
    volatile int id = 0;
    volatile int v = 0;
    uint8_t _tmpBuff[50];
    CON_t *pCon;
    pCon = CONGetHandle(1);
    switch (pMsg->MsgId)
    {
    case WM_NOTIFY_PARENT:
        /**< 添加两个滑轮的事件 */
        switch (WM_GetId(pMsg->hWinSrc))
        {
        case GUI_ID_HSCROLL://水平
            if (pMsg->Data.v == WM_NOTIFICATION_VALUE_CHANGED)
            {
                /* 得到滚动条的状态，得到的数值好像是负值 才能使得 _x - ScrollState.v是正值 */
                WM_GetScrollState(pMsg->hWinSrc, &ScrollState);
                if (_x != ScrollState.v)
                {
                    for (y = 0; y < _SYSEDIT_MAX_Y; y++)
                    {
                        for (x = 0; x < _SYSEDIT_MAX_X; x++)
                        {
                            WM_MoveWindow(_aahEdit[y][x], _x - ScrollState.v, 0);
                            WM_MoveWindow(_aahText[y][x], _x - ScrollState.v, 0);
                        }
                    }
                    _x = ScrollState.v;
                }
            }
            break;
        case GUI_ID_VSCROLL:
            if (pMsg->Data.v == WM_NOTIFICATION_VALUE_CHANGED)
            {
                WM_GetScrollState(pMsg->hWinSrc, &ScrollState);
                if (_y != ScrollState.v)
                {
                    for (y = 0; y < _SYSEDIT_MAX_Y; y++)
                    {
                        for (x = 0; x < _SYSEDIT_MAX_X; x++)
                        {
                            WM_MoveWindow(_aahEdit[y][x], 0, _y - ScrollState.v);
                            WM_MoveWindow(_aahText[y][x], 0, _y - ScrollState.v);
                        }
                    }
                    _y = ScrollState.v;
                }
            }
            break;
        case 20:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
                //WM_SendMessageNoPara(hWin,MSG_JUMPKEYPAD);
                //                        WM_SetStayOnTop(_hWinManagerConSet,0);
                //                        GUI_EndDialog(_hWinManagerConSet,0);
                //                       _deleteWin(_hWinManagerCommon);
                if(!WM_IsWindow(_hWin_hard1))
                {
                    WM_HideWindow(pMsg->hWin);
                    WM_HideWindow(_hWinManagerCommon);
                    Keypad_GetValueTest(CONSET_VALUE, 20, pMsg->hWin, _hWinManagerCommon, conQRCode, "200000000000003");
                }
            }
            break;
        case 21:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
//                WM_HideWindow(pMsg->hWin);
//                WM_HideWindow(_hWinManagerCommon);
//
//                Keypad_GetValueTest(CONSET_VALUE, 21, pMsg->hWin, _hWinManagerCommon, conSocketType, "Note: B or C");
            }
            break;
        case 22:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
                if (!WM_IsWindow(_hWin_hard1))
                {
                    WM_HideWindow(pMsg->hWin);
                    WM_HideWindow(_hWinManagerCommon);

                    Keypad_GetValueTest(CONSET_VALUE, 22, pMsg->hWin, _hWinManagerCommon, conVolatageUpperLimits, "lowest-280");   
                }
                //                    memset(_tmpBuff, '\0', sizeof(_tmpBuff));
                //                    sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageUpperLimits);
                //                    EDIT_SetText(_aahEdit[2][0], _tmpBuff);
            }
            break;
        case 23:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
                if (!WM_IsWindow(_hWin_hard1))
                {
                    WM_HideWindow(pMsg->hWin);
                    WM_HideWindow(_hWinManagerCommon);

                    Keypad_GetValueTest(CONSET_VALUE, 23, pMsg->hWin, _hWinManagerCommon, conVolatageLowerLimits, "100-highest");
                }
                //                    memset(_tmpBuff, '\0', sizeof(_tmpBuff));
                //                    sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageLowerLimits);
                //                    EDIT_SetText(_aahEdit[3][0], _tmpBuff);
            }
            break;
        case 24:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
                //电流过流值不可设 == 由额定电流计算得到
                //                    WM_HideWindow(pMsg->hWin);
                //                    WM_HideWindow(_hWinManagerCommon);
                //
                //                    Keypad_GetValueTest(CONSET_VALUE, 24, pMsg->hWin, _hWinManagerCommon, conACCurrentUpperLimits, "eg,38");
                //                    sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedCurrent);
                //                    EDIT_SetText(_aahEdit[4][0], _tmpBuff);
            }
            break;
        case 25:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
                if (!WM_IsWindow(_hWin_hard1))
                {
                    pc_opt1 = PC_C;
                    _hWin_hard1 =  GUI_CreateDialogBox(_aDialogCreateFrame, GUI_COUNTOF(_aDialogCreateFrame), _cbDialog_frame_changePC, _hWinManagerConSet1, 0, 0);       
                    WM_MakeModal(_hWin_hard1);
                }
            }
            break;
        case 26:
            if (pMsg->Data.v == WM_NOTIFICATION_RELEASED)
            {
                if (!WM_IsWindow(_hWin_hard1))
                {
                    pc_opt1 = PC_P;
                    _hWin_hard1 =  GUI_CreateDialogBox(_aDialogCreateFrame, GUI_COUNTOF(_aDialogCreateFrame), _cbDialog_frame_changePC, _hWinManagerConSet1, 0, 0);
                    WM_MakeModal(_hWin_hard1);
                }
            }
            break;
        }
        break;
    case WM_PAINT:
        /**< 绘制滑轮围绕区颜色 */
        GUI_SetBkColor(GUI_WHITE);
        GUI_Clear();
        break;
    case MSG_MANAGERSETID0:
        EDIT_SetText(_aahEdit[0][0], pCon->info.strQRCode);
        break;
    case MSG_MANAGERSETID1:
        sprintf(_tmpBuff, "%c", pCon->info.ucSocketType);
        EDIT_SetText(_aahEdit[1][0], _tmpBuff);
        break;
    case MSG_MANAGERSETID2:
        sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageUpperLimits);
        EDIT_SetText(_aahEdit[2][0], _tmpBuff);
        break;
    case MSG_MANAGERSETID3:
        sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageLowerLimits);
        EDIT_SetText(_aahEdit[3][0], _tmpBuff);
        break;
    case MSG_MANAGERSETID4:
        sprintf(_tmpBuff, "%.2lf", (pCon->info.dRatedCurrent * 1.1));
        EDIT_SetText(_aahEdit[4][0], _tmpBuff);
        break;
    case MSG_MANAGERSETID5:
        sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedCurrent);
        EDIT_SetText(_aahEdit[5][0], _tmpBuff);
        sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedPower);
        EDIT_SetText(_aahEdit[6][0], _tmpBuff);
        sprintf(_tmpBuff, "%.2lf", (pCon->info.dRatedCurrent * 1.1));
        EDIT_SetText(_aahEdit[4][0], _tmpBuff);
        break;
    case MSG_MANAGERSETID6:
        sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedCurrent);
        EDIT_SetText(_aahEdit[5][0], _tmpBuff);
        sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedPower);
        EDIT_SetText(_aahEdit[6][0], _tmpBuff);
        sprintf(_tmpBuff, "%.2lf", (pCon->info.dRatedCurrent * 1.1));
        EDIT_SetText(_aahEdit[4][0], _tmpBuff);
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}
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
    int x, y, _editxoff = 0;
    SCROLLBAR_Handle hScroll;
    SCROLLBAR_Handle wScroll;
    char _tmpBuff[50];
    CON_t *pCon;
    int i;

    pCon = CONGetHandle(1);
    // USER START (Optionally insert additional variables)
    // USER END

    switch (pMsg->MsgId)
    {
    case WM_INIT_DIALOG:
        _x = 0;
        _y = 0;
        // 创建窗口250
        hWindow = pMsg->hWin;//WM_CreateWindow(230, 70, 520, 350, WM_CF_SHOW, &_cbWindow, 0);
        WM_SetCallback(pMsg->hWin, &_cbWindow);
//        //创建水平滑轮
//        hScroll = SCROLLBAR_CreateAttached(hWindow, 0);//水平滑轮
//        //设置滑轮条目数量
//        SCROLLBAR_SetNumItems(hScroll, _SYSEDIT_MAX_X);
//        //设置页尺寸
//        //SCROLLBAR_SetPageSize(hScroll, 220);
//        SCROLLBAR_SetWidth(hScroll,HSCROLL_WIDTH);
        //创建垂直滑轮
        wScroll = SCROLLBAR_CreateAttached(hWindow, SCROLLBAR_CF_VERTICAL);//垂直滑轮
//        i = SCROLLBAR_GetNumItems(wScroll);
//        printf_safe("%d\n\n\n", i);
//        i = SCROLLBAR_GetPageSize(wScroll);
//        printf_safe("%d\n\n\n", i);
        //设置滑轮条目数量
        //SCROLLBAR_SetNumItems(wScroll, 25 * 9);
        //当前会占用一页
        SCROLLBAR_SetPageSize(wScroll, 90);
        //SCROLLBAR_SetNumItems(wScroll, 180);
        SCROLLBAR_SetNumItems(wScroll, 2 * 90);
        
        //设置页尺寸
        //SCROLLBAR_SetPageSize(wScroll, 220);
        SCROLLBAR_SetWidth(wScroll, WSCROLL_WIDTH);

        //创建文本区 -- 24号字体 4-96 5-120 6-144 7-168 8-192
        /**< 13文本固定id */
        //创建编辑区
        /**< 20-50编辑区ID */
        //枪类型

        i = _FONT_WIDTH*(strlen(conQRCode));
        //printf_safe("%d\n\n\n", i);
        _editxoff = GUI_MANAGER_XLEFT + _FONT_WIDTH*(strlen(conQRCode))/2 + 50; //+ 50;
        //可以创建一个固定长度的text 和 edit
        _aahText[0][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT, _FONT_WIDTH*(strlen(conQRCode)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conQRCode);
        _aahEdit[0][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT, _WORD_WIDTH*strlen("112233445566778812345678")*3/4, GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 20, strlen("112233445566778812345678"));
        EDIT_SetText(_aahEdit[0][0], pCon->info.strQRCode);
        //枪锁类型
//        _aahText[1][0] = strlen(conQRCode); //= 6// qrcode
//        _aahEdit[1][0] = strlen(conSocketType);// = 12 枪锁类型
        _aahText[1][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF, _FONT_WIDTH*(strlen(conSocketType)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conSocketType);
        _aahEdit[1][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF, _WORD_WIDTH*(strlen("255")), GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 21, strlen("255"));
        memset(_tmpBuff, '\0', strlen(_tmpBuff));
        sprintf(_tmpBuff, "%c", pCon->info.ucSocketType);
        EDIT_SetText(_aahEdit[1][0], _tmpBuff);
        EDIT_SetBkColor(_aahEdit[1][0], EDIT_CI_ENABLED, GUI_GRAY);
        //电压上限
        _aahText[2][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 2, _FONT_WIDTH*(strlen(conVolatageUpperLimits)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conVolatageUpperLimits);
        _aahEdit[2][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 2, _WORD_WIDTH*(strlen("255.5")), GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 22, strlen("255.5"));
        _aahText[2][1] = TEXT_CreateEx(_editxoff + _WORD_WIDTH*(strlen("255.5")), GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 2, _WORD_WIDTH*(strlen(" V")), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, " V");
        memset(_tmpBuff, '\0', strlen(_tmpBuff));
        sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageUpperLimits);
        EDIT_SetText(_aahEdit[2][0], _tmpBuff);
        //电压下限
        _aahText[3][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 3, _FONT_WIDTH*(strlen(conVolatageLowerLimits)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conVolatageLowerLimits);
        _aahEdit[3][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 3, _WORD_WIDTH*(strlen("255.5")), GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 23, strlen("255.5"));
        _aahText[3][1] = TEXT_CreateEx(_editxoff + _WORD_WIDTH*(strlen("255.5")), GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 3, _WORD_WIDTH*(strlen(" V")), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, " V");
        memset(_tmpBuff, '\0', strlen(_tmpBuff));
        sprintf(_tmpBuff, "%.2lf", pCon->info.dVolatageLowerLimits);
        EDIT_SetText(_aahEdit[3][0], _tmpBuff);
        //电流上限
        _aahText[4][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 4, _FONT_WIDTH*(strlen(conACCurrentUpperLimits)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conACCurrentUpperLimits);
        _aahEdit[4][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 4, _WORD_WIDTH*(strlen("255.5")), GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 24, strlen("255.5"));
        _aahText[4][1] = TEXT_CreateEx(_editxoff + _WORD_WIDTH*(strlen("255.5")), GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 4, _WORD_WIDTH*(strlen(" A")), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, " A");
        memset(_tmpBuff, '\0', strlen(_tmpBuff));
        /// TODO (zshare#1#): 电流上限设置值？？？？？
        sprintf(_tmpBuff, "%.2lf", (pCon->info.dRatedCurrent * 1.1));
        EDIT_SetText(_aahEdit[4][0], _tmpBuff);
        EDIT_SetBkColor(_aahEdit[4][0], EDIT_CI_ENABLED, GUI_GRAY);
        //额定电流
        _aahText[5][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 5, _FONT_WIDTH*(strlen(conRatedCurrent)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conRatedCurrent);
        _aahEdit[5][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 5, _WORD_WIDTH*(strlen("145.4")), GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 25, strlen("145.4"));
        _aahText[5][1] = TEXT_CreateEx(_editxoff + _WORD_WIDTH*(strlen("145.4")), GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 5, _WORD_WIDTH*(strlen(" A")), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, "A");
        sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedCurrent);
        EDIT_SetText(_aahEdit[5][0], _tmpBuff);
        //EDIT_SetBkColor(_aahEdit[9][0], EDIT_CI_ENABLED, GUI_GRAY);
        //额定功率
        _aahText[6][0] = TEXT_CreateEx(GUI_MANAGER_XLEFT, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 6, _FONT_WIDTH*(strlen(conRatedPower)), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, conRatedPower);
        _aahEdit[6][0] = EDIT_CreateEx(_editxoff, GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 6, _WORD_WIDTH*(strlen("145.4")), GUI_MANAGER_YSIZE, hWindow, WM_CF_SHOW, 0, 26, strlen("145.4"));
        _aahText[6][1] = TEXT_CreateEx(_editxoff + _WORD_WIDTH*(strlen("145.4")), GUI_MANAGER_YLEFT + GUI_MANAGER_YOFF * 6, _WORD_WIDTH*(strlen(" kW")), GUI_MANAGER_YOFF, hWindow, WM_CF_SHOW, 0, 13, "kW");
        sprintf(_tmpBuff, "%.2lf", pCon->info.dRatedPower);
        EDIT_SetText(_aahEdit[6][0], _tmpBuff);
       // EDIT_SetBkColor(_aahEdit[10][0], EDIT_CI_ENABLED, GUI_GRAY);
        
        for (x = 0; x < _SYSSTATUE_LINE; x++)
        {
            for (y = 0; y < _SYSSTATUE_CAL; y++)
            {
                TEXT_SetFont(_aahText[x][y], &SIF24_Font);
                TEXT_SetTextColor(_aahText[x][y], GUI_BLACK);
            }
        }

        for (y = 0; y < _SYSEDIT_MAX_Y; y++)
        {
            for (x = 0; x < _SYSEDIT_MAX_X; x++)
            {
                EDIT_SetTextAlign(_aahEdit[y][x], GUI_TA_HCENTER | GUI_TA_VCENTER);
                EDIT_SetFont(_aahEdit[y][x], &SIF24_Font);
                WIDGET_SetEffect(_aahEdit[y][x], &WIDGET_Effect_3D);//不需要设置的就改成 &WIDGET_Effect_None模式
                //WIDGET_SetEffect(_aahEdit[y][x], &WIDGET_Effect_None);
            }
        }
       // WM_SetStayOnTop(hWindow, 1);
        break;
        // USER START (Optionally insert additional message handling)
//    case MSG_CREATERRWIN:
//        /**< 故障界面不存在则创建,存在则刷新告警 */
//        err_window(pMsg->hWin);
//        break;
//    case MSG_DELERRWIN:
//        /**< 故障界面存在则删除故障界面 */
//        if(bittest(winCreateFlag,0))
//        {
//            bitclr(winCreateFlag,0);
//            GUI_EndDialog(err_hItem,0);
//            err_hItem = 0;
//        }
//        break;
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
/** @brief
 *  输出管理员界面里的系统配置
 * @param
 * @param
 * @return
 *
 *
 *       CreateManagerConSet
*/
WM_HWIN CreateManagerConSet_1(WM_HWIN srcHwin)
{
    _hWinManagerConSet1 = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_GetClientWindow(srcHwin), 0, 0);
    //    cur_win = _hWinManagerConSet;
    return _hWinManagerConSet1;
}
/*************************** End of file ****************************/





