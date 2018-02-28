#include "bsp_timer.h"
#include "user_app.h"
#include "led_control.h"
#include "stm32f4xx_hal.h"

extern void Error_Handler(void);
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
uint16_t ref_timer;
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
    sConfigOC.Pulse = 1000;
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
    sConfigOC.Pulse = 465;
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
    if (pwm == compare)
    {
        ;
    }
    else
    {
        flat_pwm_change_cp2 = 1;
        TIM4->CCR1 = compare;
        pwm = compare;
    }
}
void TIM_SetTIM3Compare1(unsigned int compare)
{
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
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
    HAL_TIM_IRQHandler(&htim2);
    pwm_samp_timer = 0;
    pwm_samp_flag = 1;

    /* USER CODE BEGIN TIM2_IRQn 1 */

      /* USER CODE END TIM2_IRQn 1 */
}
//void TIM3_IRQHandler (void)//0.1ms
//{
  //yy_test=get_va();
 // RS485_Receive_Data(&RS485_RX_MODBUS,&RS485_RX_MODBUS_CNT);

 // HAL_TIM_IRQHandler(&htim6);

//}
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
    HAL_TIM_IRQHandler(&htim4);
    pwm_samp_timer_cp2 = 0;
    pwm_samp_flag_cp2 = 1;
    /* USER CODE BEGIN TIM4_IRQn 1 */

      /* USER CODE END TIM4_IRQn 1 */
}
void TIM5_IRQHandler(void)//100¦ÌS½øÈëÒ»´Î
{
  /* USER CODE BEGIN TIM5_IRQn 0 */
  /* USER CODE END TIM5_IRQn 0 */
    HAL_TIM_IRQHandler(&htim5);
    delay_ms++;
    pwm_ms++;
    pwm_r_1++;
    pwm_r_2++;
    pwm_g_1++;
    pwm_g_2++;
    pwm_b_1++;
    pwm_b_2++;
    timer_ms++;
    delay_breath++;
    pwm_samp_timer++;
    pwm_samp_timer_cp2++;
    // ref_timer = Get_State_relay();
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
    if ((pwm_samp_flag == 1)&&(pwm_samp_timer >= 3))
    {
        RUN_ON;
        get_CP1();
        pwm_samp_timer = 0;
        pwm_samp_flag = 0;

    }
    if ((pwm_samp_flag_cp2 == 1)&&(pwm_samp_timer_cp2 >= 3))
    {
        get_CP2();
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
//Open_gun_1();
        timer_relay_ms = 0;
    }
    if (timer_ms >= 1000)
    {
        timer_ms = 0;
        timer_s++;
    }
    if (timer_s >= 60)
    {
        timer_s = 0;
        timer_min++;
    }
    if (flag_pwm_out_n == 1)
    {
        Power_out_n_pwm_ctrl();
    }
    if (flag_pwm_out_l == 1)
    {
        Power_out_l_pwm_ctrl();
    }
    if (pwm_ms >= 200)
    {
        pwm_ms = 0;
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
#ifndef EVSE_DEBUG
    led_output();
#endif
  //get_samp_point();
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}