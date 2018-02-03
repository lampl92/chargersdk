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
extern GUI_HMEM    qr_hmem;

extern int SignalFlag;//�ź�ͼ��ˢ�±�־

void MainTask(void);
void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
