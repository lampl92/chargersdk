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
#include "QR_Encode.h"
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
/*********************自定义GUI消息的宏******************************
**
** GUI_ID_USER + 0x30 - 0x3F 可定义16个消息
*/
#define MSG_CREATERRWIN     (GUI_ID_USER + 0x30)    //创建故障弹窗消息
#define MSG_DELERRWIN       (GUI_ID_USER + 0x31)    //删除故障窗口消息
#define MSG_JUMPHOME        (GUI_ID_USER + 0x32)    //跳到HOME页消息
#define MSG_JUMPCARDINFO    (GUI_ID_USER + 0x33)    //跳到卡片信息页消息
#define MSG_JUMPCHAING      (GUI_ID_USER + 0x34)    //跳到充电中页消息
#define MSG_UPDATEDATA      (GUI_ID_USER + 0x35)    //更新数据
#define MSG_JUMPCHARGEDONE  (GUI_ID_USER + 0x36)    //跳转充电完成页消息
#define MSG_JUMPKEYPAD      (GUI_ID_USER + 0x37)    //跳转键盘页来设置变量信息

extern uint8_t calebrate_done;
extern uint8_t winCreateFlag;
extern uint8_t winInitDone;
extern WM_HWIN err_hItem;
extern uint8_t strCSQ[10];
extern uint8_t _secDown[10];
extern WM_HWIN _hWinChargDone;
extern WM_HWIN _hWinCharging;
extern WM_HWIN _hWinCardInfo;
extern WM_HWIN _hWinHome;
extern WM_HWIN cur_win;
void _cbHomeDialog(WM_MESSAGE *pMsg);
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

struct _Disp_Status{
    uint8_t _workStatus:2;//工作状态
    uint8_t _scramStatus:2;//急停状态
    uint8_t _envTempStatus:2;//环境温度
    uint8_t _AsocketTempStatus:2;//A插座温度
    uint8_t _BsocketTempStatus:2;//B插座温度
    uint8_t _AplugCurrentStatus:2;//A枪输出电流
    uint8_t _BplugCurrentStatus:2;//B枪输出电流
    uint8_t _AplugStatus:2;//A枪枪锁
    uint8_t _BplugStatus:2;//B枪枪锁
    uint8_t _acVoltStatus:2;//交流电压
    uint8_t _acCurrentStatus:2;//交流电流
    uint8_t _spdStatus:2;//防雷器状态
    uint8_t _outputRelayStatus:2;//输出继电器
    uint8_t _controlPilotStatus:2;//控制导引
}disp_Status;

void PutOut_SelAOrB();
WM_HWIN CreateHome(void);
WM_HWIN CreateRegiterDisp(void);
WM_HWIN CreateCardInfo(void);
WM_HWIN CreateCharging(void);
WM_HWIN CreateChargeDone(void);
WM_HWIN CreateManagerInfoAnalog(void);
WM_HWIN CreateManagerInfoStatus(void);
WM_HWIN CreateManagerAlarmLog(void);
WM_HWIN CreateManagerSysSet(void);
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
uint8_t encodetobmp(char *filename,uint8_t *codeString);
#endif
