#include "led_control.h"
#include "user_app.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "bsp_timer.h"
void led_ctrl_r(uint8_t num, uint16_t state)
{
    if (num == 1)
    {
        if (state == keep_on)
        {
            flag_flicker_r_1 = 0;
            flag_breath_r_1 = 0;
            flag_satuse_r_1 = 1;
        }
        else if (state == keep_off)
        {
            flag_satuse_r_1 = 0;
            flag_flicker_r_1 = 0;
            flag_breath_r_1 = 0;
        }
        else if (state == breath)
        {
            flag_breath_r_1 = 1;
            flag_flicker_r_1 = 0;
            flag_satuse_r_1 = 0;
        }
        else if (state == flicker)
        {
            flag_flicker_r_1 = 1;
            flag_breath_r_1 = 0;
            flag_satuse_r_1 = 0;
        }
    }
    else if (num == 2)
    {

        if (state == keep_on)
        {
            flag_satuse_r_2 = 1;
            flag_breath_r_2 = 0;
            flag_flicker_r_2 = 0;
        }
        else if (state == keep_off)
        {
            flag_satuse_r_2 = 0;
            flag_breath_r_2 = 0;
            flag_flicker_r_2 = 0;
        }
        else if (state == breath)
        {
            flag_breath_r_2 = 1;
            flag_flicker_r_2 = 0;
            flag_satuse_r_2 = 0;
        }
        else if (state == flicker)
        {
            flag_breath_r_2 = 0;
            flag_flicker_r_2 = 1;
            flag_satuse_r_2 = 0;
        }
    }
}
void led_ctrl_g(uint8_t num, uint16_t state)
{
    if (num == 1)
    {
        if (state == keep_on)
        {
            flag_satuse_g_1 = 1;
            flag_flicker_g_1 = 0;
            flag_breath_g_1 = 0;
        }
        else if (state == keep_off)
        {
            flag_satuse_g_1 = 0;
            flag_flicker_g_1 = 0;
            flag_breath_g_1 = 0;
        }
        else if (state == breath)
        {
            flag_breath_g_1 = 1;
            flag_flicker_g_1 = 0;
            flag_satuse_g_1 = 0;
        }
        else if (state == flicker)
        {
            flag_flicker_g_1 = 1;
            flag_breath_g_1 = 0;
            flag_satuse_g_1 = 0;
        }
    }
    else if (num == 2)
    {

        if (state == keep_on)
        {
            flag_satuse_g_2 = 1;
            flag_flicker_g_2 = 0;
            flag_breath_g_2 = 0;
        }
        else if (state == keep_off)
        {
            flag_satuse_g_2 = 0;
            flag_flicker_g_2 = 0;
            flag_breath_g_2 = 0;
        }
        else if (state == breath)
        {
            flag_breath_g_2 = 1;
            flag_flicker_g_2 = 0;
            flag_satuse_g_2 = 0;
        }
        else if (state == flicker)
        {
            flag_flicker_g_2 = 1;
            flag_breath_g_2 = 0;
            flag_satuse_g_2 = 0;
        }
    }
}
void led_ctrl_b(uint8_t num, uint16_t state)
{
    if (num == 1)
    {
        if (state == keep_on)
        {
            flag_satuse_b_1 = 1;
            flag_breath_b_1 = 0;
            flag_flicker_b_1 = 0;

        }
        else if (state == keep_off)
        {
            flag_satuse_b_1 = 0;
            flag_breath_b_1 = 0;
            flag_flicker_b_1 = 0;

        }
        else if (state == breath)
        {
            flag_breath_b_1 = 1;
            flag_flicker_b_1 = 0;
            flag_satuse_b_1 = 0;
        }
        else if (state == flicker)
        {
            flag_flicker_b_1 = 1;
            flag_breath_b_1 = 0;
            flag_satuse_b_1 = 0;
        }
    }
    else if (num == 2)
    {

        if (state == keep_on)
        {
            flag_satuse_b_2 = 1;
            flag_breath_b_2 = 0;
            flag_flicker_b_2 = 0;

        }
        else if (state == keep_off)
        {
            flag_satuse_b_2 = 0;
            flag_breath_b_2 = 0;
            flag_flicker_b_2 = 0;

        }
        else if (state == breath)
        {
            flag_breath_b_2 = 1;
            flag_flicker_b_2 = 0;
            flag_satuse_b_2 = 0;
        }
        else if (state == flicker)
        {
            flag_flicker_b_2 = 1;
            flag_breath_b_2 = 0;
            flag_satuse_b_2 = 0;
        }
    }
}

