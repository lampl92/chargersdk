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
//公用图片“退出”
extern GUI_BITMAP BitmapQuit;
extern GUI_BITMAP BitmapQuitPress;

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



extern int SignalFlag;//信号图标刷新标志



void MainTask(void);
void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
