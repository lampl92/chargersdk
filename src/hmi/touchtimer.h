#ifndef _TOUCHTIMER_H
#define _TOUCHTIMER_H

#include <stdio.h>
#include <stdlib.h>
#include <includes.h>
#include "siffontcreate.h"
#include "bmpdisplay.h"
#include "touch.h"
#include "utils.h"
#include "interface.h"
#include "keyboard.h"
#include "lcddrv.h"
#include "user_app.h"
#include "stringName.h"
#include "cfg_parse.h"
//#include "chargepoint.h"

#define PAGE_HOME 0
#define PAGE_CARD_VALID 1
#define PAGE_CARD_INFO 2


#define MSG_PAINTErr      (GUI_ID_USER + 0x30)
//#define MSG_MusicStart    (GUI_ID_USER + 0x31)
//#define MSG_NextMusic     (GUI_ID_USER + 0x32)
//电压异常；AC温度异常；PE异常；撞击；防雷异常；停电；电流异常；频率异常
#define GUI_MANAGER_XLEFT 30    //管理员界面左上角X位置
#define GUI_MANAGER_YLEFT 20    //管理员界面左上角Y位置

#define GUI_MANAGER_YOFF 40     //管理员界面相关配置中每行的宽度
#define GUI_MANAGER_YSIZE 35    //管理员界面相关配置中每行的edit的宽度
#define HSCROLL_WIDTH   30  //水平滑轮宽度
#define WSCROLL_WIDTH   30  //垂直滑轮宽度

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
#define MSG_DELETEMANAGERWIN (GUI_ID_USER + 0x38)   //管理员的common通知所有page删除自己的win

#define MSG_JUMPSELECTPATTERN (GUI_ID_USER + 0x39)  //调到选择充电方式
#define MSG_JUMPSELECTGUN   (GUI_ID_USER +0x3a)     //跳到选枪页


#define MSG_MANAGERSETID0       (GUI_ID_USER + 0x40)
#define MSG_MANAGERSETID1       (GUI_ID_USER + 0x41)
#define MSG_MANAGERSETID2       (GUI_ID_USER + 0x42)
#define MSG_MANAGERSETID3       (GUI_ID_USER + 0x43)
#define MSG_MANAGERSETID4       (GUI_ID_USER + 0x44)
#define MSG_MANAGERSETID5       (GUI_ID_USER + 0x45)
#define MSG_MANAGERSETID6       (GUI_ID_USER + 0x46)
#define MSG_MANAGERSETID7       (GUI_ID_USER + 0x47)
#define MSG_MANAGERSETID8       (GUI_ID_USER + 0x48)
#define MSG_MANAGERSETID9       (GUI_ID_USER + 0x49)
#define MSG_MANAGERSETIDA       (GUI_ID_USER + 0x4A)
#define MSG_MANAGERSETIDB       (GUI_ID_USER + 0x4B)
#define MSG_MANAGERSETIDC       (GUI_ID_USER + 0x4C)
#define MSG_MANAGERSETIDD       (GUI_ID_USER + 0x4D)
#define MSG_MANAGERSETIDE       (GUI_ID_USER + 0x4E)
#define MSG_MANAGERSETIDF       (GUI_ID_USER + 0x4F)

extern uint16_t calebrate_done;
extern uint8_t winCreateFlag;
extern uint8_t winInitDone;
extern WM_HWIN err_hItem;
extern uint8_t strCSQ[32];
extern uint8_t _secDown[10];
extern WM_HWIN _hWinChargDone;
extern WM_HWIN _hWinCharging;
extern WM_HWIN _hWinCardInfo;
extern WM_HWIN _hWinHome;
extern WM_HWIN cur_win;
extern WM_HWIN _hWinAdvertizement;
void _cbHomeDialog(WM_MESSAGE *pMsg);
extern uint8_t current_page;
extern uint8_t Timer_buf[32];
extern uint8_t AdvertisementRecordFlag;
extern uint8_t managerLevel;
extern GUI_HMEM    qr_hmem;
extern GUI_QR_INFO QR_info;
extern WM_HWIN _hWinManagerCommon;
extern WM_HWIN _hWinManagerInfoAnalog;
extern WM_HWIN _hWinManagerInfoStatus;
extern WM_HWIN _hWinManagerLogDate;
extern WM_HWIN _hWinManagerConSet;
extern WM_HWIN _hWinManagerSysSet;
extern WM_HWIN _hWinManagerSysInfo;
extern WM_HWIN _hWinManagerTerminate;

GUI_QR_INFO QR_info;

enum{
    _HOMEPAGE,
    _CARDINFOPAGE,
    _CHARGINGPAGE,
    _CHARGEDONEPAGE,
};

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

//**********单双枪新界面
WM_HWIN CreateHomeDLG(void);
WM_HWIN CreateselectgunDLG(void);
WM_HWIN CreateselectpatternDLG(void);
WM_HWIN CreateCardInfoDLG(void);
//*************






void PutOut_SelAOrB();



WM_HWIN CreateHomePage(void);
WM_HWIN CreateCardInfoPage(void);
WM_HWIN CreateChargingPage(void);
WM_HWIN CreateChargeDonePage(void);
WM_HWIN CreateAdvertisementPage(void);
WM_HWIN CreateAdvertisement(void);
WM_HWIN CreateHome(void);
WM_HWIN CreateRegiterDisp(void);
WM_HWIN CreateCardInfo(void);
WM_HWIN CreateCharging(void);
WM_HWIN CreateChargeDone(void);
WM_HWIN CreateManagerInfoAnalog(WM_HWIN srcHwin);
WM_HWIN CreateManagerInfoStatus(WM_HWIN srcHwin);
WM_HWIN CreateManagerLogDate(WM_HWIN srcHwin);
//WM_HWIN CreateManagerAlarmLog();
WM_HWIN CreateManagerSysSet(WM_HWIN srcHwin);
WM_HWIN CreateManagerConSet(WM_HWIN srcHwin);
WM_HWIN CreateManagerSysInfo(WM_HWIN srcHwin);
WM_HWIN CreateManagerTerminate(WM_HWIN srcHwin);
WM_HWIN CreateManagerCommon(void);
void FrameWin_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3,uint16_t imageBack);
void Caculate_RTC_Show(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1);
void FrameWin_Show(WM_HWIN hItem,uint8_t aglin,uint8_t heigh,GUI_FONT *font,uint32_t color,uint8_t *buf);
void Text_Show(WM_HWIN hItem,GUI_FONT *font,uint32_t color,uint8_t *buf);
void Edit_Show(WM_HWIN hItem,GUI_FONT *font,uint8_t *buf);
void Button_Show(WM_HWIN hItem,uint8_t aglin,GUI_FONT *font,uint8_t bk_style,uint32_t bkcolor,uint8_t text_style,uint32_t color,uint8_t *buf);
void Image_Show(WM_HWIN hItem,uint8_t imageid,U32 filesize);
void CaliDone_Analy(WM_HWIN hWin);
uint8_t _deleteWin(WM_HWIN hItem);
void Err_Analy(WM_HWIN hWin);
void Led_Show();
void Errlist_flush(uint8_t *msg_err);
void Signal_Show();
int getSignalIntensity();

void Window_Init(WM_MESSAGE *pMsg,uint16_t textid0,uint16_t textid1,uint16_t textid2,uint16_t textid3,uint16_t imageBack);
void ErrWindow_Show(WM_HWIN hWin);
uint8_t err_window(WM_HWIN hWin);
#endif
