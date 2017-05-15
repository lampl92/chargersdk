#include "stm32f4xx_hal.h"
#include "bsp.h"
#include <stdlib.h>
#include <math.h>
#include "user_app.h"
#include "bsp_timer.h"
#include "electric_energy_meter.h"
#include "FreeRTOS.h"
float frequency_test;
const double  resistance[154] =
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
    for(i = 0; i < samp_sum; i++)
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
    for(i = 0; i < samp_sum; i++)
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
    for(i = 0; i < samp_sum; i++)
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
    for(i = 0; i < samp_sum; i++)
    {
        CD4067_sum += Sys_samp.DC_samp.CD4067[i];
    }
    Sys_samp.DC.CD4067 = (CD4067_sum / samp_sum); //*temper_k;
    return Sys_samp.DC.CD4067;
}
float get_dc_massage(uint8_t DC_channel)
{
    uint16_t j;
    double ad_samp_value;
    float ad_value, re_value;
    float dc_data;


    Chip1.a_select = DC_channel >> 0 & 0x01;
    Chip1.b_select = DC_channel >> 1 & 0x01;
    Chip1.c_select = DC_channel >> 2 & 0x01;
    Chip1.d_select = DC_channel >> 3 & 0x01;
    Chip1.cs1_select = 1;
    write_pca9554_1();
    vTaskDelay(25);
    ad_samp_value =get_CD4067();;
    switch (DC_channel)
    {
    case 0:
        ad_value = (double)(ad_samp_value * 3) / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP1 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP1;
            }
        }
        //return dc_data;
        break;
    case 1:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP2 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP2;
            }
        }
        //return dc_data;
        break;
    case 2:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP3 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP3;
            }
        }
        //return dc_data;
        break;
    case 3:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP4 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP4;
            }
        }
        //return dc_data;
        break;
    case 4:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP_ARM1 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP_ARM1;
            }
        }
        //  return dc_data;
        break;
    case 5:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP_ARM2 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP_ARM2;
            }
        }
        //return dc_data;
        break;
    case 6:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP_ARM3 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP_ARM3;
            }
        }
        //return dc_data;
        break;
    case 7:
        ad_value = (double)ad_samp_value * 3 / 4096;
        re_value = (ad_value * 30) / (3 - ad_value);
        for(j = 0; j <= 150; j++)
        {
            if((re_value >= (resistance[j + 1])) && (re_value < (resistance[j])))
            {
                Sys_samp.DC.TEMP_ARM4 = j - 40;
                dc_data=(float)Sys_samp.DC.TEMP_ARM4;
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
    return dc_data;

}
double get_CP1(void)
{
    unsigned short i;
    for(i = 0; i < samp_sum; i++)
    {
        if(Sys_samp.DC_samp.CP1[i] >= 1000)
        {
            CP1_sum_sys += Sys_samp.DC_samp.CP1[i];
            num_cp1++;
        }
    }
    if(num_cp1 <= 20)
    {
        Sys_samp.DC.CP1 = 0;
    }
    else
    {
        Sys_samp.DC.CP1 = (CP1_sum_sys * CP1_k) / num_cp1 + 0.32;
    }
    CP1_sum_sys = 0;
    num_cp1 = 0;
    return Sys_samp.DC.CP1;


}

double get_CP2(void)
{
    unsigned short i;

    for(i = 0; i < samp_sum; i++)
    {
        if(Sys_samp.DC_samp.CP2[i] >= 680)
        {
            CP2_sum_sys += Sys_samp.DC_samp.CP2[i];
            num_cp2++;
        }
    }
    if(num_cp2 <= 20)
    {
        Sys_samp.DC.CP2 = 0;

    }
    else
    {
        Sys_samp.DC.CP2 = (CP2_sum_sys * CP2_k) / num_cp2 + 0.32;
    }
    CP2_sum_sys = 0;
    num_cp2 = 0;
    return Sys_samp.DC.CP2;
}
void get_samp_point(void)//ÓÃÊ±30¦ÌS
{

    unsigned short i, j;
//    RUN_ON;
    for(i = 0; i < samp_dma; i++)
    {
        CD4067_sum += AD_samp_dma[i].CD4067;
        ia_samp_sum += AD_samp_dma[i].ia_samp;
        va_samp_sum += AD_samp_dma[i].va_samp;
        leakage_current_sum += AD_samp_dma[i].leakage_current;
        CP1_sum += AD_samp_dma[1].CP1;
        CP2_sum += AD_samp_dma[1].CP2;
    }
    for(j = 0; j < samp_sum - 1; j++)
    {
        Sys_samp.AC_samp.ia_samp[j] = Sys_samp.AC_samp.ia_samp[j + 1];
        Sys_samp.AC_samp.va_samp[j] = Sys_samp.AC_samp.va_samp[j + 1];
        Sys_samp.AC_samp.leakage_current_samp[j] = Sys_samp.AC_samp.leakage_current_samp[j + 1];
        Sys_samp.DC_samp.CD4067[j] = Sys_samp.DC_samp.CD4067[j + 1];
        Sys_samp.DC_samp.CP1[j] = Sys_samp.DC_samp.CP1[j + 1];
        Sys_samp.DC_samp.CP2[j] = Sys_samp.DC_samp.CP2[j + 1];
    }
    Sys_samp.AC_samp.ia_samp[samp_sum - 1] = ia_samp_sum / samp_dma;
    Sys_samp.AC_samp.va_samp[samp_sum - 1] = va_samp_sum / samp_dma;
    Sys_samp.AC_samp.leakage_current_samp[samp_sum - 1] = leakage_current_sum / samp_dma;
    Sys_samp.DC_samp.CD4067[samp_sum - 1] = CD4067_sum / samp_dma;
    Sys_samp.DC_samp.CP1[samp_sum - 1] = CP1_sum / samp_dma;
    Sys_samp.DC_samp.CP2[samp_sum - 1] = CP2_sum / samp_dma;
    CD4067_sum = 0;
    leakage_current_sum = 0;
    va_samp_sum = 0;
    ia_samp_sum = 0;
    CP2_sum = 0;
    CP1_sum = 0;
    //RUN_OFF;
}
void Delay_ms(unsigned long long time)
{
//    unsigned int x, y;
//    x = time * 1000;
//    for(; x > 0; x--)
//        for(y = 180; y > 0; y--);
    vTaskDelay(time);
}
void Delay_us(unsigned long long time)
{
    unsigned int y;
    for(y = 190; y > 0; y--);
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
    if(pwm_ms%2==0)
    {
        POWER_L_ON;
    }
    else
    {
        POWER_L_OFF;
    }
}
void Power_out_n_pwm_ctrl(void)
{
    if(pwm_ms%2==0)
    {
        POWER_N_ON;
    }
    else
    {
        POWER_N_OFF;
    }
}
void POWER_L_CLOSE(void)
{
    POWER_L_ON;
    flag_power_out_l=1;
    flag_pwm_out_l=0;
    timer_relay_ms=0;
}
void POWER_N_CLOSE(void)
{
    POWER_N_ON;
    flag_power_out_n=1;
    flag_pwm_out_n=0;
    timer_relay_ms=0;
}
void POWER_L_OPEN(void)
{
    flag_power_out_l=0;
    POWER_L_OFF;
    flag_pwm_out_l=0;
}
void POWER_N_OPEN(void)
{
    flag_power_out_n=0;
    POWER_N_OFF;
    flag_pwm_out_n=0;
}
uint8_t Get_State_relay()
{
    uint8_t relay_num,i,j;
    relay_num=0;
    for (i=0; i<100; i++)
    {
        j=read_pca9554_2()>>1;
        relay_num+=(j&0x01);
    }
    if(relay_num==100)
    {
        return 0;//未连接
    }
    else
    {
        return 1;//已经连接
    }

}
void Peripheral_Init(void)
{

    MX_GPIO_Init();
    IIC_Init();
    PCA9554_init();
    led_state_init();
    MX_DMA_Init();
    MX_ADC1_Init();
    MX_TIM2_Init();//1ºÅÇ¹PWMÆµÂÊ1K
    MX_TIM3_Init();//ÏµÍ³¶¨Ê±Æ÷100uS
    MX_TIM4_Init();//2ºÅÇ¹PWMÆµÂÊ1K
    MX_TIM5_Init();//ÅäºÏA/D²ÉÑù¶¨Ê±Æ÷´¥·¢Ê±¼ä100¦ÌS
    RS485_Init(9600);
    Lis2dh12_init();
    DMA_START();

    //POWER_L_CLOSE();
    //POWER_N_CLOSE();
    vref = 2045;
    //vref=get_dc_massage(VREF_1v5);
//   Get_State_relay();
//
//         Get_Electricity_meter_massage_frequency();
    //Close_gun_1();
    PWM1_ON;
    PWM2_ON;
    TIMER3_ON;
    TIMER5_ON;
}
