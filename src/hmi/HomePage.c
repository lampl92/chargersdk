#include "HMI_Start.h"
#include "touchtimer.h"
#include "bmpdisplay.h"
#include "touchtimer.h"
#include "DIALOG.h"

#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_IMAGE_0 (GUI_ID_USER + 0x01)
#define ID_IMAGE_1 (GUI_ID_USER + 0x02)
#define ID_TEXT_0 (GUI_ID_USER + 0x05)
#define ID_TEXT_1 (GUI_ID_USER + 0x06)
#define ID_TEXT_2 (GUI_ID_USER + 0x07)
#define ID_TEXT_3 (GUI_ID_USER + 0x08)

#define ID_IMAGE_0_IMAGE_0 0x00
#define ID_IMAGE_1_IMAGE_0 0x01

#define ID_TimerTime    0

#define PROGBAR_X   200
#define PROGBAR_Y   50
#define PROGBAR_XSIZE   400
#define PROGBAR_YSIZE   100
#define TEXT_X      PROGBAR_X      
#define TEXT_Y      (PROGBAR_Y+PROGBAR_YSIZE)
#define TEXT_XSIZE  400
#define TEXT_YSIZE  400

//刷卡已经不再用这些事件, 为保证编译临时定义, 整改界面流程后删掉

#define defEventBitGotIDtoHMI           BIT_1               //获取到ID，发送到HMI

WM_HWIN _hWinHome;//home界面句柄

static WM_HWIN hwinQR;//二维码句柄
static PROGBAR_Handle progbar_up;
static int progbar_value = 0;
static uint8_t textLogDone = 0;
PROGBAR_SKINFLEX_PROPS progbar_props;
uint32_t props_r, props_g, props_b;

static TEXT_Handle text_up;

int SignalFlag = 0;//信号图标刷新标志

//home界面资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "HomePage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "HomeImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { TEXT_CreateIndirect, "PowerFee", ID_TEXT_0, 96, 168, 96, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "ServiceFee", ID_TEXT_1, 96, 278, 96, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Date", ID_TEXT_2, 4, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Time", ID_TEXT_3, 136, 4, 120, 24, 0, 0x0, 0 },
};

//发送刷卡或扫码换页信号
static void Data_Process(WM_MESSAGE *pMsg)
{
    CON_t *pCON;
    EventBits_t uxBitRFID;
    
    WM_HWIN hWin = pMsg->hWin;
    pCON = CONGetHandle(0);
    
    /*如果刷卡，发送跳页消息*/
    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
                                defEventBitGotIDtoHMI,  
                                pdTRUE, pdTRUE, 0);    
    if((uxBitRFID & defEventBitGotIDtoHMI) == defEventBitGotIDtoHMI)
    {
        WM_SendMessageNoPara(hWin, MSG_JUMPCARDINFO);
    }
    
    /*如果扫码，发送跳页消息*/
//    if ((pCON->order.ucStartType == 5)
//        &&(pCON->state == STATE_CON_CHARGING))
//    {
//        WM_SendMessageNoPara(hWin, MSG_JUMPCHAING);
//    }

    if (pCON->state == STATE_CON_CHARGING)
    {
        WM_SendMessageNoPara(hWin, MSG_JUMPCHAING);
    }   
}

//二维码回调函数
static void _cbWindowQR(WM_MESSAGE *pMsg)
{
    WM_HWIN hWin;

    hWin = pMsg->hWin;
    switch (pMsg->MsgId) {
    case WM_PAINT:
        GUI_QR_Draw(qr_hmem, 0, 0);
        break;
    default:
        WM_DefaultProc(pMsg);   
        break;
    }
}

