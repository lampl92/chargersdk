/**
* @file Touch_Disp_Module.c
* @brief 封装控件的显示函数
* @author share
* @version v1.0
* @date 2017-03-02
*/
#include "touchtimer.h"
#include "DIALOG.h"

#define ERR_SIMBOL  (0x3e000)

uint8_t *cur_err = "       当前故障列表\n";
uint8_t *cur_noerr = "\n   当前没有故障\n";
uint8_t *Scram_err = "   急停异常\n";
uint8_t *Volt_err = "   充电电压异常\n";
uint8_t *ACTemp_err = "   温度异常\n";
uint8_t *PE_err = "   PE异常\n";
uint8_t *Knock_err = "   撞击异常\n";
uint8_t *Arrester_err = "   防雷异常\n";
uint8_t *PowerOff_err = "   停电异常\n";
uint8_t *Curr_err = "   电流异常\n";
uint8_t *Freq_err = "   频率异常\n";

WM_HWIN err_hItem = 0;

/**< winCreateFlag故障字说明
*       bit0:故障窗口是否已经创建
*       bit1:是否存在故障
*       bit2:键盘页鼠标是否归位(0,0)1归位,0不归位
*/
uint8_t winCreateFlag = 0;

static EventBits_t uxBitsErrTmp;
static uint8_t timer_count = 0;//用于故障列表存在且没有故障的计时

extern p_inf *HomeImage;


//uint8_t bitset(uint32_t var,uint8_t bitno)            //置位
//{
//    return ((var) |= (1<<(bitno)));
//}
//
//uint8_t bitclr(uint32_t var,uint8_t bitno)
//{
//    return ((var) &= ~(1<<(bitno)));         //清0
//}
//
//uint8_t bittest(uint32_t var,uint8_t bitno)
//{
//    return ((var >> bitno)& 0x01);           //位状态检测
//}


/** framewin初始化
 *
 * pMsg:消息体 ,textid0:文本0 ,textid1:文本1, textid2：文本2，textid3:文本3
 * @param
 * @return
 *
 */
void FrameWin_Init(WM_MESSAGE *pMsg, uint16_t textid0, uint16_t textid1, uint16_t textid2, uint16_t textid3, uint16_t imageBack)
{
    FrameWin_Show(pMsg->hWin, GUI_TA_HCENTER | GUI_TA_VCENTER, 40, &SIF24_Font, GUI_RED, "欢迎使用北京动力源交流充电桩");
//    IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, imageBack), HomeImage->pfilestring, HomeImage->pfilesize);
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid2), &SIF16_Font, GUI_RED, "信号:");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid3), &SIF16_Font, GUI_BLACK, "感谢您为空气的清新奉献一份力量");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid0), &SIF16_Font, GUI_RED, "2017-02-28");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid1), &SIF16_Font, GUI_RED, "14:00:00");
    WM_SetFocus(pMsg->hWin);
}
//void FrameWin_Init_test(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3,uint16_t imageBack)
//{
//    FrameWin_Show(pMsg->hWin,GUI_TA_HCENTER | GUI_TA_VCENTER,40,&XBF24_Font,GUI_RED,"欢迎使用北京动力源交流充电桩");
//    dispBackGroundNOFREE("system/background.bmp");
////    GUI_BMP_Draw(bmpBackGround,0,40);
//    IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, imageBack), bmpBackGround, BMPFile_BCGROUND.obj.objsize);
//    Text_Show(WM_GetDialogItem(pMsg->hWin, textid2),&XBF16_Font,GUI_RED,"信号：强");
//    Text_Show(WM_GetDialogItem(pMsg->hWin, textid3),&XBF16_Font,GUI_BLACK,"感谢您为空气的清新奉献一份力量");
//    Text_Show(WM_GetDialogItem(pMsg->hWin, textid0),&XBF16_Font,GUI_RED,"2017-02-28");
//    Text_Show(WM_GetDialogItem(pMsg->hWin, textid1),&XBF16_Font,GUI_RED,"14:00:00");
//    WM_SetFocus(pMsg->hWin);
//}
/** window初始化
 *
 * pMsg:消息体 ,textid0:文本0 ,textid1:文本1, textid2：文本2，textid3:文本3
 * @param
 * @return
 *
 */
