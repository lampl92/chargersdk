/*
* @Author: RGW
* @Date:   2018-01-04 11:45:15
* @Last Modified by:   RGW
* @Last Modified time: 2018-01-04 12:27:57
*/

#include "line.h"
#include <string.h>
/**
 * @brief 获取相电压
 * @details 获取相电压
 * 
 * @param pvLine 某相指针
 * @param ucLineID 相:1,2,3=>A,B,C
 * 
 * @return ErrorCode
 */
static ErrorCode_t GetLineVolt(void *pvLine, uint8_t ucCONID, uint8_t ucLineID)
{
    //如果电表发生故障, 例如通信故障, errcode 返回 ERR_CON_METER_FAULT; 其他情况返回ERR_NO
    
    switch (ucLineID)
    {
    case defLineA:
        break;
    case defLineB:
        break;
    case defLineC:
        break;
    default:
        break;
    }
}
/**
 * @brief 获取相电流
 * @details 获取相电流
 * 
 * @param pvLine 某相指针
 * @param ucLineID 相:1,2,3=>A,B,C
 * 
 * @return ErrorCode
 */
static ErrorCode_t GetLineCurr(void *pvLine, uint8_t ucCONID, uint8_t ucLineID)
{
    switch (ucLineID)
    {
    case defLineA:
        break;
    case defLineB:
        break;
    case defLineC:
        break;
    default:
        break;
    }
}
/**
 * @brief 获取相频率
 * @details 获取相频率
 * 
 * @param pvLine 某相指针
 * @param ucLineID 相:1,2,3=>A,B,C
 * 
 * @return ErrorCode
 */
static ErrorCode_t GetLineFreq(void *pvLine, uint8_t ucCONID, uint8_t ucLineID)
{
    switch (ucLineID)
    {
    case defLineA:
        break;
    case defLineB:
        break;
    case defLineC:
        break;
    default:
        break;
    }
}
/**
 * @brief 获取相功率
 * @details 获取相功率
 * 
 * @param pvLine 某相指针
 * @param ucLineID 相:1,2,3=>A,B,C
 * 
 * @return ErrorCode
 */
static ErrorCode_t GetLinePower(void *pvLine, uint8_t ucCONID, uint8_t ucLineID)
{
    switch (ucLineID)
    {
    case defLineA:
        break;
    case defLineB:
        break;
    case defLineC:
        break;
    default:
        break;
    }
}

/**
 * @brief 获取相温度
 * @details 获取相温度
 * 
 * @param pvLine 某相指针
 * @param ucLineID 相:1,2,3=>A,B,C
 * 
 * @return ErrorCode
 */
static ErrorCode_t GetLineTemp(void *pvLine, uint8_t ucCONID, uint8_t ucLineID)
{
    switch (ucLineID)
    {
    case defLineA:
        break;
    case defLineB:
        break;
    case defLineC:
        break;
    case defLineN:
        break;
    default:
        break;
    }
}

void LineInit(Line_t *pLine, uint8_t ucLineID)
{
    memset(pLine, 0, sizeof(Line_t));
    pLine->info.ucLineID = ucLineID;
    pLine->GetCurr = GetLineCurr;
    pLine->GetFreq = GetLineFreq;
    pLine->GetPower = GetLinePower;
    pLine->GetTemp = GetLineTemp;
    pLine->GetVolt = GetLineVolt;
}
