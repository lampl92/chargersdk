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
		
	tStart = DWT_CYCCNT;                                     /* �ս���ʱ�ļ�����ֵ */
	tCnt = 0;
	tDelayCnt = _ulDelayTime * (SystemCoreClock / 1000000);	 /* ��Ҫ�Ľ����� */ 		

	while(tCnt < tDelayCnt)
	{
		tCnt = DWT_CYCCNT - tStart; /* ��������У����������һ��32λ���������¼�������Ȼ������ȷ���� */	
	}
}



int get_ade7953(void)
{
  //  HAL_Init();
  //  SystemClock_Config(); //ϵͳʼ������Ϊ192MHz
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
 

 



