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
#define PWM1_OFF     HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1)//¹Ø±ÕPWM1
#define PWM2_OFF     HAL_TIM_OC_Stop_IT(&htim4, TIM_CHANNEL_2)//¹Ø±ÕPWM2

#define GET_CC1          HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)
#define GET_CC2         HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_11)
#define GET_GUN_STATE_1  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8)
#define GET_GUN_STATE_2  HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9)

#define RS485_EN  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8,GPIO_PIN_SET)
#define RS485_DIS HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8,GPIO_PIN_RESET)

#define cs_zl_set do{Chip2.cs_zl=1;write_pca9554_2();}while(0)
#define cs_zl_reset do{Chip2.cs_zl=0;write_pca9554_2();}while(0)


#define TIMER5_ON    HAL_TIM_Base_Start_IT(&htim5)
#define TIMER5_OFF   HAL_TIM_Base_Stop_IT(&htim5)

#define TIMER3_ON    HAL_TIM_Base_Start_IT(&htim3)
#define TIMER3_OFF   HAL_TIM_Base_Stop_IT(&htim3)

#define PWM1_1000  do{TIM_SetTIM2Compare1(1000);}while(0)
#define PWM1_535   do{TIM_SetTIM2Compare1(535);}while(0)

#define PWM2_1000  do{TIM_SetTIM4Compare1(1000);}while(0)
#define PWM2_535   do{TIM_SetTIM4Compare1(535);}while(0)

#define write_chip1 0x40 //0100 0000
#define read_chip1 0x41 //0100 0001

#define write_chip2 0x42 //0100 0010
#define read_chip2 0x43 //0100 0011

#define TEMP_L_OUT 0X00
#define TEMP_L_IN  0X01
#define TEMP_N_OUT 0X02
#define TEMP_N_IN  0X03
#define TEMP_GUN1_POSITIVE  0X04
#define TEMP_GUN1_NEGATIVE  0X05
#define TEMP_GUN2_POSITIVE  0X06
#define TEMP_GUN2_NEGATIVE  0X07
#define VREF_1v5            0X0F



#define keep_off 0
#define keep_on  1
#define breath   2
#define flicker  3

#define red      0
#define green    1
#define blue     2

#define read     0x03
#define write    0x10

#define voltage  0x000b
#define current  0x000c
#define power    0x000d
#define electric_energy_l 0x0001
#define electric_energy_h 0x0000
#define frequency 0x0011

#define	AXISDATA_REG	0x28
#define lock_timer      20
#define samp_sum    125
#define samp_dma    10
#define ia_k       0.02197265
#define va_k       0.22056//0.38?????÷??????????·?????×è300??1
#define leakage_current_k   0.073242
#define temper_k   100
#define CP1_k      0.00344238
#define CP2_k      0.00344238//14.1/3??·???±???
#define electricity_meter_num 256
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
samp Sys_samp;
typedef struct
{
    unsigned short CD4067;
    unsigned short va_samp;
    unsigned short ia_samp;
    unsigned short leakage_current;
    unsigned short CP1;
    unsigned short CP2;

} AD_samp[samp_dma];
AD_samp AD_samp_dma;
typedef struct
{
    struct
   {
       uint8_t flag_va;
       uint8_t flag_ia;
       uint8_t flag_power;
       uint8_t flag_electric_energy_l;
       uint8_t flag_electric_energy_h;
       uint8_t flag_frequency;
       uint8_t flag_erro;
   }flag;
     struct
   {
       float  massage_va;
       float  massage_ia;
       float massage_power;
       float massage_electric_energy;
       float massage_electric_energy_l;
       float massage_electric_energy_h;
       float massage_frequency;
   }massage;

} electricity_meter[electricity_meter_num];
electricity_meter  Electricity_meter;

typedef struct
{
    uint8_t a_select;
    uint8_t b_select;
    uint8_t c_select;
    uint8_t d_select;
    uint8_t cs1_select;
    uint8_t RESET_3G;
    uint8_t KEY_BT;
    uint8_t wifi_key;
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
typedef struct
{
    unsigned char pwm1;
    unsigned char pwm2;
} flag;
flag sys_flag;
void IIC_Init(void);
double get_CP1(void);
double get_CP2(void);
double get_va(void);
float get_dc_massage(uint8_t DC_channel);
void write_pca9554_1(void);
void Delay_ms(unsigned long long time);
void PCA9554_init(void);
void Gun_test(void);
void Power_out_n_pwm_ctrl(void);
void Power_out_l_pwm_ctrl(void);
void WFI_SET(void);     //????WFI????
void INTX_DISABLE(void);//??±??ù??????
void INTX_ENABLE(void); //?????ù??????
void MX_GPIO_Init(void);
void MX_DMA_Init(void);
void MX_ADC1_Init(void);
void TIM_SetTIM2Compare1(unsigned int);
void TIM2_PWM_Init(unsigned int, unsigned int);
void DMA_START(void);
void Peripheral_Init(void);
void POWER_L_CLOSE(void);
void POWER_N_CLOSE(void);
void POWER_L_OPEN(void);
void POWER_N_OPEN(void);
void Close_gun_1(void);
void Open_gun_1(void);
void Close_gun_2(void);
void Open_gun_2(void);
void get_samp_point(void);//ÓÃÊ±30¦ÌS
uint8_t flag_rs485[255];
uint8_t flag_pwm_out_n,flag_pwm_out_l,flag_gun_Close,flag_gun_Open,flag_power_out_l,flag_power_out_n;
uint16_t num_cp1,num_cp2;
double vref,va;
uint8_t RS485_RX_MODBUS_CNT;
uint32_t CD4067_sum,leakage_current_sum,va_samp_sum,ia_samp_sum,CP2_sum,CP1_sum,CP1_sum_sys,CP2_sum_sys;

#endif /* USER_APP_H_INCLUDED */
