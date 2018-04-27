#ifndef __HMI_START_H
#define __HMI_START_H
#include <stdio.h>
#include <stdlib.h>
#include <includes.h>
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
extern GUI_HMEM    qr_hmem;

extern int SignalFlag;//信号图标刷新标志

extern Fun home;

#define FONT_COLOR GUI_BLACK

//**********单双枪新界面
WM_HWIN CreatestartUpDLG(void);
WM_HWIN CreateHome0DLG(void);
WM_HWIN CreateHomeDLG(void);
WM_HWIN CreateselectgunDLG(void);
WM_HWIN CreateselectpatternDLG(void);
WM_HWIN CreateselectpatternbetterDLG(void);
WM_HWIN CreateCardInfoDLG(void);
WM_HWIN CreatechargedoneinfoDLG(void);
WM_HWIN CreatecharginginfoDLG(void);
WM_HWIN CreatereadystartDLG(void);
WM_HWIN CreatenettimeoutDLG(void);
WM_HWIN CreatecardconditionnotokDLG(void);
WM_HWIN CreatepleaseplugDLG(void);
WM_HWIN CreatechargingokDLG(void);
WM_HWIN CreateplugtimeoutDLG(void);
//*************


void MainTask(void);
void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
