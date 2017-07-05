#ifndef _TOUCHTIMER_H
#define _TOUCHTIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <includes.h>
#include "xbffontcreate.h"
#include "bmpdisplay.h"
#include "touch.h"
#include "utils.h"
#include "interface.h"
#include "keyboard.h"
#include "qr_encode.h"
#include "lcddrv.h"
#include "user_app.h"
//#include "chargepoint.h"
#define EXIT_DISP_DELAY 60
#define REFLASH 20
#define GUN_NUM 2

#define PAGE_HOME 0
#define PAGE_CARD_VALID 1
#define PAGE_CARD_INFO 2


#define MSG_PAINTErr      (GUI_ID_USER + 0x30)
//#define MSG_MusicStart    (GUI_ID_USER + 0x31)
//#define MSG_NextMusic     (GUI_ID_USER + 0x32)
//��ѹ�쳣��AC�¶��쳣��PE�쳣��ײ���������쳣��ͣ�磻�����쳣��Ƶ���쳣
/*********************�Զ���GUI��Ϣ�ĺ�******************************
**
** GUI_ID_USER + 0x30 - 0x3F �ɶ���16����Ϣ
*/
#define MSG_CREATERRWIN     (GUI_ID_USER + 0x30)    //�������ϵ�����Ϣ
#define MSG_DELERRWIN       (GUI_ID_USER + 0x31)    //ɾ�����ϴ�����Ϣ
#define MSG_JUMPHOME        (GUI_ID_USER + 0x32)    //����HOMEҳ��Ϣ
#define MSG_JUMPCARDINFO    (GUI_ID_USER + 0x33)    //������Ƭ��Ϣҳ��Ϣ
#define MSG_JUMPCHAING      (GUI_ID_USER + 0x34)    //���������ҳ��Ϣ
#define MSG_UPDATEDATA      (GUI_ID_USER + 0x35)    //��������
#define MSG_JUMPCHARGEDONE  (GUI_ID_USER + 0x36)    //��ת������ҳ��Ϣ


extern uint8_t calebrate_done;
extern uint8_t winCreateFlag;
extern WM_HWIN err_hItem;
extern uint8_t strCSQ[10];
extern uint8_t _secDown[10];
extern WM_HWIN _hWinChargDone;
extern WM_HWIN _hWinCharging;
extern WM_HWIN _hWinCardInfo;
extern WM_HWIN _hWinHome;
extern WM_HWIN cur_win;
void _cbHomeDialog(WM_MESSAGE *pMsg);
//��ʱ���Ա���
typedef struct
{
    char year[5];
    char month[5];
    char day[5];
}DateStruct;

struct errMultiEdit_size{
    uint16_t xpos;
    uint16_t ypos;
    uint16_t xlength;
    uint16_t ylength;
    uint8_t err_num;
}ErrMultiEdit_Size;


void PutOut_SelAOrB();
WM_HWIN CreateHome(void);
WM_HWIN CreateRegiterDisp(void);
WM_HWIN CreateCardInfo(void);
WM_HWIN CreateCharging(void);
WM_HWIN CreateChargeDone(void);
WM_HWIN CreateManagerInfoAnalog(void);
//void PutOut_Card_Info();
//void PutOut_Card_Valid();
//void PutOut_Charging();
//void PutOut_Charge_Done();
//void PutOut_Charging_2dimen();
void FrameWin_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3,uint16_t imageBack);
void Caculate_RTC_Show(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1);
void FrameWin_Show(WM_HWIN hItem,uint8_t aglin,uint8_t heigh,GUI_FONT *font,uint32_t color,uint8_t *buf);
void Text_Show(WM_HWIN hItem,GUI_FONT *font,uint32_t color,uint8_t *buf);
void Edit_Show(WM_HWIN hItem,GUI_FONT *font,uint8_t *buf);
void Button_Show(WM_HWIN hItem,uint8_t aglin,GUI_FONT *font,uint8_t bk_style,uint32_t bkcolor,uint8_t text_style,uint32_t color,uint8_t *buf);
void Image_Show(WM_HWIN hItem,uint8_t imageid,U32 filesize);
//void Jump_IsManager(WM_HWIN hWin);
void CaliDone_Analy(WM_HWIN hWin);
void qrencode(uint8_t *qrcode_data,uint16_t *p,uint16_t *x,uint16_t *y);
void display_encode(uint16_t *x,uint16_t *y,uint16_t *p);
uint8_t _deleteWin(WM_HWIN hItem);
void Err_Analy(WM_HWIN hWin);
void Led_Show();
void Errlist_flush(uint8_t *msg_err);
void Signal_Show();
//void PutOut_RegisterDisp();

void Window_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3,uint16_t imageBack);
void ErrWindow_Show(WM_HWIN hWin);
uint8_t err_window(WM_HWIN hWin);
//void PutOut_Manager_InfoAnalog();
uint8_t encodetobmp(uint8_t *filename,uint8_t *codeString);
#endif
