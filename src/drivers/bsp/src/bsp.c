/**
* @file bsp.c
* @brief ����Ӳ���ײ�������������ļ���ÿ��c�ļ����� #include "bsp.h" ������
*        ���е���������ģ�顣
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
* @brief ��ʼ�����е�Ӳ���豸���ú�������CPU�Ĵ���������ļĴ�������ʼ��һЩ
*        ȫ�ֱ�����ֻ��Ҫ����һ��
*/
/* ---------------------------------------------------------------------------*/
void bsp_Init(void)
{
    /*
        ����ST�̼���������ļ��Ѿ�ִ����CPUϵͳʱ�ӵĳ�ʼ�������Բ����ٴ��ظ�����ϵͳʱ�ӡ�
        �����ļ�������CPU��ʱ��Ƶ�ʡ��ڲ�Flash�����ٶȺͿ�ѡ���ⲿSRAM FSMC��ʼ����
        
    */
    /* ���ȼ���������Ϊ4��������0-15����ռʽ���ȼ���0�������ȼ����������������ȼ���*/
    
    HAL_Init();
    SystemClock_Config(); //ϵͳʼ������Ϊ180MHz
    SystemCoreClockUpdate();    /* ����PLL���ø���ϵͳʱ��Ƶ�ʱ��� SystemCoreClock */
    /* Enable the CRC Module */
    __HAL_RCC_CRC_CLK_ENABLE();	//
    bsp_RTC_Init();
    bsp_DWT_Init();
    bsp_Uart_Init();   /* ��ʼ������ */
    bsp_SDRAM_Init();
    //FTL_Init();��fatfs�г�ʼ��
    //bsp_LTDC_Init();//��GUI�г�ʼ��
    
    
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
  *            Fvco:VCOƵ��
  *            SYSCLK:ϵͳʱ��Ƶ��
  *            Fusb:USB,SDIO,RNG�ȵ�ʱ��Ƶ��
  *            Fs:PLL����ʱ��Ƶ��,������HSI,HSE��. 
  *            plln:��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:64~432.
  *            pllm:��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
  *            pllp:ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
  *            pllq:USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.
  *            �ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=360,pllm=25,pllp=2,pllq=8.
  *            �õ�:Fvco=25*(360/25)=360Mhz
  *                 SYSCLK=360/2=180Mhz
  *                 Fusb=360/8=45Mhz
  *
  *       *** ���Ҫʹ��USB,NҪ����Ϊ384, Fusb = 48Mhz ***
  *
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

    uint32_t pllm = 25;
    uint32_t plln = 384;//plln = 384ʱ,sysclk = 192Mhz, plln = 360,180Mhz,  
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
  
  if(HAL_PWREx_EnableOverDrive() != HAL_OK)//����Over-Driver����,ʹ��Ƶ�ܹ��ﵽ180MHz,����ֻ�ܵ�168MHz
  {
    /* Initialization Error */
    bsp_Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//����ϵͳʱ��ʱ��ԴΪPLL
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;  
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)//ͬʱ����FLASH��ʱ����Ϊ5WS��Ҳ����6��CPU���ڡ�
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

