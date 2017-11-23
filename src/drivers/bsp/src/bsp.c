/**
* @file bsp.c
* @brief 这是硬件底层驱动程序的主文件。每个c文件可以 #include "bsp.h" 来包含
*        所有的外设驱动模块。
* @author rgw
* @version v1.0
* @date 2016-10-28
*/
#include "includes.h"
#include "bsp.h"

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow :
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 25
  *            PLL_N                          = 360
  *            PLL_P                          = 2
  *            PLL_Q                          = 8
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  *
  *            Fvco=Fs*(plln/pllm);
  *            SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
  *            Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));
  *
  *            Fvco:VCO频率
  *            SYSCLK:系统时钟频率
  *            Fusb:USB,SDIO,RNG等的时钟频率
  *            Fs:PLL输入时钟频率,可以是HSI,HSE等.
  *            plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
  *            pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
  *            pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
  *            pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
  *            外部晶振为25M的时候,推荐值:plln=360,pllm=25,pllp=2,pllq=8.
  *            得到:Fvco=25*(360/25)=360Mhz
  *                 SYSCLK=360/2=180Mhz
  *                 Fusb=360/8=45Mhz
  *
  *       *** 如果要使用USB,N要设置为384, Fusb = 48Mhz ***
  *
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    uint32_t pllm = 25;
    uint32_t plln = 336;//plln = 384时,sysclk = 192Mhz, plln = 360,180Mhz,
    uint32_t pllp = RCC_PLLP_DIV2;
    uint32_t pllq = 8;

    /* Enable Power Control clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* The voltage scaling allows optimizing the power consumption when the device is
       clocked below the maximum system frequency, to update the voltage scaling value
       regarding system frequency refer to product datasheet.  */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /* Enable HSE Oscillator and activate PLL with HSE as source */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = pllm;
    RCC_OscInitStruct.PLL.PLLN = plln;
    RCC_OscInitStruct.PLL.PLLP = pllp;
    RCC_OscInitStruct.PLL.PLLQ = pllq;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        bsp_Error_Handler();
    }

    if(HAL_PWREx_EnableOverDrive() != HAL_OK)//开启Over-Driver功能,使主频能够达到180MHz,否则只能到168MHz
    {
        /* Initialization Error */
        bsp_Error_Handler();
    }

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
       clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)//同时设置FLASH延时周期为5WS，也就是6个CPU周期。
    {
        /* Initialization Error */
        bsp_Error_Handler();
    }
}

/* ---------------------------------------------------------------------------*/
/**
* @brief 初始化所有的硬件设备。该函数配置CPU寄存器和外设的寄存器并初始化一些
*        全局变量。只需要调用一次
*/
/* ---------------------------------------------------------------------------*/
void bsp_Init(void)
{
    /*
        由于ST固件库的启动文件已经执行了CPU系统时钟的初始化，所以不必再次重复配置系统时钟。
        启动文件配置了CPU主时钟频率、内部Flash访问速度和可选的外部SRAM FSMC初始化。

    */
    /* 优先级分组设置为4，可配置0-15级抢占式优先级，0级子优先级，即不存在子优先级。*/

    HAL_Init();
    SystemClock_Config(); //系统始终配置为168MHz
    SystemCoreClockUpdate();    /* 根据PLL配置更新系统时钟频率变量 SystemCoreClock */
    /* Enable the CRC Module */
    __HAL_RCC_CRC_CLK_ENABLE(); //
#ifdef EVSE_DEBUG
    bsp_GPIO_Init();
#endif
    bsp_RTC_Init();
    RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0); //配置 WAKE UP 中断,1 秒钟中断一次
    bsp_DWT_Init();
#ifndef DEBUG_DIAG_DUMMY
    Peripheral_Init();
#endif
    bsp_SDRAM_Init();

#if EVSE_USING_GUI
    LCD_Init();
    TP_Init();
#endif
    //bsp_LTDC_Init();//在GUI中初始化
//    bsp_Touch_Init();
    bsp_Uart_Init(UART_PORT_CLI, 1);   /* 初始化串口 */
    bsp_Uart_Init(UART_PORT_RFID, 1);
    bsp_Uart_Init(UART_PORT_GPRS, 1);
#ifndef EVSE_DEBUG
    //IWDG_Init(IWDG_PRESCALER_64,500);  	
#endif
}

void bsp_Error_Handler(void)
{
    //do some alart
    while(1);
}
