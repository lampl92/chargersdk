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

/* private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
void Stm32_Clock_Init(uint32_t plln,uint32_t pllm,uint32_t pllp,uint32_t pllq);


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
    //Stm32_Clock_Init(360,25,2,8);
    SystemClock_Config(); //系统始终配置为180MHz
    //HAL_Delay(180);
    SystemCoreClockUpdate();    /* 根据PLL配置更新系统时钟频率变量 SystemCoreClock */

    bsp_InitUart();   /* 初始化串口 */
    //bsp_InitKey();        /* 初始化按键变量 */

    //bsp_InitExtIO();    /* FMC总线上扩展了32位输出IO, 操作LED等外设必须初始化 */
    //bsp_InitLed();        /* 初始LED指示灯端口 */
}

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
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

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
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
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

void bsp_Error_Handler(void)
{
    //do some alart
    while(1);
}

