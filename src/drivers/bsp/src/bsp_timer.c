#include "bsp_timer.h"
#include "user_app.h"
#include "led_control.h"
#include "stm32f4xx_hal.h"

extern void Error_Handler(void);
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
static uint8_t pwm_samp_timer, pwm_samp_flag, pwm_samp_timer_cp2, pwm_samp_flag_cp2;
static uint16_t flat_pwm_change, flat_pwm_change_cp2;
uint16_t timer_relay_ms;
extern uint16_t pwm_r_1, pwm_g_1, pwm_b_1, pwm_r_2, pwm_g_2, pwm_b_2;
TIM_HandleTypeDef TIM3_Handler;
TIM_HandleTypeDef TIM3_Handler;
TIM_HandleTypeDef TIM2_Handler;
TIM_OC_InitTypeDef TIM2_CH1Handler;
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;
void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 96;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 1000;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim2);
}
void MX_TIM3_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 96;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 1000;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim3);
}
void MX_TIM4_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 96;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 1000;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim4);
}
void MX_TIM5_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim5.Instance = TIM5;
    htim5.Init.Prescaler = 96;
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 100;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}
void MX_TIM8_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC;
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig;

    htim8.Instance = TIM8;
    htim8.Init.Prescaler = 96;
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.Period = 1000;
    htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim8.Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
    {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
    {
        Error_Handler();
    }

    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim8);
}
void TIM_SetTIM2Compare1(unsigned int compare)
{
    static uint16_t pwm;
    if (pwm == compare)
    {
        ;
    }
    else
    {
        flat_pwm_change = 1;
        TIM2->CCR1 = compare;
        pwm = compare;
    }
}

void TIM_SetTIM4Compare1(unsigned int compare)
{
    static uint16_t pwm4;
    if (pwm4 == compare)
    {
        ;
    }
    else
    {
        flat_pwm_change_cp2 = 1;
        TIM4->CCR2 = compare;
        pwm4 = compare;
    }
}
void TIM_SetTIM3Compare1(unsigned int compare)
{
    static uint16_t pwm_relay2;
    if (pwm_relay2 == compare)
    {
        ;
    }
    else
    {
        TIM3->CCR1 = compare;
        pwm_relay2 = compare;
    }
}
void TIM_SetTIM8Compare1(unsigned int compare)
{
    static uint16_t pwm_relay1;
    if (pwm_relay1 == compare)
    {
        ;
    }
    else
    {
        TIM8->CCR1 = compare;
        pwm_relay1 = compare;
    }
}
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
    pwm_samp_timer = 0;
    pwm_samp_flag = 1;
}

void TIM4_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim4);
    pwm_samp_timer_cp2 = 0;
    pwm_samp_flag_cp2 = 1;
}
void TIM5_IRQHandler(void)
{
    static uint16_t delay_breath;

    HAL_TIM_IRQHandler(&htim5);
    pwm_r_1++;
    pwm_r_2++;
    pwm_g_1++;
    pwm_g_2++;
    pwm_b_1++;
    pwm_b_2++;
    delay_breath++;
    pwm_samp_timer++;
    pwm_samp_timer_cp2++;
    if (flat_pwm_change == 1)
    {
        pwm_samp_timer = 0;
        flat_pwm_change = 0;
    }
    if (flat_pwm_change_cp2 == 1)
    {
        pwm_samp_timer_cp2 = 0;
        flat_pwm_change_cp2 = 0;
    }
    if ((pwm_samp_flag == 1) && (pwm_samp_timer >= 2))
    {
        RUN_ON;
        calc_CP1();
        pwm_samp_timer = 0;
        pwm_samp_flag = 0;
    }
    if ((pwm_samp_flag_cp2 == 1) && (pwm_samp_timer_cp2 >= 2))
    {
        calc_CP2();
        pwm_samp_timer_cp2 = 0;
        pwm_samp_flag_cp2 = 0;
        RUN_OFF;
    }
    if (delay_breath >= 200)
    {
        led_breath_r();
        led_breath_g();
        led_breath_b();
        delay_breath = 0;
    }
    timer_relay_ms++;
    if (timer_relay_ms >= 10000)
    {
        if (flag_power_out_l == 1)
        {
            flag_pwm_out_l = 1;
        }
        if (flag_power_out_n == 1)
        {
            flag_pwm_out_n = 1;
        }
        timer_relay_ms = 0;
    }
    if (flag_pwm_out_n == 1)
    {
        Power_out_n_pwm_ctrl();
    }
    if (flag_pwm_out_l == 1)
    {
        Power_out_l_pwm_ctrl();
    }
    if (pwm_r_1 >= 100)
    {
        pwm_r_1 = 0;
    }
    if (pwm_r_2 >= 100)
    {
        pwm_r_2 = 0;
    }
    if (pwm_g_1 >= 100)
    {
        pwm_g_1 = 0;
    }
    if (pwm_g_2 >= 100)
    {
        pwm_g_2 = 0;
    }
    if (pwm_b_1 >= 100)
    {
        pwm_b_1 = 0;
    }
    if (pwm_b_2 >= 100)
    {
        pwm_b_2 = 0;
    }
#ifndef EVSE_DEVBOARD
    led_output();
#endif
}
