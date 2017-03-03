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
    printf_safe("\nhello world\n");
    printf_safe("HCLK = SYSCLK = %dMHz\n", SystemCoreClock / 1000000);
    printf_safe("AHB  = SYSCLK / DIV1 = %dMHz\n", SystemCoreClock / 1000000 / 1);
    printf_safe("APB1 = SYSCLK / DIV4 = %dMHz\n", SystemCoreClock / 1000000 / 4);
    printf_safe("APB2 = SYSCLK / DIV2 = %dMHz\n", SystemCoreClock / 1000000 / 2);
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
    printf_safe("ÿ��16K�ֽ�,д��һ������,�ܹ�д��2048������,�պ���32M�ֽ�\n");
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
            printf_safe("[@0x%x]=OK!\n", Bank5_SDRAM_ADDR | i);
        }
        else
        {
            printf_safe("[@0x%x]=ERR!\n", Bank5_SDRAM_ADDR | i);
            break;
        }
        temp++;
    }
    printf_safe("SDRAM ����:%dMB\r\n", i / 1024 / 1024); //��ӡSDRAM����
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
        xsprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        printf_safe("\n%s\n", tbuf);
        HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
        xsprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        printf_safe("%s\n", tbuf);
        xsprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
        printf_safe("%s\n", tbuf);
        taskEXIT_CRITICAL();
    }
    else if(strcmp(argv[1], "-s") == 0)
    {
        char *str = argv[2];
        //argv[2]: "2016-09-12 15:04:00"
        //str:     ^
        taskENTER_CRITICAL();
        if(str[0] != '\"' && str[strlen(str) - 1] != '\"')
        {
            taskENTER_CRITICAL();
            printf_safe("\nWrong command! please date --help\n");
            taskEXIT_CRITICAL();
            return ;
        }

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
        xsprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        printf_safe("\n%s\n", tbuf);
        HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
        xsprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        printf_safe("%s\n", tbuf);
        xsprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
        printf_safe("%s\n", tbuf);
        taskEXIT_CRITICAL();

    }
    else if(strcmp(argv[1], "--help") == 0)
    {
        taskENTER_CRITICAL();
        printf_safe("\neg:date -s \"2016-09-12 15:04:00\"\n");
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
        printf_safe("argv[%d]=\"%s\"\n", i, argv[i]);
    }
}

static void foo_fnt(int argc, char **argv)
{
    printf_safe("foo command called\n");
    display_args(argc, argv);
}

tinysh_cmd_t myfoocmd = {0, "foo", "foo command", "[args]",
                         foo_fnt, 0, 0, 0
                        };

static void item_fnt(int argc, char **argv)
{
    printf_safe("item%d command called\n", (int)tinysh_get_arg());
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
        printf_safe("\"%s\"-->%u (0x%x)\n",
               argv[i], tinysh_atoxi(argv[i]), tinysh_atoxi(argv[i]));
    }
}

tinysh_cmd_t atoxi_cmd = {0, "atoxi", "demonstrate atoxi support",
                          "[args-to-convert]", atoxi_fnt, 0, 0, 0
                         };

