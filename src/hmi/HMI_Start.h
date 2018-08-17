#ifndef __HMI_START_H
#define __HMI_START_H
#include <stdio.h>
#include <stdlib.h>
#include <includes.h>
#include "bsp.h"
#include "siffontcreate.h"
#include "display.h"
#include "touch.h"
#include "utils.h"
#include "interface.h"
#include "keyboard.h"
#include "lcddrv.h"
#include "user_app.h"
#include "skin.h"
#include "GUI_backstage.h"
#include "skinposition.h"

typedef WM_HWIN(*Fun)(void);
extern Fun home;

extern GUI_HMEM    qr_hmem;
extern uint16_t flag_specially;

extern int SignalFlag;//信号图标刷新标志

#define FONT_COLOR GUI_BLACK
#define color_manager_in 0x0000CC
#define color_manager_out GUI_BLACK

#define FontManager SIF24_Font;

//**********单双枪新界面
WM_HWIN CreatestartUpDLG(void);
WM_HWIN CreateHome0DLG(void);
WM_HWIN CreateHomeDLG(void);
WM_HWIN CreateSuperManager(void);
WM_HWIN CreateselectgunDLG(void);
WM_HWIN CreateselectpatternDLG(void);
WM_HWIN CreateselectpatternbetterDLG(void);
WM_HWIN CreateCardInfoDLG(void);
WM_HWIN CreatechargedoneinfoDLG(void);
WM_HWIN CreatecharginginfoDLG(void);
WM_HWIN CreatereadystartDLG(void);
WM_HWIN CreatePwdTest(void);
WM_HWIN CreatePwdFull(void);
WM_HWIN CreatePwdErrorAgain(void);
WM_HWIN CreatenettimeoutDLG(void);
WM_HWIN CreatecardconditionnotokDLG(void);
WM_HWIN CreatepleaseplugDLG(void);
WM_HWIN CreatePwdInput(void);
WM_HWIN CreatechargingokDLG(void);
WM_HWIN CreateplugtimeoutDLG(void);
WM_HWIN CreateEquipmentFailureNoStartDLG(void);
//*************


void MainTask(void);
void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
