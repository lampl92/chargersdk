#ifndef USER_APP_H_INCLUDED
#define USER_APP_H_INCLUDED
#include "myiic.h"
#include "led_control.h"
#include "ST_LIS2DH12.h"

extern void TIM_SetTIM4Compare1(unsigned int compare);
extern void TIM_SetTIM2Compare1(unsigned int compare);
#define A_KEY_OFF    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2,GPIO_PIN_RESET)//ctr_gjd1
#define A_KEY_ON     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2,GPIO_PIN_SET)//ctr_gjd1

#define B_KEY_OFF    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3,GPIO_PIN_RESET)//ctr_gjd2
#define B_KEY_ON     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3,GPIO_PIN_SET) //ctr_gjd2

#define C_KEY_OFF    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6,GPIO_PIN_RESET)//ctr_gjd3
#define C_KEY_ON     HAL_GPIO_WritePin(GPIOE, GPIO_PIN_6,GPIO_PIN_SET) //ctr_gjd3

#define D_KEY_OFF    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8,GPIO_PIN_RESET) //ctr_gjd4
#define D_KEY_ON     HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8,GPIO_PIN_SET)//ctr_gjd4

#define POWER_L_ON   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_SET)
#define POWER_L_OFF  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,GPIO_PIN_RESET)

#define POWER_N_ON   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,GPIO_PIN_SET)
#define POWER_N_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5,GPIO_PIN_RESET)

#define PWM1_ON      HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1)//¿ªÆôPWM1
#define PWM2_ON      HAL_TIM_OC_Start_IT(&htim4, TIM_CHANNEL_2)//¿ªÆôPWM2
#define PWM3_ON      HAL_TIM_OC_Start_IT(&htim3, TIM_CHANNEL_2)//¿ªÆôPWM2
#define PWM4_ON      HAL_TIM_OC_Start_IT(&htim8, TIM_CHANNEL_3)//¿ªÆôPWM2

#define PWM1_OFF     ;//HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1)//¹Ø±ÕPWM1
#define PWM2_OFF     ;//HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_2)//¹Ø±ÕPWM2

#define GET_CC1          HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)
#define GET_CC2         HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_11)
#define GET_GUN_STATE_1  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)
#define GET_GUN_STATE_2  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)
#define Get_Power_Status           (~HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_3) & 0x01)
#define GET_RELAY1_STATE_1         (~HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_7) & 0x01)//双枪交流接触器1常开辅助节点检测返回1位闭合0位断开 枪1
#define GET_RELAY1_STATE_2         (~HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_8) & 0x01)//双枪交流接触器2常开辅助节点检测返回1位闭合0位断开 枪2

#define RS485_EN  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8,GPIO_PIN_SET)
#define RS485_DIS HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8,GPIO_PIN_RESET)

#define cs_zl_set   do{Chip2.cs_zl=1;write_pca9554_2();}while(0)
#define cs_zl_reset do{Chip2.cs_zl=0;write_pca9554_2();}while(0)

#define GPRS_reset      do {Chip1.GPRS_key=0;write_pca9554_1();} while(0)
#define GPRS_set    do {Chip1.GPRS_key=1;write_pca9554_1();} while(0)

#define TIMER5_ON    HAL_TIM_Base_Start_IT(&htim5)
#define TIMER5_OFF   HAL_TIM_Base_Stop_IT(&htim5)

#define TIMER3_ON    HAL_TIM_Base_Start_IT(&htim3)
#define TIMER3_OFF   HAL_TIM_Base_Stop_IT(&htim3)

#define TIMER8_ON    HAL_TIM_Base_Start_IT(&htim8)
#define TIMER8_OFF   HAL_TIM_Base_Stop_IT(&htim8)

#define PWM1_1000  TIM_SetTIM2Compare1(TIMER_MAX)
#define PWM1_535   TIM_SetTIM2Compare1(465)
#define PWM1_0   TIM_SetTIM2Compare1(1000)

#define RELAY2_ON  user_pwm_relay2_setvalue(1000)
#define RELAY2_KEEP  user_pwm_relay2_setvalue(500)
#define RELAY2_OFF  user_pwm_relay2_setvalue(0)

