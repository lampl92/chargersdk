#include "bsp_timer.h"
unsigned int pwm, pwm4;
unsigned int delay_ms;
TIM_HandleTypeDef TIM3_Handler;      //定时器句柄
TIM_HandleTypeDef TIM2_Handler;         //定时器3PWM句柄
TIM_OC_InitTypeDef TIM2_CH1Handler;     //定时器3通道4句柄
TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
extern void Error_Handler(void);
extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim_base);
extern void get_samp_point(void);
unsigned int delay_ms,pwm_ms,pwm_r_1,pwm_r_2,pwm_g_1,pwm_g_2,
	pwm_b_1,pwm_b_2,duty_ratio_r_1,duty_ratio_r_2,duty_ratio_g_1,
  duty_ratio_g_2,duty_ratio_b_1,duty_ratio_b_2;
uint8_t flag_power_pwm_on,flag_pwm_r_on_1,flag_pwm_r_on_2,flag_pwm_g_on_1,
flag_pwm_g_on_2,flag_pwm_b_on_1,flag_pwm_b_on_2;

void MX_TIM2_Init(void)
{

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;
    TIM_OC_InitTypeDef sConfigOC;

    htim2.Instance = TIM2;
    htim2.Init.Prescaler =96;
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
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim2);

}
void MX_TIM3_Init(void)
{

    TIM_ClockConfigTypeDef sClockSourceConfig;
    TIM_MasterConfigTypeDef sMasterConfig;

    htim3.Instance = TIM3;
    htim3.Init.Prescaler =192;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 10000;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }

    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }

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
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
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
    htim5.Init.Prescaler = 192;
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

void TIM_SetTIM2Compare1(unsigned int compare)
{
    if(pwm == compare)
    {
        ;
    }
    else
    {
        TIM2->CCR1 = compare;
        pwm = compare;
    }
}


void TIM_SetTIM4Compare1(unsigned int compare)
{
    if(pwm == compare)
    {
        ;
    }
    else
    {
        TIM4->CCR1 = compare;
        pwm = compare;
    }
}


void TIM2_IRQHandler (void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}
void TIM3_IRQHandler (void)
{
  HAL_TIM_IRQHandler(&htim3);
	delay_ms++;
	pwm_ms++;
	pwm_r_1++;
	pwm_r_2++;
	pwm_g_1++;
	pwm_g_2++;
	pwm_b_1++;
	pwm_b_2++;
	if(pwm_r_1>=100)
	{
		pwm_r_1=0;
	}
	if(pwm_r_2>=100)
	{
		pwm_r_2=0;
	}
	if(pwm_g_1>=100)
	{
		pwm_g_1=0;
	}
	if(pwm_g_2>=100)
	{
		pwm_g_2=0;
	}
		if(pwm_b_1>=100)
	{
		pwm_b_1=0;
	}
	if(pwm_b_2>=100)
	{
		pwm_b_2=0;
	}
	if(flag_pwm_r_on_1)
	{
	if(pwm_r_1>=duty_ratio_r_1)
	{
		LED1_R_RUN;
	}
	else
	{
		LED1_R_OFF;
	}
 }
		if(flag_pwm_g_on_1)
	{
	if(pwm_g_1>=duty_ratio_g_1)
	{
		LED1_G_RUN;
	}
	else
	{
		LED1_G_OFF;
	}
 }

		if(flag_pwm_b_on_1)
	{
	if(pwm_b_1>=duty_ratio_b_1)
	{
		LED1_B_RUN;
	}
	else
	{
		LED1_B_OFF;
	}
 }

 if(flag_pwm_r_on_2)
	{
	if(pwm_r_2>=duty_ratio_r_2)
	{
		LED2_R_RUN;
	}
	else
	{
		LED2_R_OFF;
	}
 }
		if(flag_pwm_g_on_2)
	{
	if(pwm_g_2>=duty_ratio_g_2)
	{
		LED2_G_RUN;
	}
	else
	{
		LED2_G_OFF;
	}
 }

		if(flag_pwm_b_on_2)
	{
	if(pwm_b_2>=duty_ratio_b_2)
	{
		LED2_B_RUN;
	}
	else
	{
		LED2_B_OFF;
	}
 }

	if(flag_power_pwm_on)
	{
	if(pwm_ms/2==0)
	{
		POWER_L_ON;
		POWER_N_ON;
	}
	  else
	{
		POWER_L_OFF;
		POWER_N_OFF;
	}
 }
}
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */

  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}
void TIM5_IRQHandler(void)//100¦ÌS½øÈëÒ»´Î
{
  /* USER CODE BEGIN TIM5_IRQn 0 */

  /* USER CODE END TIM5_IRQn 0 */
  HAL_TIM_IRQHandler(&htim5);
	get_samp_point();
  /* USER CODE BEGIN TIM5_IRQn 1 */

  /* USER CODE END TIM5_IRQn 1 */
}
