#include "stm32f4xx_hal.h"
#include "bsp.h"
#include <stdlib.h>
#include <math.h>
#include "user_app.h"
#include "bsp_timer.h"
#include "electric_energy_meter.h"
#include "FreeRTOS.h"
#include "task.h"
#include "evse_globals.h"
#include "taskcreate.h"
float frequency_test;
samp Sys_samp;
void user_pwm_relay2_setvalue(uint16_t value);
void user_pwm_relay1_setvalue(uint16_t value);
static void value_reset(void)
{
    Sys_samp.DC.TEMP_ARM1 = 0;
    Sys_samp.DC.TEMP_ARM2 = 0;
    Sys_samp.DC.TEMP_ARM3 = 0;
    Sys_samp.DC.TEMP_ARM4 = 0;
    Sys_samp.DC.TEMP1 = 0;
    Sys_samp.DC.TEMP2 = 0;
    Sys_samp.DC.TEMP3 = 0;
    Sys_samp.DC.TEMP4 = 0;
}
const double  resistance[146] =
{
    382.300, 358.686, 336.457, 315.560, 295.938, 277.531, 260.278, 244.117, 228.987, 214.829,
    201.585, 189.199, 177.617, 166.789, 156.665, 147.200, 138.349, 130.073, 122.333, 115.093,
    108.319, 101.979, 96.045, 90.489, 85.284, 80.409, 75.839, 71.554, 67.537, 63.767,
    60.230, 56.908, 53.789, 50.859, 48.105, 45.515, 43.080, 40.789, 38.632, 36.602,
    34.665, 32.888, 31.189, 29.588, 28.078, 26.653, 25.309, 24.039, 22.840, 21.708,
    20.690, 19.625, 18.669, 17.764, 16.907, 16.097, 15.329, 14.603, 13.914, 13.262,
    12.644, 12.058, 11.502, 10.975, 10.474, 10.000, 9.549, 9.121, 8.714, 8.327,
    7.960, 7.611, 7.279, 6.964, 6.663, 6.377, 6.105, 5.846, 5.600, 5.365,
    5.141, 4.928, 4.725, 4.531, 4.346, 4.170, 4.001, 3.841, 3.688, 3.541,
    3.410, 3.268, 3.141, 3.019, 2.902, 2.791, 2.685, 2.583, 2.485, 2.392,
    2.303, 2.217, 2.135, 2.057, 1.982, 1.910, 1.841, 1.775, 1.712, 1.651,
    1.592, 1.537, 1.483, 1.431, 1.382, 1.334, 1.289, 1.245, 1.203, 1.162,
    1.123, 1.086, 1.050, 1.015, 0.982, 0.950, 0.919, 0.889, 0.860, 0.833,
    0.806, 0.780, 0.756, 0.732, 0.709, 0.687, 0.666, 0.645, 0.625, 0.606,
    0.588, 0.570, 0.552, 0.536, 0.520, 0.504
};
double get_ia(void)
{
    unsigned short i;
    unsigned long ia_sum = 0;
    for (i = 0; i < samp_sum; i++)
    {
        ia_sum += ((Sys_samp.AC_samp.ia_samp[i] - vref) * (Sys_samp.AC_samp.ia_samp[i] - vref));
    }
    ia_sum = ia_sum / samp_sum;
    Sys_samp.AC.ia = sqrt(ia_sum) * ia_k;
    ia_sum = 0;
    return Sys_samp.AC.ia;
}
double get_va(void)
{
    unsigned short i;
    unsigned long long va_sum = 0;
    for (i = 0; i < samp_sum; i++)
    {
        va_sum += ((Sys_samp.AC_samp.va_samp[i] - vref) * (Sys_samp.AC_samp.va_samp[i] - vref));
    }
    va_sum = va_sum / samp_sum;
    Sys_samp.AC.va = sqrt(va_sum) * va_k;
    va_sum = 0;
    return Sys_samp.AC.va;
}
double get_leakage_current(void)
{
    unsigned short i;
    for (i = 0; i < samp_sum; i++)
    {
        leakage_current_sum += (Sys_samp.AC_samp.leakage_current_samp[i] * Sys_samp.AC_samp.leakage_current_samp[i]);
    }
    leakage_current_sum = leakage_current_sum / samp_sum;
    Sys_samp.AC.leakage_current = sqrt(leakage_current_sum) * leakage_current_k;
    return Sys_samp.AC.leakage_current;
}
float get_CD4067(void)
{
    unsigned short i;
    for (i = 0; i < samp_dma; i++)
    {
        CD4067_sum += AD_samp_dma[i].CD4067;
    }
    Sys_samp.DC.CD4067 = (CD4067_sum / samp_dma); //*temper_k;
    CD4067_sum = 0;
    return Sys_samp.DC.CD4067;
}
uint8_t Get_State_relay(uint32_t relay_id)
{
    if (relay_id == 0)//L
    {
        if (pEVSE->info.ucTotalCON > 1)
        {
            return GET_RELAY1_STATE_1;
        }
        else
        {
            uint16_t i, j = 0;
            for (i = 0; i < 50; i++)
            {
                j += ((~HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9)) & 0x01);
                bsp_DelayUS(500);
            }
            if (j >= 3)
            {
                return 1; //on
            }  
            else
            {    
                return 0; //off
            }
        }
    }
    else if (relay_id == 1)//N
    {
        return GET_RELAY1_STATE_2;
    }
}
/********************************
*蜂鸣器状态控制函数
*state 0 关闭 1 开启
*无返回值
*********************************/
void Buzzer_control(uint8_t state)
{
    Chip2.buzzer = state & 0x01;
    write_pca9554_2();
}
float get_dc_massage(uint8_t DC_channel)
{
    uint16_t j;
    double ad_samp_value;
    float ad_value, re_value;
    float dc_data;

    if (xSemaphoreTake(xTempMutex, 1000) == pdPASS)
    {
        Chip1.a_select = DC_channel >> 0 & 0x01;
        Chip1.b_select = DC_channel >> 1 & 0x01;
        Chip1.c_select = DC_channel >> 2 & 0x01;
        Chip1.d_select = DC_channel >> 3 & 0x01;
        Chip1.cs1_select = 1;
        write_pca9554_1();
        vTaskDelay(25);
        ad_samp_value = get_CD4067();
        switch (DC_channel)
        {
        case 0:
            ad_value = (double)(ad_samp_value * 3) / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP1 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP1;
                }
            }
            //return dc_data;
            break;
        case 1:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP2 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP2;
                }
            }
            //return dc_data;
            break;
        case 2:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP3 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP3;
                }
            }
            //return dc_data;
            break;
        case 3:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP4 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP4;
                }
            }
            //return dc_data;
            break;
        case 4:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM1 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP_ARM1;
                }
            }
            //  return dc_data;
            break;
        case 5:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM2 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP_ARM2;
                }
            }
            //return dc_data;
            break;
        case 6:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM3 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP_ARM3;
                }
            }
            //return dc_data;
            break;
        case 7:
            ad_value = (double)ad_samp_value * 3 / 4096;
            re_value = (ad_value * 30) / (3 - ad_value);
            for (j = 0; j < 145; j++)
            {
                if ((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
                {
                    Sys_samp.DC.TEMP_ARM4 = j - 40;
                    dc_data = (float)Sys_samp.DC.TEMP_ARM4;
                }
            }

                    //return dc_data;
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            dc_data = ad_samp_value;
            //return dc_data;
            break;

        default :

            break;
        }
        xSemaphoreGive(xTempMutex);
        return dc_data;
    }
    else
    {
        return -1;
    }
}
void get_CP1(void)
{
    unsigned short i;
    float dma_cp1_sum = 0;
    for (i = 0; i < samp_dma; i++)
    {
        dma_cp1_sum += AD_samp_dma[i].CP1;
    }
    dma_cp1_sum = dma_cp1_sum / samp_dma;
    Sys_samp.DC.CP1 = dma_cp1_sum * CP1_k + 0.2;
    //return Sys_samp.DC.CP1;
}