void Window_Init(WM_MESSAGE *pMsg, uint16_t textid0, uint16_t textid1, uint16_t textid2, uint16_t textid3, uint16_t imageBack)
{
    IMAGE_SetBMP(WM_GetDialogItem(pMsg->hWin, imageBack), HomeImage->pfilestring, HomeImage->pfilesize);
    //FrameWin_Show(pMsg->hWin,GUI_TA_HCENTER | GUI_TA_VCENTER,40,&XBF24_Font,GUI_RED,"欢迎使用北京动力源交流充电桩");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid2), &SIF16_Font, GUI_RED, "信号：强");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid3), &SIF16_Font, GUI_BLACK, "感谢您为空气的清新奉献一份力量");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid0), &SIF16_Font, GUI_RED, "2017-02-28");
    Text_Show(WM_GetDialogItem(pMsg->hWin, textid1), &SIF16_Font, GUI_RED, "14:00:00");
    WM_SetFocus(pMsg->hWin);
}
/** framewin初始化
 *
 * pMsg:消息体 ,textid0:文本0 ,textid1:文本1, textid2：文本2，textid3:文本3
 * @param
 * @return
 *
 */
void Caculate_RTC_Show(WM_MESSAGE *pMsg, uint16_t textid0, uint16_t textid1)
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    WM_HWIN hWin = pMsg->hWin;
    uint8_t Date_buf[30];
    uint8_t Time_buf[30];

    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
    sprintf((char *)Date_buf, "20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
    TEXT_SetText(WM_GetDialogItem(hWin, textid0), Date_buf);

    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    sprintf((char *)Time_buf, "%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    TEXT_SetText(WM_GetDialogItem(hWin, textid1), Time_buf);
    //printf_safe("time = %s , %02d:%02d:%02d \n", Time_buf, RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
}
/**信号数据刷新
 *
 *
 * @param
 * @return
 *
 */
/// TODO (zshare#1#): ///在此函数中刷新控件TEXT_SETTEXT,text3的信号后,重启定时器2(cardinfo.c中)会进hardfault
void Signal_Show()//(WM_MESSAGE *pMsg,uint16_t textid3)
{
    EventBits_t uxBits;

    memset(strCSQ, '\0', strlen(strCSQ));
    sprintf(strCSQ, "信号:%.2d", pModem->status.ucSignalQuality);
    //uxBits = xEventGroupGetBits(xHandleEventTCP);
    //if((uxBits & defEventBitTCPConnectOK) != defEventBitTCPConnectOK)

    if ((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Registed) != defSignalEVSE_State_Network_Registed)
    {
        strcat(strCSQ, " 服务器未连接");
    }
    else
    {
        strcat(strCSQ, " 服务器已连接");
    }
}

/*
*函数功能：返回信号强度
*    参数：无
*  返回值：信号强度“返回值分别为0,1,2,3，数值越大信号越强”
*/
int getSignalIntensity()
{
    if ((pEVSE->status.ulSignalState & defSignalEVSE_State_Network_Registed) != defSignalEVSE_State_Network_Registed)
    {
        return 0;
    }
    else
    {
        if ((pModem->status.ucSignalQuality) > 0 && (pModem->status.ucSignalQuality) < 6)
        {
            return 1;
        }
        else if ((pModem->status.ucSignalQuality) > 5 && (pModem->status.ucSignalQuality) < 12)
        {
            return 2;
        }
        else if ((pModem->status.ucSignalQuality) > 11 && (pModem->status.ucSignalQuality) < 18)
        {
            return 3;
        }
        else if ((pModem->status.ucSignalQuality) > 17 && (pModem->status.ucSignalQuality) < 24)
        {
            return 4;
        }
        else if ((pModem->status.ucSignalQuality) > 23 && (pModem->status.ucSignalQuality) < 30)
        {
            return 5;
        }
        else
        {
            return 0;
        }
    }
}

/** framewin初始化
 *
 * WM_WIN:源窗口 ,align:对齐方式,heigh:窗口高度,font:字体类型,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *
 */
void FrameWin_Show(WM_HWIN hItem, uint8_t aglin, uint8_t heigh, GUI_FONT *font, uint32_t color, uint8_t *buf)
{
    FRAMEWIN_SetTextAlign(hItem, aglin);
    FRAMEWIN_SetTitleHeight(hItem, heigh);
    FRAMEWIN_SetFont(hItem, font);
    FRAMEWIN_SetTextColor(hItem, color);
    FRAMEWIN_SetText(hItem, buf);
//    FRAMEWIN_SetDefaultBarColor(1,GUI_BLUE);
//    FRAMEWIN_AddMinButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
}
/** Text显示
 *
 * WM_WIN:源窗口,font:字体类型,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *
 */
void Text_Show(WM_HWIN hItem, GUI_FONT *font, uint32_t color, uint8_t *buf)
{
    TEXT_SetFont(hItem, font);
    TEXT_SetTextColor(hItem, color);
    TEXT_SetText(hItem, buf);
}
/** Edit用作带外方框的显示
 *
 * WM_WIN:源窗口,font:字体类型,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 * note；此处并不做编辑处理,没有做颜色处理
 */
void Edit_Show(WM_HWIN hItem, GUI_FONT *font, uint8_t *buf)
{
    EDIT_SetFont(hItem, font);
    EDIT_SetText(hItem, buf);
}
/** Button显示
 *
 * WM_WIN:源窗口,aglin:文字对齐方式,font:字体类型,bkcolor:按钮背景颜色 ,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *
 */
void Button_Show(WM_HWIN hItem, uint8_t aglin, GUI_FONT *font, uint8_t bk_style, uint32_t bkcolor, uint8_t text_style, uint32_t color, uint8_t *buf)
{
    BUTTON_SetFont(hItem, font);
    BUTTON_SetTextAlign(hItem, aglin);
    BUTTON_SetBkColor(hItem, bk_style, bkcolor);
    BUTTON_SetTextColor(hItem, text_style, color);
    BUTTON_SetText(hItem, buf);
}
/** 图片显示
 *
 * WM_WIN:源窗口,aglin:文字对齐方式,font:字体类型,bkcolor:按钮背景颜色 ,color:字体颜色,buf:显示的文字内容
 * @param
 * @return
 *Note:主要针对于二维码
 */
void Image_Show(WM_HWIN hItem, uint8_t imageid, U32 filesize)
{
    const void  *pData;

    //pData = _GetImageById(imageid, &filesize);
    IMAGE_SetBMP(hItem, pData, filesize);
}
/** 删除窗口
 *
 * WM_WIN:源窗口
 * @param
 * @return hItem 窗口句柄
 *
 */
uint8_t _deleteWin(WM_HWIN hItem)
{
    if (bittest(winInitDone, 0))
    {
        bitclr(winInitDone, 0);
    }
    if (bittest(winCreateFlag, 0))
    {
        bitclr(winCreateFlag, 0);
        GUI_EndDialog(err_hItem, 0);
    }
    uxBitsErrTmp = 0;
    err_hItem = 0;
    GUI_EndDialog(hItem, 0);
    hItem = 0;
    //memset(_secDown, '\0', strlen(_secDown));
    return hItem;
}
/** 解析置位数据
 *bit2： 对充电中界面停止充电按钮解锁操作
 *bit3： 显示故障界面的控件
 *bit4： 删除故障界面的控件
 *bit5： 重新校准
 *bit6： 跳转首页选枪界面(仅限应用模式)
 *bit7： 跳转管理员密码界面
 *bit8： 广告页 
 
 * WM_WIN:源窗口
 * @param
 * @return
 *Note:
 */
void CaliDone_Analy(WM_HWIN hWin)//Jump_IsManager(WM_HWIN hWin)
{
    CON_t *pCON;
    pCON =  CONGetHandle(0);
    if (bittest(calebrate_done, 3) == 1)
    {
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
        }
        bitclr(calebrate_done, 3);
        WM_SendMessageNoPara(hWin, MSG_CREATERRWIN);
        //err_window(hWin,uxBitsErr);
    }

    if (bittest(calebrate_done, 4))
    {
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
        }
        bitclr(calebrate_done, 4);
        WM_SendMessageNoPara(hWin, MSG_DELERRWIN);
//        if(bittest(winCreateFlag,0))
//        {
//            bitclr(winCreateFlag,0);
//            WM_SendMessageNoPara(cur_hWin,MSG_DELERRWIN);
//           // WM_SendMessageNoPara(hWin,MSG_DELERRWIN);
//            //WM_DeleteWindow(err_hItem);
//        }
    }

    if (bittest(calebrate_done, 5))
    {
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
        }
        bitclr(calebrate_done, 5);
        _deleteWin(hWin);
        vTaskDelay(100);
        LCD_Init();
        TP_Init();
        vTaskDelay(100);
        LCD_Clear(WHITE);
        TP_Adjust();
        calebrate_done = 1;
        //PutOut_SelAOrB();
        //MainTask();
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
        }
        CreateHomePage();
    }
