#ifndef _TOUCHTIMER_H
#define _TOUCHTIMER_H

#include <stdio.h>
#include <stdlib.h>
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

#endif
