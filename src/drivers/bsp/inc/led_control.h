#ifndef LED_CONTROL_H_INCLUDED
#define LED_CONTROL_H_INCLUDED
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
uint16_t pwm_r_1, pwm_g_1, pwm_b_1, pwm_r_2, pwm_g_2, pwm_b_2,duty_ratio_r_1,duty_ratio_r_2,duty_ratio_g_1,duty_ratio_g_2,
         duty_ratio_b_1,duty_ratio_b_2;
uint8_t flag_breath_r_1,flag_breath_r_2,flag_breath_g_1,flag_breath_g_2,flag_breath_b_1,flag_breath_b_2;
 void led_breath_r(void);
 void led_breath_g(void);
 void led_breath_b(void);

#endif /* LED_CONTROL_H_INCLUDED */
