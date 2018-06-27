#include "ade7953.h"
#include "bsp.h"
/************************************************

************************************************/
extern void Init_ADE7953_SPI(void);
extern void ConfigAde7953(void);
extern void ReadAde7953DataRegister(void);
extern void Read_ADE7953_SPI_TEST(void);
extern void setgain(void);
extern void delay_ms(u16 nms);
extern int get_ade7953(void);
#define  DWT_CYCCNT  *(volatile unsigned int *)0xE0001004





void DelayUS(uint32_t _ulDelayTime)
{
  uint32_t tCnt, tDelayCnt;
	uint32_t tStart;
		
	tStart = DWT_CYCCNT;                                     /* 刚进入时的计数器值 */
	tCnt = 0;
	tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);	 /* 需要的节拍数 */ 		

	while(tCnt < tDelayCnt)
	{
		tCnt = DWT_CYCCNT - tStart; /* 求减过程中，如果发生第一次32位计数器重新计数，依然可以正确计算 */	
	}
}



int get_ade7953(void)
{
  //  HAL_Init();
  //  SystemClock_Config(); //系统始终配置为192MHz
   // SystemCoreClockUpdate();
	 Init_ADE7953_SPI();
	setgain();
	bsp_DelayMS(2000);
 // ConfigAde7953();
	//while (1)
	//{
	 //LED_OFF;
	// Main_Delay(100);	
 ReadAde7953DataRegister();
	
//Read_ADE7953_SPI_TEST();
	//Main_Delay(100);
//	}
	 
}
 

 



