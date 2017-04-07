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
//    uint8_t charge_gun_status;//0可用；1正在使用中
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

#endif
