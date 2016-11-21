#include "includes.h"

//?-����??��?
#define WHITE            0xFFFF
#define BLACK            0x0000
#define BLUE             0x001F
#define BRED             0XF81F
#define GRED             0XFFE0
#define GBLUE            0X07FF
#define RED              0xF800
#define MAGENTA          0xF81F
#define GREEN            0x07E0
#define CYAN             0x7FFF
#define YELLOW           0xFFE0
#define BROWN            0XBC40 //��?��?
#define BRRED            0XFC07 //��?o����?
#define GRAY             0X8430 //?����?
//GUI??��?

#define DARKBLUE         0X01CF //��?��?��?
#define LIGHTBLUE        0X7D7C //?3��?��?  
#define GRAYBLUE         0X5458 //?����?��?
//��?��?��y��??aPANEL��???��?

#define LIGHTGREEN       0X841F //?3?����?
//#define LIGHTGRAY        0XEF5B //?3?����?(PANNEL)
#define LGRAY            0XC618 //?3?����?(PANNEL),��㨬?��3?�㨦?

#define LGRAYBLUE        0XA651 //?3?����?��?(?D??2???��?)
#define LBBLUE           0X2B12 //?3��?��?��?(????��???��?���䨦?)


void cli_hello_fnt(int argc, char **argv)
{
    printf("\nhello world\n");
    printf("HCLK = SYSCLK = %dMHz\n", SystemCoreClock / 1000000);
    printf("AHB  = SYSCLK / DIV1 = %dMHz\n", SystemCoreClock / 1000000 / 1);
    printf("APB1 = SYSCLK / DIV4 = %dMHz\n", SystemCoreClock / 1000000 / 4);
    printf("APB2 = SYSCLK / DIV2 = %dMHz\n", SystemCoreClock / 1000000 / 2);
    RTC_Set_Time(10, 24, 0, RTC_HOURFORMAT12_PM);       //����ʱ�� ,����ʵ��ʱ���޸�
    RTC_Set_Date(16, 11, 21,1);                        //��������

#if 0
    for(i = 0; i < 12; i++)
    {
        switch(i)
        {
            case 0:
                LTDC_Clear(WHITE);
                break;
            case 1:
                LTDC_Clear(BLACK);
                break;
            case 2:
                LTDC_Clear(BLUE);
                break;
            case 3:
                LTDC_Clear(RED);
                break;
            case 4:
                LTDC_Clear(MAGENTA);
                break;
            case 5:
                LTDC_Clear(GREEN);
                break;
            case 6:
                LTDC_Clear(CYAN);
                break;
            case 7:
                LTDC_Clear(YELLOW);
                break;
            case 8:
                LTDC_Clear(BRRED);
                break;
            case 9:
                LTDC_Clear(GRAY);
                break;
            case 10:
                LTDC_Clear(LGRAY);
                break;
            case 11:
                LTDC_Clear(BROWN);
                break;
        }
        bsp_DelayMS(500);
    }
#endif
}

tinysh_cmd_t cli_hello_cmd =
{
    0,
    "hello",
    "display hello",
    0, cli_hello_fnt,
    "<cr>", 0, 0
};



//SDRAM�ڴ����
void fsmc_sdram_test()
{
    u32 i = 0;
    u32 temp = 0;
    u32 sval = 0; //�ڵ�ַ0����������
    printf("ÿ��16K�ֽ�,д��һ������,�ܹ�д��2048������,�պ���32M�ֽ�\n");
    for(i = 0; i < 32 * 1024 * 1024; i += 16 * 1024)
    {
        *(vu32 *)(Bank5_SDRAM_ADDR + i) = temp;
        temp++;
    }
    temp = 0;
    //���ζ���֮ǰд�������,����У��
    for(i = 0; i < 32 * 1024 * 1024; i += 16 * 1024)
    {
        sval = *(vu32 *)(Bank5_SDRAM_ADDR + i);
        if(temp == sval)
        {
            printf("[@0x%x]=OK!\n", Bank5_SDRAM_ADDR | i);
        }
        else
        {
            printf("[@0x%x]=ERR!\n", Bank5_SDRAM_ADDR | i);
            break;
        }
        temp++;
    }
    printf("SDRAM ����:%dMB\r\n", i / 1024 / 1024); //��ӡSDRAM����
}


void cli_testsdram_fnt(int argc, char **argv)
{
    taskENTER_CRITICAL();
    fsmc_sdram_test();
    taskEXIT_CRITICAL();
}

tinysh_cmd_t cli_testsdram_cmd =
{
    0,
    "testsdram",
    "test sdram",
    0, cli_testsdram_fnt,
    "<cr>", 0, 0
};

void cli_systemtime_fnt(int argc, char **argv)
{
    u8 tbuf[40];
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    taskENTER_CRITICAL();
    
    HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
    sprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
    printf("%s\n", tbuf);
    HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
    sprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
    printf("%s\n", tbuf); //LCD_ShowString(30, 160, 210, 16, 16, tbuf);
    sprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
    printf("%s\n", tbuf); //LCD_ShowString(30, 180, 210, 16, 16, tbuf);


    taskEXIT_CRITICAL();
}

tinysh_cmd_t cli_systemdate_cmd =
{
    0,
    "date",
    "show system date",
    0, cli_systemtime_fnt,
    "<cr>", 0, 0
};
