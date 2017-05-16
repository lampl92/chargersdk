/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2016  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.36 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  ARM LIMITED whose registered office
is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
distributed as part of the  MDK-ARM  Professional  under the terms and
conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
Professional.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information

Licensor:                 SEGGER Software GmbH
Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
Licensed SEGGER software: emWin
License number:           GUI-00181
License model:            LES-SLA-20007, Agreement, effective since October 1st 2011
Licensed product:         MDK-ARM Professional
Licensed platform:        ARM7/9, Cortex-M/R4
Licensed number of seats: -
----------------------------------------------------------------------
File        : GUI_TOUCH_X.C
Purpose     : Config / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include "bsp.h"
#include "lcddrv.h"
#include "touch.h"

#define Y (1)
static uint16_t adc_x,adc_y;
static float adc_press;
volatile static uint8_t i = 0;
static uint8_t step = 0;
extern uint8_t calebrate_done;
void GUI_TOUCH_X_ActivateX(void) {
}

void GUI_TOUCH_X_ActivateY(void) {
}

int  GUI_TOUCH_X_MeasureX(void) {

    if(((calebrate_done&0x1) == 1)&&(PEN == 0))
    {
        if(TP_Read_Pressure(&adc_press) == 0)
        {
            if(adc_press >= 2000 && adc_press <= 6000)
            {
                i = i + Y;
                if(i == 200)//持续5S
                {
                    i = 0;
                    bitset(calebrate_done,5);
                    bitclr(calebrate_done,0);
                }
            }
            else
            {
                i = 0;
            }
        }
        if(TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0])) //读取屏幕坐标
        {
            tp_dev.x[0] = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff; //将结果转换为屏幕坐标
            tp_dev.y[0] = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
        }
        adc_x = tp_dev.x[0];
        adc_y = tp_dev.y[0];
        if(adc_y <= 40 && adc_y >= 0)
        {
            if(adc_x <= 40 && adc_x >= 0)
            {
                bitset(calebrate_done,6);
            }
            switch(step)
            {
            case 0:
                if(adc_x <= 800 && adc_x >= 400)
                    step++;
                else
                    step = 0;
            break;
//            case 1:
//                if(adc_x < 600 && adc_x >= 400)
//                    step++;
//                else
//                    step = 0;
//            break;
            case 1:
                if(adc_x < 400 && adc_x >= 0)
                {
                    step = 0;
                    bitset(calebrate_done,7);
                }
                else
                    step = 0;
            break;

            }
        }
        else
        {
            step = 0;
        }
    }
    else
    {
        adc_x = 0;
        adc_y = 0;
    }

    return adc_x;
}

int  GUI_TOUCH_X_MeasureY(void) {
    return adc_y;
}


