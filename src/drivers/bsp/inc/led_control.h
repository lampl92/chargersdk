#ifndef LED_CONTROL_H_INCLUDED
#define LED_CONTROL_H_INCLUDED
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define LED2_B_RUN  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2,GPIO_PIN_SET)
#define LED2_B_OFF  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2,GPIO_PIN_RESET)

#define LED2_G_RUN  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3,GPIO_PIN_SET)
#define LED2_G_OFF  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3,GPIO_PIN_RESET)

#define LED2_R_RUN  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4,GPIO_PIN_SET)
#define LED2_R_OFF  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4,GPIO_PIN_RESET)

#define LED1_R_RUN  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,GPIO_PIN_SET)
#define LED1_R_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,GPIO_PIN_RESET)

#define LED1_G_RUN  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,GPIO_PIN_SET)
#define LED1_G_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,GPIO_PIN_RESET)

#define LED1_B_RUN  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7,GPIO_PIN_SET)
#define LED1_B_OFF  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7,GPIO_PIN_RESET)

#define RUN_ON      HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5,GPIO_PIN_SET)
#define RUN_OFF     HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5,GPIO_PIN_RESET)

uint16_t pwm_r_1, pwm_g_1, pwm_b_1, pwm_r_2, pwm_g_2, pwm_b_2,
         duty_ratio_r_1,duty_ratio_r_2,duty_ratio_g_1,duty_ratio_g_2,duty_ratio_b_1,duty_ratio_b_2;

uint8_t flag_breath_r_1,flag_breath_r_2,flag_breath_g_1,flag_breath_g_2,flag_breath_b_1,flag_breath_b_2,
        flag_flicker_r_1,flag_flicker_r_2,flag_flicker_g_1,flag_flicker_g_2,flag_flicker_b_1,flag_flicker_b_2;
void led_ctrl(uint8_t num, uint8_t colour, uint8_t state);
 void led_state_init(void);
 void led_breath_r(void);
 void led_breath_g(void);
 void led_breath_b(void);
 void led_output(void);

#endif /* LED_CONTROL_H_INCLUDED */