/// TODO (zshare#1#): ///添加跳转首页会有问题???

//    if(bittest(calebrate_done,6))
//    {
//        bitclr(calebrate_done,6);
//        //WM_DeleteWindow(hWin);
//        _deleteWin(hWin);
//        PutOut_SelAOrB();
//    }

    if (bittest(calebrate_done, 7))
    {
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
        }
        /**< 跳转管理员界面的密码输入页 */
        bitclr(calebrate_done, 7);
        //WM_DeleteWindow(hWin);
        _deleteWin(hWin);
        Keypad_GetValue(LOGIN_PASSWD, " ");
    }
    if (bittest(calebrate_done, 8))
    {
        bitclr(calebrate_done, 8);
        if ((cur_win == _hWinHome) && (pCON->status.xPlugState == UNPLUG))
        {
            if (AdvertisementRecordFlag == 0)
            {
                WM_HideWindow(cur_win);
                WM_ShowWindow(_hWinAdvertizement);
                AdvertisementRecordFlag = 1;
            }
        }
        if (cur_win == _hWinCharging)
        {
            if (AdvertisementRecordFlag == 0)
            {
                WM_HideWindow(cur_win);
                WM_ShowWindow(_hWinAdvertizement);
                AdvertisementRecordFlag = 1;
            }
        }   
          
    }
}
/** @brief 故障弹窗弹出展示
 *
 * @param
 * @param
 * @return
 *
 */
