#ifndef __BSP_RTC_H
#define __BSP_RTC_H

extern RTC_HandleTypeDef RTC_Handler;  //RTC句柄
    
uint8_t bsp_RTC_Init(void);              //RTC初始化
HAL_StatusTypeDef RTC_Set_Time(uint8_t hour,uint8_t min,uint8_t sec);      //RTC时间设置
HAL_StatusTypeDef RTC_Set_Date(uint8_t year,uint8_t month,uint8_t date);	//RTC日期设置
void RTC_Set_AlarmA(uint8_t week,uint8_t hour,uint8_t min,uint8_t sec); //设置闹钟时间(按星期闹铃,24小时制)
void RTC_Set_WakeUp(uint32_t wksel,uint16_t cnt);             //周期性唤醒定时器设置
void printTime(time_t now);
#endif
