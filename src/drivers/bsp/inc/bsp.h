/**
* @file bsp.h
* @brief 定义板级驱动功能开关,相应头文件引用.
* @author rgw
* @version v1.0
* @date 2016-11-01
*/
#ifndef _BSP_H
#define _BSP_H
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* Public define -------------------------------------------------------------*/
#define STM32F4
//#define STM32F0
#define USE_FreeRTOS


/* NVIC PreemptPriority ------------------------------------------------------*/

#define bspUSART1_PreemptPriority       3
#define bspUSART1_SubPriority           0

#define bspUSART2_PreemptPriority       4
#define bspUSART2_SubPriority           0

#define bspUSART3_PreemptPriority       5
#define bspUSART3_SubPriority           0

#define bspLTDC_PreemptPriority         15
#define bspLTDC_SubPriority             0

#define bspWKUP_PreemptPriority         6
#define bspWKUP_SubPriority             0

#define bspAlarm_PreemptPriority        7
#define bspAlarm_SubPriority            0

/* CPU空闲时执行的函数 */
//#define CPU_IDLE()        bsp_Idle()

#ifdef USE_FreeRTOS
#define DISABLE_INT()    taskENTER_CRITICAL()
#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
/* 开关全局中断的宏 */
#define ENABLE_INT()    __set_PRIMASK(0)    /* 使能全局中断 */
#define DISABLE_INT()   __set_PRIMASK(1)    /* 禁止全局中断 */
#endif


/*
    EXTI9_5_IRQHandler 的中断服务程序分散在几个独立的 bsp文件中。
    需要整合到 stm32f4xx_it.c 中。
    定义下面行表示EXTI9_5_IRQHandler入口函数集中放到 stm32f4xx_it.c。
*/
#define EXTI9_5_ISR_MOVE_OUT

#define DEBUG_GPS_TO_COM1   /* 打印GPS数据到串口1 */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "errorcode.h"

/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_uart.h"
#include "bsp_sdram.h"
#include "bsp_dwt.h"
#include "bsp_rtc.h"
//#include "bsp_ltdc.h"
#include "bsp_user_lib.h"
#include "bsp_nand_hw.h"
#include "bsp_nand_ftl.h"
#include "bsp_touch_2046.h"

/* Public function prototypes ------------------------------------------------*/
void bsp_Init(void);
void bsp_Idle(void);
void bsp_Error_Handler(void);

/* private function prototypes -----------------------------------------------*/

typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;
typedef const int16_t sc16;
typedef const int8_t sc8;

typedef __IO int32_t  vs32;
typedef __IO int16_t  vs16;
typedef __IO int8_t   vs8;

typedef __I int32_t vsc32;
typedef __I int16_t vsc16;
typedef __I int8_t vsc8;

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;
typedef const uint16_t uc16;
typedef const uint8_t uc8;

typedef __IO uint32_t  vu32;
typedef __IO uint16_t vu16;
typedef __IO uint8_t  vu8;

typedef __I uint32_t vuc32;
typedef __I uint16_t vuc16;
typedef __I uint8_t vuc8;

//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014
#define GPIOJ_ODR_ADDr    (GPIOJ_BASE+20) //0x40022414
#define GPIOK_ODR_ADDr    (GPIOK_BASE+20) //0x40022814

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010
#define GPIOJ_IDR_Addr    (GPIOJ_BASE+16) //0x40022410
#define GPIOK_IDR_Addr    (GPIOK_BASE+16) //0x40022810

//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入

#define PJout(n)   BIT_ADDR(GPIOJ_ODR_Addr,n)  //输出
#define PJin(n)    BIT_ADDR(GPIOJ_IDR_Addr,n)  //输入

#define PKout(n)   BIT_ADDR(GPIOK_ODR_Addr,n)  //输出
#define PKin(n)    BIT_ADDR(GPIOK_IDR_Addr,n)  //输入


/********************************************************************
sys.h
**********************************************************************/

#define LED2_R_RUN  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2,GPIO_PIN_RESET)
#define LED2_R_OFF  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_2,GPIO_PIN_SET)