void Err_Analy(WM_HWIN hWin)
{
    CON_t *pCON;
    static EventBits_t uxBitsErr;

    pCON = CONGetHandle(0);

    uxBitsErr = xEventGroupGetBits(pCON->status.xHandleEventCharge);

    if (((uxBitsErr & ERR_SIMBOL) != ERR_SIMBOL))
    {
        if (uxBitsErrTmp != (uxBitsErr & ERR_SIMBOL))
        {
            uxBitsErrTmp = (uxBitsErr & ERR_SIMBOL);
            WM_SendMessageNoPara(hWin, MSG_CREATERRWIN);
        }
    }
    else
    {
//        if(uxBitsErrTmp != (uxBitsErr & ERR_SIMBOL))
 //       {
        uxBitsErrTmp = (uxBitsErr & ERR_SIMBOL);
        WM_SendMessageNoPara(hWin, MSG_CREATERRWIN);
      //      }
    }

    if (!bittest(winCreateFlag, 1)&&bittest(winCreateFlag, 0))
    {
        timer_count++;
        if (timer_count == 10)
        {
            timer_count = 0;
            bitset(calebrate_done, 4);
        }
    }

}
/** @brief
 *  灯光控制
 * @param
 * @param
 * @return
 *
 */
void Led_Show()
{
    CON_t *pCON;
    uint8_t ledSignalPool;
    uint8_t led_signal;
    static uint8_t led_signalold;

    pCON = CONGetHandle(0);

    led_signal = 0;
    /**< 置位说明有故障存在闪烁红灯 */
    if (bittest(winCreateFlag, 1))
    {
        bitset(led_signal, 0);
    }
    else
    {
        switch (pCON->state)
        {
        case STATE_CON_IDLE:
            /**< 空闲状态 */
            bitset(led_signal, 1);
            break;
        case STATE_CON_CHARGING:
            /**< 充电过程中 */
            bitset(led_signal, 2);
            break;
        default:
            if (pCON->status.xPlugState == PLUG)
            {
                if (pCON->status.xCPState == CP_6V_PWM
                    || pCON->status.xCPState == CP_6V)
                {
                    /**< 等待车端插枪 */
                    bitset(led_signal, 3);
                }
                else if (pCON->status.xCPState == CP_9V_PWM
                    || pCON->status.xCPState == CP_9V)
                {
                    /**< S1未闭合 */
                    bitset(led_signal, 4);
                }
            }
            else
            {
                /**< 未知状态 */
                bitset(led_signal, 5);
            }
            break;
        }
    }

    ledSignalPool = led_signalold ^ led_signal;
    if (ledSignalPool != 0)
    {
        ledSignalPool = ledSignalPool & led_signal;
        if (bittest(ledSignalPool, 0))
        {
            led_ctrl(1, red, flicker);
        }
        if (bittest(ledSignalPool, 1))
        {
            led_ctrl(1, green, keep_on);
        }
        if (bittest(ledSignalPool, 2))
        {
            led_ctrl(1, green, breath);
        }
        if (bittest(ledSignalPool, 3))
        {
            led_ctrl(1, green, flicker);
        }
        if (bittest(ledSignalPool, 4))
        {
            led_ctrl(1, blue, flicker);
        }
        if (bittest(ledSignalPool, 5))
        {
            led_ctrl(1, green, keep_on);
        }

        led_signalold = led_signal;
    }
}
/** @brief
 *刷新故障列表;
 * @param msg_err:故障列表指针
 * @param
 * @return
 *
 */

