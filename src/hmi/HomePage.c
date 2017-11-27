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

WM_HWIN _hWinHome;//home������

static WM_HWIN hwinQR;//��ά����

int SignalFlag = 0;//�ź�ͼ��ˢ�±�־

static float preServiceFee = 0;
static float prePowerFee = 0;

//home������Դ��
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
    { WINDOW_CreateIndirect, "HomePage", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
    { IMAGE_CreateIndirect, "HomeImage", ID_IMAGE_0, 0, 0, 800, 480, 0, 0, 0 },
    { IMAGE_CreateIndirect, "SignalImage", ID_IMAGE_1, 755, 3, 40, 29, 0, 0, 0 },
    { TEXT_CreateIndirect, "PowerFee", ID_TEXT_0, 96, 168, 96, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "ServiceFee", ID_TEXT_1, 96, 278, 96, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Date", ID_TEXT_2, 4, 4, 120, 24, 0, 0x0, 0 },
    { TEXT_CreateIndirect, "Time", ID_TEXT_3, 136, 4, 120, 24, 24, 0, 0x0, 0 },
};

//����ˢ����ɨ�뻻ҳ�ź�
static void Data_Process(WM_MESSAGE *pMsg)
{
    CON_t *pCON;
    EventBits_t uxBitRFID;
    WM_HWIN hWin = pMsg->hWin;
    pCON = CONGetHandle(0);
    SignalIntensity = getSignalIntensity();
    /*���ˢ����������ҳ��Ϣ*/
    uxBitRFID = xEventGroupWaitBits(pRFIDDev->xHandleEventGroupRFID,
        defEventBitGotIDtoHMI,
        pdTRUE,
        pdTRUE,
        0);
    if (((uxBitRFID & defEventBitGotIDtoHMI) == defEventBitGotIDtoHMI) && (SignalIntensity > 0))
    {
        WM_SendMessageNoPara(hWin, MSG_JUMPCARDINFO);
    }
    
    /*���ɨ�룬������ҳ��Ϣ*/
    if ((pCON->order.ucStartType == 5)
        &&(pCON->state == STATE_CON_CHARGING))
    {
        WM_SendMessageNoPara(hWin, MSG_JUMPCHAING);
    }

    if (pCON->state == STATE_CON_CHARGING)
    {
        WM_SendMessageNoPara(hWin, MSG_JUMPCHAING);
    }   
}

//��ά��ص�����
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
    
    uint8_t strPowerFee[10];
    uint8_t strServiceFee[10];
    
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
        
        hwinQR = WM_CreateWindowAsChild(299, 156, 200, 200, pMsg->hWin, WM_CF_SHOW | WM_CF_HASTRANS, _cbWindowQR, 0);            
        break;
    case WM_PAINT:
     /// TODO (zshare#1#): �����if��������. ����if��Ƕ�׵�if������,Ŀǰ���ô�����ܲ������õ�if
        if ((bittest(winInitDone, 0))&&(_hWinHome == cur_win))
        {
            Data_Process(pMsg);
            Signal_Show();
            Led_Show();
            if (_hWinHome == cur_win)
            {
                Err_Analy(pMsg->hWin);
                CaliDone_Analy(pMsg->hWin);
            }
        }
        break;
    case WM_TIMER:
    /**< ��ʾʱ������� */
        Caculate_RTC_Show(pMsg, ID_TEXT_2, ID_TEXT_3);
        //������ʾ
        if (((pEVSE->info.dDefSegFee - prePowerFee) > 0.01) || ((pEVSE->info.dServiceFee - preServiceFee) > 0.01))
        {
            prePowerFee = pEVSE->info.dDefSegFee;
            preServiceFee = pEVSE->info.dServiceFee;
            sprintf(strPowerFee, "%.2lf", pEVSE->info.dDefSegFee);
            sprintf(strServiceFee, "%.2lf", pEVSE->info.dServiceFee);
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_0), strPowerFee);/**< ����*/
            TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_1), strServiceFee);/**< ����� */      
        }
        if ((SignalFlag == 4) || (SignalFlag > 4))
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
            PreSignalIntensity = SignalIntensity;  
            SignalFlag = 0;
        }
        SignalFlag++;
        /**< ������ʱ�� */
        WM_RestartTimer(pMsg->Data.v, 300);
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
            GUI_EndDialog(err_hItem, 0);
        }
        break;
    case MSG_JUMPCARDINFO:
        GUI_EndDialog(hwinQR, 0);
        _deleteWin(_hWinHome);
        CreateCardInfoPage();
        current_page = _CARDINFOPAGE;
        bitset(winInitDone, 7);
        break;
    case MSG_JUMPCHAING:
        _deleteWin(_hWinHome);
        CreateChargingPage();
        current_page = _CHARGINGPAGE;
        bitset(winInitDone, 7);
        break;
    default:
        WM_DefaultProc(pMsg);
        break;
    }
}

WM_HWIN CreateHomePage(void);
WM_HWIN CreateHomePage(void) {
    _hWinHome = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
    WM_CreateTimer(WM_GetClientWindow(_hWinHome), ID_TimerTime, 250, 0);
    cur_win = _hWinHome;
    bitset(winInitDone, 0);
    return 0;
}