#define LED2_G_RUN  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3,GPIO_PIN_RESET)
#define LED2_G_OFF  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_3,GPIO_PIN_SET)

#define LED2_B_RUN  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4,GPIO_PIN_RESET)
#define LED2_B_OFF  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_4,GPIO_PIN_SET)

#define LED1_R_RUN  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,GPIO_PIN_RESET)
#define LED1_R_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3,GPIO_PIN_SET)

#define LED1_G_RUN  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,GPIO_PIN_RESET)
#define LED1_G_OFF  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4,GPIO_PIN_SET)

#define LED1_B_RUN  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7,GPIO_PIN_RESET)
#define LED1_B_OFF  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7,GPIO_PIN_SET)

#define RUN_ON      HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5,GPIO_PIN_SET)
#define RUN_OFF     HAL_GPIO_WritePin(GPIOH, GPIO_PIN_5,GPIO_PIN_RESET)

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


#define TIMER5_ON    HAL_TIM_Base_Start_IT(&htim5)
#define TIMER5_OFF   HAL_TIM_Base_Stop_IT(&htim5)

#define TIMER3_ON    HAL_TIM_Base_Start_IT(&htim3)
#define TIMER3_OFF   HAL_TIM_Base_Stop_IT(&htim3)

#define PWM1_1000  TIM_SetTIM2Compare1(1000);
#define PWM1_535   TIM_SetTIM2Compare1(535);

#define PWM2_1000  TIM_SetTIM4Compare1(1000);
#define PWM2_535   TIM_SetTIM4Compare1(535);

#define write_chip1 0x40 //0100 0000
#define read_chip1 0x41 //0100 0001

#define write_chip2 0x42 //0100 0010
#define read_chip2 0x43 //0100 0011


#define keep_off 0
#define keep_on  1
#define breath   2

#define red      0
#define green    1
#define blue     2

#define read     0x03
#define write    0x10

#define voltage  0x000A
#define current  0x0016
#define power    0x001C
#define electric_energy 0x0065
#define frequency 0x0030



#define samp_sum   200
#define samp_dma    10
#define ia_k       0.02197265
#define va_k       0.22056//0.38?????÷??????????·?????×è300??1
#define leakage_current_k   0.073242
#define temper_k   100
#define CP1_k      0.00344238
#define CP2_k      0.00344238//14.1/3??·???±???
#define electricity_meter_num 256

void Stm32_Clock_Init(u32 plln, u32 pllm, u32 pllp, u32 pllq); //?±??????????
//????????±à????
void WFI_SET(void);     //????WFI????
void INTX_DISABLE(void);//??±??ù??????
void INTX_ENABLE(void); //?????ù??????
void MSR_MSP(u32 addr); //?è?????????·
extern void MX_GPIO_Init(void);
extern void MX_DMA_Init(void);
extern void MX_ADC1_Init(void);
extern void TIM_SetTIM2Compare1(unsigned int);
extern void TIM2_PWM_Init(unsigned int, unsigned int);
extern void DMA_START(void);
extern void MX_TIM3_Init(void);
extern void MX_TIM2_Init(void);
extern void MX_TIM1_Init(void);
extern double get_CP1(void);
extern double get_CP2(void);
extern double get_va(void);
extern ADC_HandleTypeDef hadc1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;
extern void Gun_test(void);
extern void Peripheral_Init(void);


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
    unsigned short va_samp;
    unsigned short ia_samp;
    unsigned short leakage_current;
    unsigned short CP1;
    unsigned short CP2;

} AD_samp[samp_dma];
typedef struct
{
    struct
   {
       uint8_t flag_va;
       uint8_t flag_ia;
       uint8_t flag_power;
       uint8_t flag_electric_energy;
       uint8_t flag_frequency;
   }flag;
     struct
   {
       float  massage_va;
       float  massage_ia;
       float massage_power;
       float massage_electric_energy;
       float massage_frequency;
   }massage;

} electricity_meter[electricity_meter_num];

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

typedef struct
{
    unsigned char pwm1;
    unsigned char pwm2;
} flag;
/********************************************************************
end of sys.h
**********************************************************************/




void  printf_safe(char *format, ...);
void bsp_Error_Handler(void);

#endif

