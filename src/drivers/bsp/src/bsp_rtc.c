#include <time.h>
#include "bsp.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//RTC驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

RTC_HandleTypeDef RTC_Handler;  //RTC句柄
time_t time_dat;
//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败
HAL_StatusTypeDef RTC_Set_Time(u8 hour, u8 min, u8 sec)
{
    RTC_TimeTypeDef RTC_TimeStructure;
    u8 ampm;
    if(RTC_Handler.Init.HourFormat == RTC_HOURFORMAT_24)
    {
        if(hour <= 12)
        {
            ampm = RTC_HOURFORMAT12_AM;
        }
        else if(hour > 12 && hour < 23)
        {
            ampm = RTC_HOURFORMAT12_PM;
        }
    }
    else if(RTC_Handler.Init.HourFormat == RTC_HOURFORMAT_12)
    {
        //TODO: 不设置12小时制,进行错误处理
        while(1);
    }
    RTC_TimeStructure.Hours = hour;
    RTC_TimeStructure.Minutes = min;
    RTC_TimeStructure.Seconds = sec;
    RTC_TimeStructure.TimeFormat = ampm;
    RTC_TimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
    return HAL_RTC_SetTime(&RTC_Handler, &RTC_TimeStructure, RTC_FORMAT_BIN);
}

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败
HAL_StatusTypeDef RTC_Set_Date(u8 year, u8 month, u8 date)
{
    RTC_DateTypeDef RTC_DateStructure;
    int y, m, d, w;
    y = year;
    m = month;
    d = date;
    //基姆拉尔森计算公式, w=0 时为 星期一
    if(m == 1 || m == 2)
    {
        m += 12;
        y--;
    }
    w = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;

    RTC_DateStructure.WeekDay = w + 1;
    RTC_DateStructure.Month = month;
    RTC_DateStructure.Date = date;
    RTC_DateStructure.Year = year;
    return HAL_RTC_SetDate(&RTC_Handler, &RTC_DateStructure, RTC_FORMAT_BIN);
}

//RTC初始化
//返回值:0,初始化成功;
//       2,进入初始化模式失败;
u8 bsp_RTC_Init(void)
{


    RTC_Handler.Instance = RTC;
    RTC_Handler.Init.HourFormat = RTC_HOURFORMAT_24; //RTC设置为24小时格式
    RTC_Handler.Init.AsynchPrediv = 0X7F;         //RTC异步分频系数(1~0X7F)
    RTC_Handler.Init.SynchPrediv = 0XFF;          //RTC同步分频系数(0~7FFF)
    RTC_Handler.Init.OutPut = RTC_OUTPUT_DISABLE;
    RTC_Handler.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTC_Handler) != HAL_OK)
    {
        return 2;
    }

    if(HAL_RTCEx_BKUPRead(&RTC_Handler, RTC_BKP_DR0) != 0X5050) //是否第一次配置
    {
        RTC_Set_Time(9, 34, 56);       //设置时间 ,根据实际时间修改
        RTC_Set_Date(16, 11, 21);                        //设置日期
        HAL_RTCEx_BKUPWrite(&RTC_Handler, RTC_BKP_DR0, 0X5050); //标记已经初始化过了
    }
    return 0;
}

//RTC底层驱动，时钟配置
//此函数会被HAL_RTC_Init()调用
//hrtc:RTC句柄
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
    HAL_PWR_EnableBkUpAccess();//取消备份区域写保护

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE; //LSE配置
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;                //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC; //外设为RTC
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE; //RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_WeekDay_Definitions
//hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(u8 week, u8 hour, u8 min, u8 sec)
{
    RTC_AlarmTypeDef RTC_AlarmSturuct;

    RTC_AlarmSturuct.AlarmTime.Hours = hour; //小时
    RTC_AlarmSturuct.AlarmTime.Minutes = min; //分钟
    RTC_AlarmSturuct.AlarmTime.Seconds = sec; //秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds = 0;
    RTC_AlarmSturuct.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;

    RTC_AlarmSturuct.AlarmMask = RTC_ALARMMASK_NONE; //精确匹配星期，时分秒
    RTC_AlarmSturuct.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY; //按星期
    RTC_AlarmSturuct.AlarmDateWeekDay = week; //星期
    RTC_AlarmSturuct.Alarm = RTC_ALARM_A;   //闹钟A
    HAL_RTC_SetAlarm_IT(&RTC_Handler, &RTC_AlarmSturuct, RTC_FORMAT_BIN);

    HAL_NVIC_SetPriority(RTC_Alarm_IRQn, bspAlarm_PreemptPriority, bspAlarm_SubPriority);
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

//周期性唤醒定时器设置
/*wksel:  @ref RTCEx_Wakeup_Timer_Definitions
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u32 wksel, u16 cnt)
{
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志

    HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler, cnt, wksel);          //设置重装载值和时钟

    HAL_NVIC_SetPriority(RTC_WKUP_IRQn, bspWKUP_PreemptPriority, bspWKUP_SubPriority);
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RTC_Handler);
}

//RTC闹钟A中断处理回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    xprintf("ALARM A!\r\n");
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler);
}

//RTC WAKE UP中断处理
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
    time_dat ++;
}

time_t time (time_t *_timer)
{
    struct tm *ts;
    if(_timer != NULL)
    {
        time_dat = *_timer;
        ts = localtime (_timer);
        RTC_Set_Time(ts->tm_hour, ts->tm_min, ts->tm_sec);
        RTC_Set_Date(ts->tm_year+1900-2000, ts->tm_mon+1, ts->tm_mday);
    }
    return time_dat;
}
extern volatile uint32_t ulHighFrequencyTimerTicks;
clock_t clock(void)
{
    clock_t clock_dat;
    clock_dat = (clock_t)ulHighFrequencyTimerTicks;
    return clock_dat;
}
