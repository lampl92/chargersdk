#ifndef __HMI_START_H
#define __HMI_START_H
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
extern GUI_HMEM    qr_hmem;

#pragma region MyRegion				 
extern int SignalIntensity;
extern int PreSignalIntensity;

extern p_inf *HomeImage;
extern p_inf *SignalImage0;
extern p_inf *SignalImage1;
extern p_inf *SignalImage2;
extern p_inf *SignalImage3;
extern p_inf *SignalImage4;
extern p_inf *SignalImage5;

extern p_inf *CardInfoImage;
extern p_inf *GetCardInfoImage;
extern p_inf *CardUnregisteredImage;
extern p_inf *CardArrearsImage;
extern p_inf *PleaseConnectPlugImage;
extern p_inf *CardInfoVoidImage;

extern p_inf *ChargingImage;
extern p_inf *cartoonImage0;
extern p_inf *cartoonImage1;
extern p_inf *cartoonImage2;
extern p_inf *cartoonImage3;
extern p_inf *cartoonImage4;
extern p_inf *cartoonImage5;
extern p_inf *StopByCardImage;
extern p_inf *StopByQRImage;
extern p_inf *ChargingVoidImage;

extern p_inf *ChargeDoneImage;
extern p_inf *OrderUploadImage;
extern p_inf *NormalDoneImage;
extern p_inf *FullDoneImage;
extern p_inf *DevErrDoneImage;
extern p_inf *MoneyNotEnoughDoneImage;
extern p_inf *ChargeDoneVoidImage;

extern p_inf *AdvertisementImage;

extern p_inf *DtaFileCheckboxChosen;
extern p_inf *DtaFileCheckboxNotChosen;
extern p_inf *DtaFileCheckboxDisable;
extern p_inf *DtaFileBeijing;
extern GUI_BITMAP BitmapCheckboxChosen;
extern GUI_BITMAP BitmapCheckboxNotChosen;
extern GUI_BITMAP BitmapCheckboxDisable;

extern GUI_BITMAP BitmapBeijing;
#pragma endregion


//公用图片“退出”
extern GUI_BITMAP BitmapQuit;
extern GUI_BITMAP BitmapQuitPress;
extern GUI_BITMAP BitmapbackQuit;
extern GUI_BITMAP BitmapbackQuitPress;

//********
//选枪页相关图片
extern GUI_BITMAP BitmapSelectGunBack;
extern GUI_BITMAP BitmapSelectGunAbottonNotpress;
extern GUI_BITMAP BitmapSelectGunBbottonNotpress;
extern GUI_BITMAP BitmapSelectGunAbottonPress;
extern GUI_BITMAP BitmapSelectGunBbottonPress;
//********

//***********
//选择充电模式页图片
extern GUI_BITMAP Bitmapselectpatternback;
extern GUI_BITMAP Bitmapselectpatternelectricnumber;
extern GUI_BITMAP Bitmapselectpatternfull;
extern GUI_BITMAP Bitmapselectpatternmoneynumber;
extern GUI_BITMAP Bitmapselectpatterntime;

extern GUI_BITMAP Bitmapselectpatternelectricnumberpress;
extern GUI_BITMAP Bitmapselectpatternfullpress;
extern GUI_BITMAP Bitmapselectpatternmoneynumberpress;
extern GUI_BITMAP Bitmapselectpatterntimepress;

extern GUI_BITMAP Bitmapselectpatternunityuan;
extern GUI_BITMAP Bitmapselectpatternunitdu;
extern GUI_BITMAP Bitmapselectpatternunitfen;
extern GUI_BITMAP Bitmapselectpatternunitno;

extern GUI_BITMAP BitmapKeyboardback;

extern GUI_BITMAP BitmapKeyboard0;
extern GUI_BITMAP BitmapKeyboard1;
extern GUI_BITMAP BitmapKeyboard2;
extern GUI_BITMAP BitmapKeyboard3;
extern GUI_BITMAP BitmapKeyboard4;
extern GUI_BITMAP BitmapKeyboard5;
extern GUI_BITMAP BitmapKeyboard6;
extern GUI_BITMAP BitmapKeyboard7;
extern GUI_BITMAP BitmapKeyboard8;
extern GUI_BITMAP BitmapKeyboard9;
extern GUI_BITMAP BitmapKeyboardBackspace;
extern GUI_BITMAP BitmapKeyboardesc;
extern GUI_BITMAP BitmapKeyboardok;
extern GUI_BITMAP BitmapKeyboardpoint;

