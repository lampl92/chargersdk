#include "ade7953.h"
#include "bsp.h"
/************************************************

************************************************/
extern void Init_ADE7953_SPI(void);
extern void ConfigAde7953(void);
extern void ReadAde7953DataRegister(void);
extern void Read_ADE7953_SPI_TEST(void);
extern void setgain(void);
extern int get_ade7953(void);

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
