#include "includes.h"

//?-±ê??é?
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
#define BROWN            0XBC40 //×?é?
#define BRRED            0XFC07 //×?oìé?
#define GRAY             0X8430 //?òé?
//GUI??é?

#define DARKBLUE         0X01CF //é?à?é?
#define LIGHTBLUE        0X7D7C //?3à?é?  
#define GRAYBLUE         0X5458 //?òà?é?
//ò?é?èyé??aPANELμ???é?

#define LIGHTGREEN       0X841F //?3?ìé?
//#define LIGHTGRAY        0XEF5B //?3?òé?(PANNEL)
#define LGRAY            0XC618 //?3?òé?(PANNEL),′°ì?±3?°é?

#define LGRAYBLUE        0XA651 //?3?òà?é?(?D??2???é?)
#define LBBLUE           0X2B12 //?3×?à?é?(????ì???μ?·′é?)


void cli_hello_fnt(int argc, char **argv)
{
    printf("\nhello world\n");
    printf("HCLK = SYSCLK = %dMHz\n", SystemCoreClock / 1000000);
    printf("AHB  = SYSCLK / DIV1 = %dMHz\n", SystemCoreClock / 1000000 / 1);
    printf("APB1 = SYSCLK / DIV4 = %dMHz\n", SystemCoreClock / 1000000 / 4);
    printf("APB2 = SYSCLK / DIV2 = %dMHz\n", SystemCoreClock / 1000000 / 2);


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



//SDRAM内存测试
void fsmc_sdram_test()
{
    u32 i = 0;
    u32 temp = 0;
    u32 sval = 0; //在地址0读到的数据
    printf("每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节\n");
    for(i = 0; i < 32 * 1024 * 1024; i += 16 * 1024)
    {
        *(vu32 *)(Bank5_SDRAM_ADDR + i) = temp;
        temp++;
    }
    temp = 0;
    //依次读出之前写入的数据,进行校验
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
    printf("SDRAM 容量:%dMB\r\n", i / 1024 / 1024); //打印SDRAM容量
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
    uint8_t tbuf[40];
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    if(argc == 1)
    {
        taskENTER_CRITICAL();
        HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        printf("%s\n", tbuf);
        HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
        sprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        printf("%s\n", tbuf);
        sprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
        printf("%s\n", tbuf);
        taskEXIT_CRITICAL();
    }
    else if(strcmp(argv[1], "-s") == 0)
    {
        char *str = argv[2];

        taskENTER_CRITICAL();
        if(str[0] != '\"' && str[strlen(str) - 1] != '\"')
        {
            taskENTER_CRITICAL();
            printf("\nWrong command! please date --help\n");
            taskEXIT_CRITICAL();
            return ;
        }
        //argv[2]: "2016-09-12 15:04:00"
        //str:     ^
        
        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:        ^
        strncpy((char *)tbuf, str, 2);//"16"->tbuf
        RTC_DateStruct.Year = (uint8_t)atoi((char *)tbuf);
        
        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:           ^
        strncpy((char *)tbuf, str, 2);
        RTC_DateStruct.Month = (uint8_t)atoi((char *)tbuf);
        
        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:              ^
        strncpy((char *)tbuf, str, 2);
        RTC_DateStruct.Date = (uint8_t)atoi((char *)tbuf);

        RTC_Set_Date(RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        
        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:                 ^
        strncpy((char *)tbuf, str, 2);
        RTC_TimeStruct.Hours = (uint8_t)atoi((char *)tbuf);
        
        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:                    ^
        strncpy((char *)tbuf, str, 2);
        RTC_TimeStruct.Minutes = (uint8_t)atoi((char *)tbuf);
        
        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:                       ^
        strncpy((char *)tbuf, str, 2);
        RTC_TimeStruct.Seconds = (uint8_t)atoi((char *)tbuf);

        RTC_Set_Time(RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);

        HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        printf("%s\n", tbuf);
        HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
        sprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        printf("%s\n", tbuf);
        sprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
        printf("%s\n", tbuf);
        taskEXIT_CRITICAL();

    }
    else if(strcmp(argv[1], "--help") == 0)
    {
        taskENTER_CRITICAL();
        printf("\neg:date -s \"2016-09-12 15:04:00\"\n");
        taskEXIT_CRITICAL();
    }

}

tinysh_cmd_t cli_systemdate_cmd =
{
    0,
    "date",
    "show system date",
    0, cli_systemtime_fnt,
    "<cr>", 0, 0
};

static void display_args(int argc, char **argv)
{
    int i;
    for(i = 0; i < argc; i++)
    {
        printf("argv[%d]=\"%s\"\n", i, argv[i]);
    }
}

static void foo_fnt(int argc, char **argv)
{
    printf("foo command called\n");
    display_args(argc, argv);
}

tinysh_cmd_t myfoocmd = {0, "foo", "foo command", "[args]",
                         foo_fnt, 0, 0, 0
                        };

static void item_fnt(int argc, char **argv)
{
    printf("item%d command called\n", (int)tinysh_get_arg());
    display_args(argc, argv);
}

tinysh_cmd_t ctxcmd = {0, "ctx", "contextual command", "item1|item2",
                       0, 0, 0, 0
                      };
tinysh_cmd_t item1 = {&ctxcmd, "item1", "first item", "[args]", item_fnt,
                      (void *)1, 0, 0
                     };
tinysh_cmd_t item2 = {&ctxcmd, "item2", "second item", "[args]", item_fnt,
                      (void *)2, 0, 0
                     };


static void atoxi_fnt(int argc, char **argv)
{
    int i;

    for(i = 1; i < argc; i++)
    {
        printf("\"%s\"-->%u (0x%x)\n",
               argv[i], tinysh_atoxi(argv[i]), tinysh_atoxi(argv[i]));
    }
}

tinysh_cmd_t atoxi_cmd = {0, "atoxi", "demonstrate atoxi support",
                          "[args-to-convert]", atoxi_fnt, 0, 0, 0
                         };