#define RELAY1_ON  user_pwm_relay1_setvalue(1000)
#define RELAY1_KEEP   user_pwm_relay1_setvalue(500)
#define RELAY1_OFF  user_pwm_relay1_setvalue(0)

#define PWM2_1000  TIM_SetTIM4Compare1(TIMER_MAX)
#define PWM2_535   TIM_SetTIM4Compare1(465)
#define PWM2_0   TIM_SetTIM4Compare1(1000)

#define TEMP_N_OUT  0X00 //板卡输出N
#define TEMP_L_OUT  0X01 //    L
#define TEMP_N_IN   0X02 //市电输入N
#define TEMP_L_IN   0X03 //市电输入L
#define TEMP_GUN1_POSITIVE  0X04
#define TEMP_GUN1_NEGATIVE  0X05
#define TEMP_GUN2_POSITIVE  0X06
#define TEMP_GUN2_NEGATIVE  0X07
#define VREF_1v5            0X0F

#define samp_sum    124
#define samp_dma    2
#define TIMER_MAX 1 //不能写成0
typedef struct
{
    unsigned short va_samp[samp_sum];
    unsigned short ia_samp[samp_sum];
    unsigned short leakage_current_samp[samp_sum];
} AC_samp_t;
typedef struct
{
    unsigned short CP1[samp_sum];
    unsigned short CP2[samp_sum];
    unsigned short CD4067[samp_sum];
} DC_samp_t;
typedef struct
{
    double va;
    double ia;
    double leakage_current;
} AC_t;
typedef struct
{
    double CP1;
    double CP2;
    double CD4067;
    double TEMP1;
    double TEMP2;
    double TEMP3;
    double TEMP4;
    double TEMP_ARM1;
    double TEMP_ARM2;
    double TEMP_ARM3;
    double TEMP_ARM4;
} DC_t;
typedef struct
{
    AC_samp_t AC_samp;
    DC_samp_t DC_samp;
    AC_t AC;
    DC_t DC;
} samp;
typedef struct
{
    unsigned short CD4067;
    unsigned short CP1;
    unsigned short CP2;
} AD_samp[samp_dma];
AD_samp AD_samp_dma;

typedef struct
{
    uint8_t a_select;
    uint8_t b_select;
    uint8_t c_select;
    uint8_t d_select;
    uint8_t cs1_select;
    uint8_t RESET_3G;
    uint8_t KEY_BT;
    uint8_t GPRS_key;
} IO_chip1;
IO_chip1 Chip1;
typedef struct
{
    uint8_t in1;
    uint8_t in2;
    uint8_t pause;
    uint8_t in6;
    uint8_t in7;
    uint8_t in8;
    uint8_t cs_zl;
    uint8_t buzzer;
} IO_chip2;
IO_chip2 Chip2;
void IIC_Init(void);
void calc_CP1(void);
void calc_CP2(void);
double get_CP1Volt(void);
double get_CP2Volt(void);
float get_dc_massage(uint8_t DC_channel);
void write_pca9554_1(void);
void PCA9554_init(void);
void Gun_test(void);
void Power_out_n_pwm_ctrl(void);
void Power_out_l_pwm_ctrl(void);
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_ADC1_Init(void);
void TIM_SetTIM2Compare1(unsigned int);
void TIM2_PWM_Init(unsigned int, unsigned int);
void DMA_START(void);
void Peripheral_Init(void);
void Peripheral_Start(void);
void POWER_L_CLOSE(void);
void POWER_N_CLOSE(void);
void POWER_L_OPEN(void);
void POWER_N_OPEN(void);
void Close_gun_1(void);
void Open_gun_1(void);
void Close_gun_2(void);
void Open_gun_2(void);
void Buzzer_control(uint8_t state);
uint8_t Get_State_relay(uint32_t relay_id);
uint8_t flag_pwm_out_n, flag_pwm_out_l, flag_gun_Close, flag_gun_Open, flag_power_out_l, flag_power_out_n;

double curr2duty(double rate_curr);
void curr2pwm(double rate_curr, uint8_t con_id);
#endif /* USER_APP_H_INCLUDED */
