#ifndef _TOUCHTIMER_H
#define _TOUCHTIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <includes.h>
#include "xbffontcreate.h"
#include "bmpdisplay.h"

//#include "chargepoint.h"

#define REFLASH 20
#define GUN_NUM 2

#define PAGE_HOME 0
#define PAGE_CARD_VALID 1
#define PAGE_CARD_INFO 2
//#define PAGE_
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
////uint8_t current_id_disp = 0;

//typedef struct
//{
//    uint32_t id;
//    uint8_t status;
//    uint16_t balance;
//    uint8_t charge_gun_status;//0���ã�1����ʹ����
//}All_Card_Info;
//
//typedef struct
//{
//    uint8_t point_statue;
//
//}ALL_Point_Info;
//
//All_Card_Info Card_Info[GUN_NUM];
//ALL_Point_Info Point_Info[GUN_NUM];

struct Sys{
    uint8_t charge_gun_num;
}Sys_Info;

struct Wait_timer{
    uint8_t card_valid;//�ȴ���Ƭ��Ч��ʱ��
    uint8_t card_info;//��ʾ��Ƭ���Ϳ��ŵĵȴ�ʱ��
    uint8_t charge_screen_lock;//��������������ʱ
    uint8_t charge_done_exit;//�����ɽ����˳�����ʱ
}wait_timer;



//��ʱ���Ա���
typedef struct
{
    char year[5];
    char month[5];
    char day[5];
}DateStruct;


void PutOut_Home();
void FrameWin_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3);
void Caculate_RTC_Show(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1);
void FrameWin_Show(WM_HWIN hItem,uint8_t aglin,uint8_t heigh,GUI_FONT *font,uint32_t color,uint8_t *buf);
void Text_Show(WM_HWIN hItem,GUI_FONT *font,uint32_t color,uint8_t *buf);
void Edit_Show(WM_HWIN hItem,GUI_FONT *font,uint8_t *buf);
void Button_Show(WM_HWIN hItem,uint8_t aglin,GUI_FONT *font,uint8_t bk_style,uint32_t bkcolor,uint8_t text_style,uint32_t color,uint8_t *buf);
void Image_Show(WM_HWIN hItem,uint8_t imageid,U32 filesize);

#endif