void led_breath_r(void)
{
    if (flag_breath_r_1 == 1)
    {

        if (pwm_r_flag_1 == 0)
        {
            duty_ratio_r_1++;
            if (duty_ratio_r_1 >= 100)
            {
                pwm_r_flag_1 = 1;
            }
        }
        if (pwm_r_flag_1 == 1)
        {
            duty_ratio_r_1--;
            if (duty_ratio_r_1 <= 10)
            {
                pwm_r_flag_1 = 0;
            }

        }
    }
    if (flag_breath_r_2 == 1)
    {

        if (pwm_r_flag_2 == 0)
        {
            duty_ratio_r_2++;
            if (duty_ratio_r_2 >= 100)
            {
                pwm_r_flag_2 = 1;
            }
        }
        if (pwm_r_flag_2 == 1)
        {
            duty_ratio_r_2--;
            if (duty_ratio_r_2 <= 10)
            {
                pwm_r_flag_2 = 0;
            }

        }
    }
}
void led_breath_g(void)
{
    if (flag_breath_g_1 == 1)
    {

        if (pwm_g_flag_1 == 0)
        {
            duty_ratio_g_1++;
            if (duty_ratio_g_1 >= 100)
            {
                pwm_g_flag_1 = 1;
            }
        }
        if (pwm_g_flag_1 == 1)
        {
            duty_ratio_g_1--;
            if (duty_ratio_g_1 <= 10)
            {
                pwm_g_flag_1 = 0;
            }

        }
    }
    if (flag_breath_g_2 == 1)
    {

        if (pwm_g_flag_2 == 0)
        {
            duty_ratio_g_2++;
            if (duty_ratio_g_2 >= 100)
            {
                pwm_g_flag_2 = 1;
            }
        }
        if (pwm_g_flag_2 == 1)
        {
            duty_ratio_g_2--;
            if (duty_ratio_g_2 <= 10)
            {
                pwm_g_flag_2 = 0;
            }

        }
    }
}
void led_breath_b(void)
{
    if (flag_breath_b_1 == 1)
    {

        if (pwm_b_flag_1 == 0)
        {
            duty_ratio_b_1++;
            if (duty_ratio_b_1 >= 100)
            {
                pwm_b_flag_1 = 1;
            }
        }
        if (pwm_b_flag_1 == 1)
        {
            duty_ratio_b_1--;
            if (duty_ratio_b_1 <= 10)
            {
                pwm_b_flag_1 = 0;
            }

        }
    }
    if (flag_breath_b_2 == 1)
    {

        if (pwm_b_flag_2 == 0)
        {
            duty_ratio_b_2++;
            if (duty_ratio_b_2 >= 100)
            {
                pwm_b_flag_2 = 1;
            }
        }
        if (pwm_b_flag_2 == 1)
        {
            duty_ratio_b_2--;
            if (duty_ratio_b_2 <= 10)
            {
                pwm_b_flag_2 = 0;
            }

        }
    }
}
/**************LED ctrl **********************/
/*       
#define keep_off 0
#define keep_on  1
#define breath   2
#define flicker  3

#define red      0
#define green    1
#define blue     2                                    */
/********************************************/
void led_ctrl(uint8_t num, uint8_t colour, uint8_t state)
{	
    if (colour == red)
    {
        led_ctrl_g(num, keep_off);
        led_ctrl_b(num, keep_off);
        led_ctrl_r(num, state);	    
    }
    else if (colour == green)
    {
        led_ctrl_r(num, keep_off);
        led_ctrl_b(num, keep_off);
        led_ctrl_g(num, state);
    }
    else if (colour == blue)
    {
        led_ctrl_r(num, keep_off);
        led_ctrl_g(num, keep_off);
        led_ctrl_b(num, state);
    }
}
void led_flicker_out(void)
{
    if (flag_flicker_r_1 == 1)
    {
        if (timer_s % 4 == 0)
        {
            LED1_R_RUN;
        }
        else
        {
            LED1_R_OFF;
        }

    }
    if (flag_flicker_r_2 == 1)
    {
        if (timer_s % 4 == 0)
        {
            LED2_R_RUN;
        }
        else
        {
            LED2_R_OFF;
        }

    }
    if (flag_flicker_g_1 == 1)
    {

        if (timer_s % 4 == 0)
        {
            LED1_G_RUN;
        }
        else
        {
            LED1_G_OFF;
        }

    }
    if (flag_flicker_g_2 == 1)
    {

        if (timer_s % 4 == 0)
        {
            LED2_G_RUN;
        }
        else
        {
            LED2_G_OFF;
        }

    }
    if (flag_flicker_b_1 == 1)
    {

        if (timer_s % 4 == 0)
        {
            LED1_B_RUN;
        }
        else
        {
            LED1_B_OFF;
        }

    }
    if (flag_flicker_b_2 == 1)
    {
        if (timer_s % 4 == 0)
        {
            LED2_B_RUN;
        }
        else
        {
            LED2_B_OFF;
        }

    }
}
void led_breath_out(void)
{
    if (flag_breath_r_1 == 1)
    {
        if (pwm_r_1 >= duty_ratio_r_1)
        {
            LED1_R_RUN;
        }
        else
        {
            LED1_R_OFF;
        }
    }
    if (flag_breath_g_1 == 1)
    {
        if (pwm_g_1 >= duty_ratio_g_1)
        {
            LED1_G_RUN;
        }
        else
        {
            LED1_G_OFF;
        } 
    }
   
    if (flag_breath_b_1 == 1)
    {
        if (pwm_b_1 >= duty_ratio_b_1)
        {
            LED1_B_RUN;
        }
        else
        {
            LED1_B_OFF;
        }   
    }   


    if (flag_breath_r_2 == 1)
    {
        if (pwm_r_2 >= duty_ratio_r_2)
        {
            LED2_R_RUN;
        }
        else
        {
            LED2_R_OFF;
        } 
    }
    
    if (flag_breath_g_2 == 1)
    {
        if (pwm_g_2 >= duty_ratio_g_2)
        {
            LED2_G_RUN;
        }
        else
        {
            LED2_G_OFF;
        } 
    }
   

    if (flag_breath_b_2 == 1)
    {
        if (pwm_b_2 >= duty_ratio_b_2)
        {
            LED2_B_RUN;
        }
        else
        {
            LED2_B_OFF;
        }
    }
   
}
void led_status_out(void)
{
    if ((flag_satuse_r_1 == 1)&&(flag_breath_r_1 == 0)&&(flag_flicker_r_1 == 0))
    {
        LED1_R_RUN;
    }
    if ((flag_satuse_r_1 == 0)&&(flag_breath_r_1 == 0)&&(flag_flicker_r_1 == 0))
    {
        LED1_R_OFF;
    }
    if ((flag_satuse_g_1 == 1)&&(flag_breath_g_1 == 0)&&(flag_flicker_g_1 == 0))
    {
        LED1_G_RUN;
    }
    if ((flag_satuse_g_1 == 0)&&(flag_breath_g_1 == 0)&&(flag_flicker_g_1 == 0))
    {
        LED1_G_OFF;
    }
    if ((flag_satuse_b_1 == 1)&&(flag_breath_b_1 == 0)&&(flag_flicker_b_1 == 0))
    {
        LED1_B_RUN;
    }
    if ((flag_satuse_b_1 == 0)&&(flag_breath_b_1 == 0)&&(flag_flicker_b_1 == 0))
    {
        LED1_B_OFF;
    }



    if ((flag_satuse_r_2 == 1)&&(flag_breath_r_2 == 0)&&(flag_flicker_r_2 == 0))
    {
        LED2_R_RUN;
    }
    if ((flag_satuse_r_2 == 0)&&(flag_breath_r_2 == 0)&&(flag_flicker_r_2 == 0))
    {
        LED2_R_OFF;
    }
    if ((flag_satuse_g_2 == 1)&&(flag_breath_g_2 == 0)&&(flag_flicker_g_2 == 0))
    {
        LED2_G_RUN;
    }
    if ((flag_satuse_g_2 == 0)&&(flag_breath_g_1 == 0)&&(flag_flicker_g_2 == 0))
    {
        LED2_G_OFF;
    }
    if ((flag_satuse_b_2 == 1)&&(flag_breath_b_2 == 0)&&(flag_flicker_b_2 == 0))
    {
        LED2_B_RUN;
    }
    if ((flag_satuse_b_2 == 0)&&(flag_breath_b_2 == 0)&&(flag_flicker_b_2 == 0))
    {
        LED2_B_OFF;
    }
}
void led_output(void)
{
    led_breath_out();
    led_flicker_out();
    led_status_out();
}
void led_state_init(void)
{
    led_ctrl(1, red, keep_off);
    led_ctrl(1, blue, keep_off);
    led_ctrl(1, green, keep_off);
    led_ctrl(2, red, keep_off);
    led_ctrl(2, blue, keep_off);
    led_ctrl(2, green, keep_off);
}