static void _cbDialog(WM_MESSAGE * pMsg) {
    
    WM_HWIN      hItem;
    uint8_t pos = 0;
    uint8_t ucSegState;
    time_t  now;
    uint8_t strPowerFee[10];
    uint8_t strServiceFee[10];
    uint8_t pDest[256];
    uint8_t buff[50];
    EventBits_t uxbits;
    
    switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_RED, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_RED, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_RED, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_RED, " ");
        
        hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
        IMAGE_SetBMP(hItem, HomeImage->pfilestring, HomeImage->pfilesize);
        
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
        
        hwinQR = WM_CreateWindowAsChild((800 - QR_info.Size)/2 - 10, (480 - QR_info.Size)/2, QR_info.Size, QR_info.Size, pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, _cbWindowQR, 0);            
//        pechProto->info.ftp.ucDownloadStart ? 0,1=shengji
//        text_up = TEXT_CreateEx(TEXT_X, TEXT_Y, TEXT_XSIZE, TEXT_YSIZE, _hWinHome, WM_CF_SHOW, 0, 0, "");
//        TEXT_SetFont(text_up, &SIF16_Font);
//        TEXT_SetTextColor(text_up, GUI_BLACK);            
//        TEXT_SetText(text_up, "测试显示");
        if (pechProto->info.ftp.ucDownloadStart == 1)
        {
            progbar_up = PROGBAR_CreateEx(PROGBAR_X, PROGBAR_Y, PROGBAR_XSIZE, PROGBAR_YSIZE, _hWinHome, WM_CF_SHOW, PROGBAR_CF_HORIZONTAL, 0);
            text_up = TEXT_CreateEx(TEXT_X, TEXT_Y, TEXT_XSIZE, TEXT_YSIZE, _hWinHome, WM_CF_SHOW, 0, 0, "");
            
            PROGBAR_SetMinMax(progbar_up, 0, 100);
            PROGBAR_SetFont(progbar_up, &SIF24_Font);

            progbar_props.aColorUpperL[0] = GUI_BLUE;
            progbar_props.aColorUpperL[1] = GUI_BLUE;
            progbar_props.aColorLowerL[0] = GUI_BLUE;
            progbar_props.aColorLowerL[1] = GUI_BLUE;
            progbar_props.aColorUpperR[0] = GUI_GRAY;
            progbar_props.aColorUpperR[1] = GUI_GRAY;
            progbar_props.aColorLowerR[0] = GUI_GRAY;
            progbar_props.aColorLowerR[1] = GUI_GRAY;
            progbar_props.ColorFrame = GUI_BLACK;
        
            PROGBAR_SetSkinFlexProps(&progbar_props, 0);
            //        PROGBAR_SetSkin(progbar_up, PROGBAR_SKIN_FLEX);
            //        PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
            //        PROGBAR_SetText(progbar_up, "(1/5)");
        
            TEXT_SetFont(text_up, &SIF16_Font);
            TEXT_SetTextColor(text_up, GUI_BLACK);            
            TEXT_SetText(text_up, "打开FTP...");
            TEXT_SetBkColor(text_up, GUI_GRAY);
        }
        
        break;
    case WM_PAINT:
        break;
    case WM_TIMER:
        if ((bittest(winInitDone, 0))&&(_hWinHome == cur_win))
        {
            if (pModem->state == DS_MODEM_FTP_GET)
            {
                if (((pechProto->info.ftp.ftp_proc.ulRecvFileSize * 100 / (768 * 1024))  < progbar_value)
                    &&(textLogDone == 0))
                {
                    textLogDone = 1;
                }
        
                if (textLogDone == 1)
                {
                    memset(buff, '\0', sizeof(buff));
            
                    sprintf(buff, "\n下载失败!\n第(%d/5)次尝试下载文件...", pechProto->info.ftp.ftp_proc.ulFTPReGetCnt + 1);
                    memset(pDest, '\0', sizeof(pDest));
                    TEXT_GetText(text_up, pDest, 256);
                    strcat(pDest, buff);
                    TEXT_SetText(text_up, pDest);
                    textLogDone = 0;
                }
       /// TODO (zshare#1#): 下面的if不起作用. 但是if里嵌套的if起作用,目前先用此来规避不起作用的if
   //        
   //              5   xiazai
   //             pechProto->info.ftp.ftp_proc.ulFTPReOpenCnt 5  dakai ftp
   //        if (pModem->status == DS_MODEM_FTP_OPEN)
   //        {
   //                        
   //        }
   //        if (pechProto->info.ftp.ftp_proc.ulRecvFileSize > 0)
   //        {
   //            pModem->state = DS_MODEM_FTP_OPEN //dakaizhong...
   //                DS_MODEM_FTP_GET //xiazaizhong
   //                DS_MODEM_FTP_REGET//jindutiao = 0// chongxinxiazai 
   //                
   //                memset(pDest,'\0',sizeof(pDest));
   //            TEXT_GetText(text_up, pDest, 256);
   //            strcat(pDest, "打开成功！\n正在下载...\n");
   //            TEXT_SetText(text_up,pDest);
   //        }
           //pechProto->info.ftp.ftp_proc.ulRecvFileSize = filesize  768 = 100 % filesize * 100 / 768
                progbar_value = pechProto->info.ftp.ftp_proc.ulRecvFileSize * 100 / (768 * 1024);
                PROGBAR_SetValue(progbar_up, progbar_value);
            }
            
            uxbits = xEventGroupWaitBits(xHandleEventHMI, defEventBitHMI_UP_FAILD,pdTRUE,pdTRUE,0);
            if((uxbits & defEventBitHMI_UP_FAILD) == defEventBitHMI_UP_FAILD)
            {
                GUI_EndDialog(progbar_up, 0);
                GUI_EndDialog(text_up, 0);
            }
            
            Data_Process(pMsg);
            Signal_Show();
            Led_Show();
            if (_hWinHome == cur_win)
            {
                Err_Analy(pMsg->hWin);
                CaliDone_Analy(pMsg->hWin);
            }    
        }
    /**< 显示时间和日期 */
        Caculate_RTC_Show(pMsg, ID_TEXT_2, ID_TEXT_3);
        //费用显示

        now = time(NULL);
        extern OrderSegState_e JudgeSegState(time_t now, echProtocol_t *pProto, uint8_t *ppos);
        ucSegState = (uint8_t)JudgeSegState(now, pechProto, &pos);
        sprintf(strPowerFee, "%.2lf", pechProto->info.dSegPowerFee[ucSegState]);
        sprintf(strServiceFee, "%.2lf", pechProto->info.dSegServFee[ucSegState]);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), strPowerFee);/**< 充电费*/
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), strServiceFee);/**< 服务费 */   

        if ((SignalFlag == 12) || (SignalFlag > 12))
        {
            SignalIntensity = getSignalIntensity();
            if (SignalIntensity != PreSignalIntensity)
            {
                hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_1);
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
        }
        SignalFlag++;
        /**< 重启定时器 */
        WM_RestartTimer(pMsg->Data.v, 50);
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
        }
        break;
    case MSG_JUMPCARDINFO:
        if (AdvertisementRecordFlag == 1)
        {
            WM_HideWindow(_hWinAdvertizement);
            WM_ShowWindow(cur_win);
            AdvertisementRecordFlag = 0;
            GUI_Exec();
        }
        current_page = _CARDINFOPAGE;
        bitset(winInitDone, 7);
        GUI_EndDialog(hwinQR, 0);
        _deleteWin(_hWinHome);
        _hWinHome = 0;
        CreateCardInfoPage();
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
        GUI_EndDialog(hwinQR, 0);
        _deleteWin(_hWinHome);
        _hWinHome = 0;
        CreateChargingPage();
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreateHomePage(void);
WM_HWIN CreateHomePage(void) {
    _hWinHome = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    WM_CreateTimer(WM_GetClientWindow(_hWinHome), ID_TimerTime, 50, 0);
    cur_win = _hWinHome;
    bitset(winInitDone, 0);
    return 0;
}

