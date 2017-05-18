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
//电压异常；AC温度异常；PE异常；撞击；防雷异常；停电；电流异常；频率异常

extern uint8_t calebrate_done;
struct Sys{
    uint8_t charge_gun_num;
}Sys_Info;

struct Wait_timer{
    uint8_t card_valid;//等待卡片无效的时间
    uint8_t card_info;//显示卡片余额和卡号的等待时间
    uint8_t charge_screen_lock;//充电界面锁死倒计时
    uint8_t charge_done_exit;//充电完成界面退出倒计时
}wait_timer;

//临时调试变量
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


//uint8_t bitset(uint32_t var,uint8_t bitno);
//uint8_t bitclr(uint32_t var,uint8_t bitno);
//uint8_t bittest(uint32_t var,uint8_t bitno);
void PutOut_SelAOrB();
void PutOut_Home();
void PutOut_Card_Info();
void PutOut_Card_Valid();
void PutOut_Charging();
void PutOut_Charge_Done();
void PutOut_Charging_2dimen();
void FrameWin_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3);
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
void PutOut_RegisterDisp();
void Window_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3);
void ErrWindow_Show(WM_HWIN hWin);
void err_window(WM_HWIN hWin,EventBits_t uxBitsErr);
#endif
