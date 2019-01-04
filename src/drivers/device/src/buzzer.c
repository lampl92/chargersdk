/**
* @file buzzer.c
* @brief 蜂鸣器驱动，具备常开和PWM控制两种模式
* @author rgw
* @version v1.0
* @date 2017-01-23
*/
#include "bsp.h"
#include "buzzer.h"

void buzzer(int n, int t1, int t2)
{
    for (int i = 0; i < n; i++)
    {
        Buzzer_control(1);
        bsp_DelayMS(t1);
        Buzzer_control(0);
        bsp_DelayMS(t1);
    }
    bsp_DelayMS(t2);
}
//只实现操作，具体蜂鸣器驱动单独实现