void Errlist_flush(uint8_t *msg_err)
{
    CON_t *pCON;
    EventBits_t uxBitsErr;

    ErrMultiEdit_Size.err_num = 0;

    pCON = CONGetHandle(0);

    uxBitsErr = xEventGroupGetBits(pCON->status.xHandleEventCharge);

    memset(msg_err, '\0', strlen(msg_err));
    strncat(msg_err, cur_err, strlen(cur_err));

    if (((uxBitsErr >> 13) & 0x01) == 0)
    {
        strncat(msg_err, Scram_err, strlen(Scram_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 2) & 0x01) == 0)
    {
        strncat(msg_err, Volt_err, strlen(Volt_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 9) & 0x01) == 0)
    {
        strncat(msg_err, ACTemp_err, strlen(ACTemp_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 14) & 0x01) == 0)
    {
        strncat(msg_err, PE_err, strlen(PE_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 15) & 0x01) == 0)
    {
        strncat(msg_err, Knock_err, strlen(Knock_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 16) & 0x01) == 0)
    {
        strncat(msg_err, Arrester_err, strlen(Arrester_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 17) & 0x01) == 0)
    {
        strncat(msg_err, PowerOff_err, strlen(PowerOff_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 3) & 0x01) == 0)
    {
        strncat(msg_err, Curr_err, strlen(Curr_err));
        ErrMultiEdit_Size.err_num++;
    }
    if (((uxBitsErr >> 4) & 0x01) == 0)
    {
        strncat(msg_err, Freq_err, strlen(Freq_err));
        ErrMultiEdit_Size.err_num++;
    }

    if (strcmp(msg_err, cur_err) == 0)
    {
        /**< 没有故障拼接，拼接"当前无故障"进列表，计算窗口大小 */
        strncat(msg_err, cur_noerr, strlen(cur_noerr));
        ErrMultiEdit_Size.xlength = 300;
        ErrMultiEdit_Size.ylength = 24 * 4;
        ErrMultiEdit_Size.xpos = 480;
        ErrMultiEdit_Size.ypos = (400 - ErrMultiEdit_Size.ylength);
        bitclr(winCreateFlag, 1);
    }
    else
    {
        /**< 有故障拼接,只计算窗口大小 */
        ErrMultiEdit_Size.xlength = 300;
        ErrMultiEdit_Size.ylength = 24*(ErrMultiEdit_Size.err_num + 4);
        ErrMultiEdit_Size.xpos = 480;
        ErrMultiEdit_Size.ypos = (400 - ErrMultiEdit_Size.ylength);
        bitset(winCreateFlag, 1);
    }
//    printf_safe("msg_err = %s\n",msg_err);
}
/** @brief 故障弹窗内容组装
 *
 * @param
 * @param
 * @return
 *
 */
uint8_t err_window(WM_HWIN hWin)//,EventBits_t uxBitsErr)
{
    uint8_t msg_err[150] = { "\0" };
    EventBits_t uxBitsErr;

    //故障界面是否存在
    if (bittest(winCreateFlag, 0))//存在故障界面
    {
        //MULTIEDIT_SetText(err_hItem, msg_err);
        //刷新故障列表
        Errlist_flush(msg_err);
        MULTIEDIT_SetText(err_hItem, msg_err);
//        bitclr(winCreateFlag,0);
//        GUI_EndDialog(err_hItem,0);
    }
    else
    {
        //刷新故障列表
        Errlist_flush(msg_err);
        if (bittest(winCreateFlag, 1))
        {
        //创建故障界面
            err_hItem = MULTIEDIT_CreateEx(ErrMultiEdit_Size.xpos,
                ErrMultiEdit_Size.ypos,
                ErrMultiEdit_Size.xlength,
                ErrMultiEdit_Size.ylength,
                hWin,
                WM_CF_SHOW,
                0,
                GUI_ID_MULTIEDIT0,
                100,
                NULL);
            bitset(winCreateFlag, 0);
            MULTIEDIT_SetInsertMode(err_hItem, 1);  //开启插入模式
            MULTIEDIT_SetFont(err_hItem, &SIF24_Font);
            WM_SetFocus(err_hItem);
            MULTIEDIT_SetInsertMode(err_hItem, 1);
            MULTIEDIT_SetCursorOffset(err_hItem, 0);
            MULTIEDIT_EnableBlink(err_hItem, 0, 0);

            MULTIEDIT_SetText(err_hItem, msg_err);
            MULTIEDIT_SetCursorOffset(err_hItem, 300);

            MULTIEDIT_SetBkColor(err_hItem, MULTIEDIT_CI_EDIT, GUI_RED);//GUI_INVALID_COLOR);

            WM_SetFocus(err_hItem);
            bitclr(calebrate_done, 4);
        }
    }
}





