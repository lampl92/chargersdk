#include "includes.h"
#include "utils.h"
#include <time.h>
#include "interface.h"
#include "sys_types.h"
extern void testBnWList(void);
void cli_hello_fnt(int argc, char **argv)
{
    int i;
    printf_safe("\nhello world\n");
    printf_safe("HCLK = SYSCLK = %dMHz\n", SystemCoreClock / 1000000);
    printf_safe("AHB  = SYSCLK / DIV1 = %dMHz\n", SystemCoreClock / 1000000 / 1);
    printf_safe("APB1 = SYSCLK / DIV4 = %dMHz\n", SystemCoreClock / 1000000 / 4);
    printf_safe("APB2 = SYSCLK / DIV2 = %dMHz\n", SystemCoreClock / 1000000 / 2);
    //testBnWList();
#if 0
    CON_t *pCON;
    pCON = CONGetHandle(0);
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REGISTER, 20, 3);//0
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_HEARTBEAT, 20, 3);//1
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_RESET, 20, 3);//2
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_STATUS, 20, 3);//3
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REMOTE_CTRL, 20, 3);//4
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_RTDATA, 20, 3);//5
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_ORDER, 20, 3);//6
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_SET_SUCC, 20, 3);//7
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_SET_FAIL, 20, 3);//8
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_POWERFEE, 20, 3);//14
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_SERVFEE, 20, 3);//15
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_CYC, 20, 3);//16
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_TIMESEG, 20, 3);//17
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_KEY, 20, 3);//18
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_SOFTVER, 20, 3);//19
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_QR, 20, 3);//21
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_SET_BLACK, 20, 3);//22
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_SET_WHITE, 20, 3);//23
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_BLACK, 20, 3);//24
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_REQ_WHITE, 20, 3);//25
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_CARD_START, 20, 3);//28
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_CARD_START_RES, 20, 3);//29
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_CARD_STOP_RES, 20, 3);//30
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_CARD_RTDATA, 20, 3);//31
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_UP_FAULT, 20, 3);//32
    pechProto->sendCommand(pechProto, pEVSE, pCON, ECH_CMDID_UP_WARNING, 20, 3);//33
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
    printf_safe("每隔16K字节,写入一个数据,总共写入2048个数据,刚好是32M字节\n");
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
            printf_safe("[@0x%x]=OK!\n", Bank5_SDRAM_ADDR | i);
        }
        else
        {
            printf_safe("[@0x%x]=ERR!\n", Bank5_SDRAM_ADDR | i);
            break;
        }
        temp++;
    }
    printf_safe("SDRAM 容量:%dMB\r\n", i / 1024 / 1024); //打印SDRAM容量
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
extern time_t time_dat;
void cli_systemtime_fnt(int argc, char **argv)
{
    uint8_t tbuf[40];
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
    int i;
    if(argc == 1)
    {
        for (i = 0; i < 1; i++)
        {
            HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
            sprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
            printf_safe("\n%s\n", tbuf);
            HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
            sprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
            printf_safe("%s\n", tbuf);
            sprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
            printf_safe("%s\n", tbuf);
            //vTaskDelay(1);
        }
        

        printf_safe("<time.h>-----------测试--------------\n");
        time_t now;
        struct tm *ts;
        char buf [80];

        now = time (NULL);
        ts = localtime (& now);
        strftime (buf, sizeof (buf), "%a %Y-%m-%d %H:%M:%S %Z", ts);
        printf_safe("%s \n", buf);
    }
    else if(strcmp(argv[1], "-s") == 0)
    {
        struct tm settime_tm;
        time_t tim_set;
        char *str = argv[2];
        //argv[2]: "2016-09-12 15:04:00"
        //str:     ^
        if(str[0] != '\"' && str[strlen(str) - 1] != '\"')
        {
            printf_safe("\nWrong command! please date --help\n");
            return ;
        }

        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:        ^
        strncpy((char *)tbuf, str, 2);//"16"->tbuf
        RTC_DateStruct.Year = (uint8_t)atoi((char *)tbuf);
        settime_tm.tm_year = RTC_DateStruct.Year + 2000 - 1900;

        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:           ^
        strncpy((char *)tbuf, str, 2);
        RTC_DateStruct.Month = (uint8_t)atoi((char *)tbuf);
        settime_tm.tm_mon = RTC_DateStruct.Month - 1;

        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:              ^
        strncpy((char *)tbuf, str, 2);
        RTC_DateStruct.Date = (uint8_t)atoi((char *)tbuf);
        settime_tm.tm_mday = RTC_DateStruct.Date;

        //RTC_Set_Date(RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);

        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:                 ^
        strncpy((char *)tbuf, str, 2);
        RTC_TimeStruct.Hours = (uint8_t)atoi((char *)tbuf);
        settime_tm.tm_hour = RTC_TimeStruct.Hours;

        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:                    ^
        strncpy((char *)tbuf, str, 2);
        RTC_TimeStruct.Minutes = (uint8_t)atoi((char *)tbuf);
        settime_tm.tm_min = RTC_TimeStruct.Minutes;

        str += 3;
        //argv[2]: "2016-09-12 15:04:00"
        //str:                       ^
        strncpy((char *)tbuf, str, 2);
        RTC_TimeStruct.Seconds = (uint8_t)atoi((char *)tbuf);
        settime_tm.tm_sec = RTC_TimeStruct.Seconds;

        //RTC_Set_Time(RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        tim_set = mktime(&settime_tm);
        time(&tim_set);

        HAL_RTC_GetTime(&RTC_Handler, &RTC_TimeStruct, RTC_FORMAT_BIN);
        sprintf((char *)tbuf, "Time:%02d:%02d:%02d", RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);
        printf_safe("\n%s\n", tbuf);
        HAL_RTC_GetDate(&RTC_Handler, &RTC_DateStruct, RTC_FORMAT_BIN);
        sprintf((char *)tbuf, "Date:20%02d-%02d-%02d", RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date);
        printf_safe("%s\n", tbuf);
        sprintf((char *)tbuf, "Week:%d", RTC_DateStruct.WeekDay);
        printf_safe("%s\n", tbuf);
    }
    else if(strcmp(argv[1], "--help") == 0)
    {
        printf_safe("\neg:date -s \"2016-09-12 15:04:00\"\n");
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

