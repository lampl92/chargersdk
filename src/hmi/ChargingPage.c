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
int cartoonflag = 0;//����������ʾ��־

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "ChargingPage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "ChargingImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { TEXT_CreateIndirect, "Date", ID_TEXT_0, 4, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Time", ID_TEXT_1, 136, 4, 120, 24, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "CartoonImage", ID_IMAGE_2, 60, 60, 200, 80, 0, 0, 0 },
    { IMAGE_CreateIndirect, "PromptImage", ID_IMAGE_3, 40, 340, 300, 80, 0, 0, 0 },
    { TEXT_CreateIndirect, "Hour", ID_TEXT_2, 30, 200, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "min", ID_TEXT_3, 100, 200, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "sec", ID_TEXT_4, 170, 200, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "ChargingCurrent", ID_TEXT_5, 535, 70, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "ChargingPower", ID_TEXT_6, 535, 120, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalPower", ID_TEXT_7, 535, 170, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalServFee", ID_TEXT_8, 535, 220, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalPowerFee", ID_TEXT_9, 535, 270, 48, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "TotalFee", ID_TEXT_10, 535, 320, 48, 24, 0, 0x0, 0 },
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

    pCON = CONGetHandle(0);///** @todo (zshare#1#): ˫ǹIDѡ�� */
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
    sprintf(temp_buf, "%2.1f", pCON->status.dChargingCurrent);//������   
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_5), temp_buf);
//    pCON->status.GetChargingPower(pCON);
    sprintf(temp_buf, "%d", (int)pCON->status.dChargingPower);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_6), temp_buf);//��繦��
    sprintf(temp_buf, "%3.1f", pCON->order.dTotalPower);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_7), temp_buf);//�������
    sprintf(temp_buf, "%3.1f", pCON->order.dTotalServFee);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_8), temp_buf);//��ǰ�����
    sprintf(temp_buf, "%3.1f", pCON->order.dTotalPowerFee);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_9), temp_buf);//��ǰ���
    sprintf(temp_buf, "%3.1f", pCON->order.dTotalFee);
    TEXT_SetText(WM_GetDialogItem(hWin, ID_TEXT_10), temp_buf);//�����ܶ�
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
        /**< ���������� */
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
      
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_2), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_3), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_4), &SIF24_Font, GUI_BLUE," ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_5), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_6), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_7), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_8), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_9), &SIF24_Font, GUI_BLUE, " ");
        Text_Show(WM_GetDialogItem(pMsg->hWin, ID_TEXT_10), &SIF24_Font, GUI_BLUE, " ");
        break;
    case WM_PAINT://MSG_UPDATEDATA:
    /// TODO (zshare#1#): �����if��������.\����if��Ƕ�׵�if������,Ŀǰ���ô�����ܲ������õ�if
        if ((bittest(winInitDone, 0))&&(_hWinCharging == cur_win))
        {
            /**< ���ݴ��� */
            Data_Process(pMsg);
            /**< �ź����ݴ��� */
            Signal_Show();
            /**< �ƹ���� */
            Led_Show();
            /**< ������淢�����л� */
            if (_hWinCharging == cur_win)
            {
                /**< ���Ϸ��� */
                Err_Analy(pMsg->hWin);
                /**< ���ⴥ�ص���� */
                CaliDone_Analy(pMsg->hWin);
            }
           // Data_Flush(pMsg);
//            Caculate_RTC_Show(pMsg, ID_TEXT_1, ID_TEXT_2);
        }
        break;
    case WM_TIMER:
        if (pMsg->Data.v == _timerRTC)
        {
            /**< ��ʾʱ������� */
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
            /**< ������ʱ�� */
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
            Data_Flush(pMsg);
            WM_RestartTimer(pMsg->Data.v, 500);
        }
        break;
    case MSG_CREATERRWIN:
        /**< ���Ͻ��治�����򴴽�,������ˢ�¸澯 */
        err_window(pMsg->hWin);
        break;
    case MSG_DELERRWIN:
        /**< ���Ͻ��������ɾ�����Ͻ��� */
        if (bittest(winCreateFlag, 0))
        {
            bitclr(winCreateFlag, 0);
            //WM_DeleteWindow(err_hItem);
            GUI_EndDialog(err_hItem, 0);
            err_hItem = 0;
        }
        break;
    case MSG_JUMPCHARGEDONE:
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
    _timerText = WM_CreateTimer(WM_GetClientWindow(_hWinCharging), ID_TimerText, 500, 0);
    bitset(winInitDone, 0);
    return 0;
}
