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
#include "GUI_backstage.h"
#include "GUI.h"
#include "bsp.h"
#include "lcddrv.h"
#include "touch.h"
#include "touchtimer.h"

#define CALEBRATE_DEBUG 0
#define CALEBRATE_TIME  860

#define DOWN_VALID		2	/* 按下30ms 后, 开始统计ADC */

//第0位为进入校准标志位
//第1位为背光标志位
uint16_t flag_specially = 0;

volatile static uint16_t adc_x = 0, adc_y = 0;
static uint8_t step = 0;
GUI_PID_STATE State = { 0 };
static uint8_t _pidFlag = 0;
static uint16_t _pidCount = 0;
static uint32_t _AsmtCount = 0;//进入广告时间计数
static uint8_t s_count = 0;//按键消抖


//static uint32_t _pointpause = 0;//

extern uint16_t calebrate_done;

void GUI_TOUCH_X_ActivateX(void)
{
    if ((calebrate_done & 0x01) == 1)//初始化完成
    {
        if (PEN == 0)// && (TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0])))//有触摸并且结果有效
        {
            if (s_count >= DOWN_VALID)
            {
                if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]))
                {
                    //Buzzer_control(1);
                    adc_x = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff; //将结果转换为屏幕坐标
                    adc_y = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;
                    State.x = adc_x;
                    State.y = adc_y;
                    State.Pressed = 1;
                    GUI_TOUCH_StoreStateEx(&State);
                }
            }
            else
            {
                s_count++;
            }
        }
        else
        {
            if (s_count > 0)
            {
                if (--s_count == 0)
                {
                    State.Pressed = 0;
                    GUI_TOUCH_StoreStateEx(&State);
                    s_count = 0;
                }
            }
        }
    }    
}

void GUI_TOUCH_X_ActivateX1(void)
{
    if ((calebrate_done & 0x01) == 1)//初始化完成
    {
        if (PEN == 0)// && (TP_Read_XY2(&tp_dev.x[0],&tp_dev.y[0])))//有触摸并且结果有效
        {
            if (TP_Read_XY2(&tp_dev.x[0], &tp_dev.y[0]))
            {
                _AsmtCount = 0;//一旦触屏则广告时间计数归零
                (_pidFlag == 0) ? (_pidFlag = 1) : (_pidFlag = 0);
               // Buzzer_control(1);
                if (_pidFlag == 1)
                {
                    adc_x = tp_dev.xfac * tp_dev.x[0] + tp_dev.xoff; //将结果转换为屏幕坐标
                    adc_y = tp_dev.yfac * tp_dev.y[0] + tp_dev.yoff;

                    if (!bittest(winCreateFlag, 2))
                    {
                        State.x = adc_x;
                        State.y = adc_y;
                        State.Pressed = 1;
                        GUI_TOUCH_StoreStateEx(&State);
                    }
                    
                }
            }
        }
        else
        {

            State.x = adc_x;
            State.y = adc_y;
            State.Pressed = 0;
            GUI_TOUCH_StoreStateEx(&State);

            if ((cur_win == _hWinHome) || (cur_win == _hWinCharging))
            {
                _AsmtCount++;
                if (_AsmtCount > xSysconf.ulDispSleepTime_s * 50)
                {
                    _AsmtCount = 0;
                    bitset(calebrate_done, 8);//calebrate_done的第八位为广告页
                }
            }
            else
            {
                _AsmtCount = 0;
            }

//            _pidFlag = 0;
//            Buzzer_control(0);

            /**< 键盘鼠标路线会触发拐角的键,
            暂时在键盘页增加下面的返回值,点击后把鼠标位置置于左角 */
            if (_pidFlag == 1)
            {
                _pidFlag = 0;
                Buzzer_control(0);
                if (bittest(winCreateFlag, 2))
                {
                    State.x = 400;
                    State.y = 60;
                }
                State.Pressed = 0;
                GUI_TOUCH_StoreStateEx(&State);
            }
            _pidFlag = 0;
        }
    }
}

void GUI_TOUCH_X_ActivateY(void)
{

}

int  GUI_TOUCH_X_MeasureX(void)
{
    if (PEN == 0)//检测到有效坐标
    {
        _AsmtCount = 0;//一旦触屏则广告时间计数归零
        bitclr(flag_specially, 1);
        _pidCount++;
#if CALEBRATE_DEBUG
        printf_safe("_pid_count = %d  ", _pidCount);
        if (_pidCount % 10 == 0)
            printf_safe("\n");
#endif
        switch (_pidCount)
        {
        case CALEBRATE_TIME:
            _pidCount = 0;
#if CALEBRATE_DEBUG
            printf_safe("\n进入校准模式!\n");
#endif
            bitset(flag_specially, 0);//校准

           // Buzzer_control(0);
           // State.Pressed = 0;
            //GUI_TOUCH_StoreStateEx(&State);
            break;
        }
    }
    else
    {       
        _pidCount = 0;
        if (gbsstate == StateHome)
        {
            _AsmtCount++;
            if (_AsmtCount > xSysconf.ulDispSleepTime_s * 50)
            {
                _AsmtCount = xSysconf.ulDispSleepTime_s * 50 + 1;
                bitset(flag_specially, 1);//flag_specially的第1位为广告页
            }
        }
        else
        {
            _AsmtCount = 0;
            bitclr(flag_specially, 1);
        }
    }

    return adc_x;
}

int  GUI_TOUCH_X_MeasureY(void) {
    return adc_y;
}
