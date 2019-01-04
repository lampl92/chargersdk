#ifndef LED_CONTROL_H_INCLUDED
#define LED_CONTROL_H_INCLUDED

#include "stm32f4xx.h"

#define keep_off 0
#define keep_on  1
#define breath   2
#define flicker  3

#define led_red      0
#define led_green    1
#define led_blue     2
#define led_yellow   3

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

void led_ctrl(uint8_t num, uint8_t colour, uint8_t state);
void led_state_init(void);
void led_breath_r(void);
void led_breath_g(void);
void led_breath_b(void);
void led_output(void);

#endif /* LED_CONTROL_H_INCLUDED */