double get_CP2(void)
{
    unsigned short i;
    float dma_cp2_sum = 0;
    for (i = 0; i < samp_dma; i++)
    {
        dma_cp2_sum += AD_samp_dma[i].CP2;
    }
    dma_cp2_sum = dma_cp2_sum / samp_dma;
     Sys_samp.DC.CP2 = dma_cp2_sum * CP2_k + 0.2;
}

void Delay_ms(unsigned long long time)
{
    //unsigned int x, y;
   // x = time * 1000;
   // for(; x > 0; x--)
    //    for(y = 180; y > 0; y--);
    vTaskDelay(time);
}
void Delay_us(unsigned long long time)
{
    unsigned int y;
    for (y = 190; y > 0; y--)
        ;
}
void Close_gun_1(void)
{
    A_KEY_ON;
    B_KEY_OFF;
    Delay_ms(lock_timer);
    A_KEY_OFF;
}
void Open_gun_1(void)
{
    A_KEY_OFF;
    B_KEY_ON;
    Delay_ms(lock_timer);
    B_KEY_OFF;
}
void Close_gun_2(void)
{
    A_KEY_ON;
    Delay_ms(10);
    B_KEY_ON;
    Delay_ms(lock_timer);
    A_KEY_OFF;
}
void Open_gun_2(void)
{
    A_KEY_OFF;
    Delay_ms(10);
    B_KEY_ON;
    Delay_ms(lock_timer);
    B_KEY_OFF;
}