extern GUI_BITMAP BitmapKeyboard0press;
extern GUI_BITMAP BitmapKeyboard1press;
extern GUI_BITMAP BitmapKeyboard2press;
extern GUI_BITMAP BitmapKeyboard3press;
extern GUI_BITMAP BitmapKeyboard4press;
extern GUI_BITMAP BitmapKeyboard5press;
extern GUI_BITMAP BitmapKeyboard6press;
extern GUI_BITMAP BitmapKeyboard7press;
extern GUI_BITMAP BitmapKeyboard8press;
extern GUI_BITMAP BitmapKeyboard9press;
extern GUI_BITMAP BitmapKeyboardBackspacepress;
extern GUI_BITMAP BitmapKeyboardescpress;
extern GUI_BITMAP BitmapKeyboardokpress;
extern GUI_BITMAP BitmapKeyboardpointpress;
//************

//卡信息图片
extern GUI_BITMAP Bitmapcardinfoarrears;
extern GUI_BITMAP Bitmapcardinfoback;
extern GUI_BITMAP Bitmapcardinfoempty;
extern GUI_BITMAP Bitmapcardinfoget;
extern GUI_BITMAP Bitmapcardinfoplug;
extern GUI_BITMAP Bitmapcardinfounregister;


//主页图片
extern GUI_BITMAP Bitmaphomeback;
extern GUI_BITMAP BitmaphomegunAchargedone;
extern GUI_BITMAP BitmaphomegunAcharging;
extern GUI_BITMAP BitmaphomegunAerror;
extern GUI_BITMAP BitmaphomegunAfree;
extern GUI_BITMAP BitmaphomegunBchargedone;
extern GUI_BITMAP BitmaphomegunBcharging;
extern GUI_BITMAP BitmaphomegunBerror;
extern GUI_BITMAP BitmaphomegunBfree;
extern GUI_BITMAP Bitmaphomegunlookinfo;
extern GUI_BITMAP Bitmaphomegunscancode;
extern GUI_BITMAP Bitmaphomegunlookinfopress;
extern GUI_BITMAP Bitmaphomegunscancodepress;
extern GUI_BITMAP Bitmaphomesignal0;
extern GUI_BITMAP Bitmaphomesignal1;
extern GUI_BITMAP Bitmaphomesignal2;
extern GUI_BITMAP Bitmaphomesignal3;

//提示信息页图片
extern GUI_BITMAP Bitmapchargedoneinfo;
extern GUI_BITMAP Bitmapcharginginfo;

//主页储存设备
extern GUI_MEMDEV_Handle Memdevhomeback;
extern GUI_MEMDEV_Handle MemdevhomegunAchargedone;
extern GUI_MEMDEV_Handle MemdevhomegunAcharging;
extern GUI_MEMDEV_Handle MemdevhomegunAerror;
extern GUI_MEMDEV_Handle MemdevhomegunAfree;
extern GUI_MEMDEV_Handle MemdevhomegunBchargedone;
extern GUI_MEMDEV_Handle MemdevhomegunBcharging;
extern GUI_MEMDEV_Handle MemdevhomegunBerror;
extern GUI_MEMDEV_Handle MemdevhomegunBfree;
extern GUI_MEMDEV_Handle Memdevhomegunlookinfo;
extern GUI_MEMDEV_Handle Memdevhomegunscancode;
extern GUI_MEMDEV_Handle Memdevhomegunlookinfopress;
extern GUI_MEMDEV_Handle Memdevhomegunscancodepress;
extern GUI_MEMDEV_Handle Memdevhomesignal0;
extern GUI_MEMDEV_Handle Memdevhomesignal1;
extern GUI_MEMDEV_Handle Memdevhomesignal2;
extern GUI_MEMDEV_Handle Memdevhomesignal3;

extern GUI_MEASDEV_Handle MemdevSelectGunBack;


extern int SignalFlag;//信号图标刷新标志



void MainTask(void);
void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