void Power_out_l_pwm_ctrl(void)
{
    RELAY2_KEEP;
  /*  if(pwm_ms%2==0)
    {
        POWER_L_ON;
    }
    else
    {
        POWER_L_OFF;
    }*/
}
void Power_out_n_pwm_ctrl(void)
{
    RELAY1_KEEP;
   /* if(pwm_ms%2==0)
    {
        POWER_N_ON;
    }
    else
    {
        POWER_N_OFF;
    }*/
}

void POWER_N_CLOSE(void)
{
    RELAY1_ON;// POWER_N_ON;
    flag_power_out_n = 1;
    flag_pwm_out_n = 0;
    timer_relay_ms = 0;
}
void POWER_N_OPEN(void)
{
    flag_power_out_n = 0;
    RELAY1_OFF;// POWER_N_OFF;
    flag_pwm_out_n = 0;
}
void POWER_L_CLOSE(void)
{
    RELAY2_ON;//POWER_L_ON;
    flag_power_out_l = 1;
    flag_pwm_out_l = 0;
    timer_relay_ms = 0;
}
void POWER_L_OPEN(void)
{
    RELAY2_OFF;// 
    POWER_L_OFF;
    flag_power_out_l = 0;
    flag_pwm_out_l = 0;
}

void user_pwm_relay1_setvalue(uint16_t value)
{
  //  TIM_OC_InitTypeDef sConfigOC;
  //   sConfigOC.OCMode = TIM_OCMODE_PWM1;
//   sConfigOC.Pulse = value;
    //sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
   // sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  // HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2);
   // HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    TIM3->CCR2 = value;
}
void user_pwm_relay2_setvalue(uint16_t value)
{
  // TIM_OC_InitTypeDef sConfigOC;
   //sConfigOC.OCMode = TIM_OCMODE_PWM1;
   // sConfigOC.Pulse = value;
   //  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  // sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  // HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3);
  //  HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    TIM8->CCR3 = value;
}
void Peripheral_Init(void)
{
    value_reset();
    MX_GPIO_Init();
    IIC_Init();
    PCA9554_init();
    led_state_init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM2_Init();//CP1PWM
    MX_TIM3_Init();//1ºÅÇ¹PWMÆµÂÊ1K
    MX_TIM4_Init();//CP2PWM
    MX_TIM5_Init();//ÅäºÏA/D²ÉÑù¶¨Ê±Æ÷´¥·¢Ê±¼ä100¦ÌS
    MX_TIM8_Init();
    RS485_Init(9600);
    Lis2dh12_init();
    DMA_START();
    PWM1_ON;
    PWM2_ON;
    //PWM3_ON;
    //PWM4_ON;
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    TIMER5_ON;
    //Close_gun_1();
    //POWER_L_CLOSE();
    //POWER_N_CLOSE();
   PWM1_1000;
   PWM2_1000;
   //user_pwm_relay1_setvalue(1000);
  // user_pwm_relay2_setvalue(1000);
   // vref = Get_State_relay();
  // RELAY1_ON;
   // RELAY2_ON;
    //vref = Get_State_relay();
 //   vref=get_dc_massage(VREF_1v5);
//   Get_State_relay();
//         Get_Electricity_meter_massage_frequency();
    //Close_gun_1();
}

TIM_HandleTypeDef htim2;
extern DMA_HandleTypeDef hdma_adc1;

/**
* @brief This function handles DMA2 stream0 global interrupt.
*/
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */

  /* USER CODE END DMA2_Stream0_IRQn 0 */
    HAL_DMA_IRQHandler(&hdma_adc1);
    /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

      /* USER CODE END DMA2_Stream0_IRQn 1 */
}

void curr2pwm(uint32_t rate_curr, uint32_t con_id)
{
    uint32_t compare;
    double duty;
    //7kw duty = 53.3%, 40kw duty = 89.2%
    if (rate_curr <= 51 && rate_curr >= 6)
    { 
        duty = rate_curr / 0.6 ;
    }
    else if (rate_curr > 51 && rate_curr <= 63)
    {
        duty = rate_curr / 2.5 + 64;
    }
    else//默认
    {
        duty = 53.3;
    }
    compare = 1000 - duty * 10;
    if (con_id == 0)
    {
        TIM_SetTIM2Compare1(compare);//465=>7kw   108=>40kw
    }
    if (con_id == 1)
    {
        TIM_SetTIM4Compare1(compare);
    }
}


