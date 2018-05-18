/*
* @Author: RGW
* @Date:   2018-01-04 11:45:15
* @Last Modified by:   RGW
* @Last Modified time: 2018-01-04 12:27:57
*/

#include "line.h"
#include "evse_globals.h"
#include "electric_energy_meter.h"
#include "user_app.h"
#include "interface.h"
#include <string.h>

#define DEBUG_DIAG_DUMMY_METER
//#define DEBUG_DIAG_DUMMY_TEMP

/**
 * @brief 获取相电压
 * @details 获取相电压
 * 
 * @param pvLine 某相指针
 * @param ucLineID 相:1,2,3=>A,B,C
 * 
 * @return ErrorCode
 */
static ErrorCode_t GetLineVolt(void *pvLine, void *pvCON, uint8_t ucLineID)
{
    //如果电表发生故障, 例如通信故障, errcode 返回 ERR_CON_METER_FAULT; 其他情况返回ERR_NO
    Line_t *pLine;
    CON_t *pCON;
    ErrorCode_t errcode;
    
    pLine = (Line_t *)pvLine;
    pCON = (CON_t *)pvCON;
    errcode = ERR_NO;
#ifdef DEBUG_DIAG_DUMMY_METER
    switch (ucLineID)
    {
    case defLineA:
        pLine->status.dVolt = 220.1 + pCON->info.ucCONID;
        break;
    case defLineB:
        pLine->status.dVolt = 220.2 + pCON->info.ucCONID;
        break;
    case defLineC:
        pLine->status.dVolt = 220.3 + pCON->info.ucCONID;
        break;
    default:
        break;
    }  
#else
    if (pCON->info.ucPhaseLine == 1)
    {
        pLine->status.dVolt = Get_meter_DDSD1352_voltage(pCON->info.ucCONID + 1);
    }
    else
    {
        switch (ucLineID)
        {
        case defLineA:
            pLine->status.dVolt = Get_meter_DTSF1352_voltage(pCON->info.ucCONID + 1, voltage_a);
            break;
        case defLineB:
            pLine->status.dVolt = Get_meter_DTSF1352_voltage(pCON->info.ucCONID + 1, voltage_b);
            break;
        case defLineC:
            pLine->status.dVolt = Get_meter_DTSF1352_voltage(pCON->info.ucCONID + 1, voltage_c);
            break;
        default:
            break;
        } 
    }
    
#endif
    return errcode;
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
static ErrorCode_t GetLineCurr(void *pvLine, void *pvCON, uint8_t ucLineID)
{
    Line_t *pLine;
    CON_t *pCON;
    ErrorCode_t errcode;
    
    pLine = (Line_t *)pvLine;
    pCON = (CON_t *)pvCON;
    errcode = ERR_NO;
    
#ifdef DEBUG_DIAG_DUMMY_METER
    switch (ucLineID)
    {
    case defLineA:
        pLine->status.dCurr = 60.1 + pCON->info.ucCONID;
        break;
    case defLineB:
        pLine->status.dCurr = 60.2 + pCON->info.ucCONID;
        break;
    case defLineC:
        pLine->status.dCurr = 60.3 + pCON->info.ucCONID;
        break;
    default:
        break;
    }  
#else
    if (pCON->info.ucPhaseLine == 1)
    {
        pLine->status.dCurr = Get_meter_DDSD1352_current(pCON->info.ucCONID + 1);
    }
    else
    {
        switch (ucLineID)
        {
        case defLineA:
            pLine->status.dCurr = Get_meter_DTSF1352_current(pCON->info.ucCONID + 1, current_a);
            break;
        case defLineB:
            pLine->status.dCurr = Get_meter_DTSF1352_current(pCON->info.ucCONID + 1, current_b);
            break;
        case defLineC:
            pLine->status.dCurr = Get_meter_DTSF1352_current(pCON->info.ucCONID + 1, current_c);
            break;
        default:
            break;
        }
    }
#endif
    return errcode;
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
static ErrorCode_t GetLineFreq(void *pvLine, void *pvCON, uint8_t ucLineID)
{
    Line_t *pLine;
    CON_t *pCON;
    ErrorCode_t errcode;
    
    pLine = (Line_t *)pvLine;
    pCON = (CON_t *)pvCON;
    errcode = ERR_NO;
    
    if (pCON->info.ucPhaseLine == 1)
    {
        pLine->status.dCurr = Get_meter_DDSD1352_frequency(pCON->info.ucCONID + 1);
    }
    else
    {
        switch (ucLineID)
        {
        case defLineA:
        case defLineB:
        case defLineC:
            pLine->status.dFreq = 50;
            break;
        default:
            break;
        }
    }
    return errcode;
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
static ErrorCode_t GetLinePower(void *pvLine, void *pvCON, uint8_t ucLineID)
{
    Line_t *pLine;
    ErrorCode_t errcode;
    
    pLine = (Line_t *)pvLine;
    errcode = ERR_NO;
    
    switch (ucLineID)
    {
    case defLineA:
    case defLineB:
    case defLineC:
        pLine->status.dPower = pLine->status.dVolt * pLine->status.dCurr;
        break;
    default:
        break;
    }
    return errcode;
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
static ErrorCode_t GetLineTemp(void *pvLine, void *pvCON, uint8_t ucLineID)
{
    Line_t *pLine;
    CON_t *pCON;
    uint8_t a_channel;
    uint8_t b_channel;
    uint8_t c_channel;
    uint8_t n_channel;
    ErrorCode_t errcode;
    
    pLine = (Line_t *)pvLine;
    pCON = (CON_t *)pvCON;
    errcode = ERR_NO;
#ifdef DEBUG_DIAG_DUMMY_TEMP
    if (pCON->info.ucCONID == 0)
    {
        a_channel = 23;
        b_channel = 23;
        c_channel = 23;
        if (pEVSE->info.ucTotalCON > 1)
        {
            n_channel = 23;
        }
        else
        {
            n_channel = 24;
        }
        
    }
    else if (pCON->info.ucCONID == 1)
    {
        a_channel = 24;
        b_channel = 24;
        c_channel = 24;
        n_channel = 24;
    }
    switch (ucLineID)
    {
    case defLineA:
        pLine->status.dTemp = a_channel;
        break;
    case defLineB:
        pLine->status.dTemp = b_channel;
        break;
    case defLineC:
        pLine->status.dTemp = c_channel;
        break;
    case defLineN:
        pLine->status.dTemp = n_channel;
        break;
    default:
        break;
    }
#else
    if (pCON->info.ucCONID == 0)
    {
        a_channel = TEMP_L_OUT;
        b_channel = TEMP_L_OUT;
        c_channel = TEMP_L_OUT;
        if (pEVSE->info.ucTotalCON > 1)
        {
            n_channel = TEMP_L_OUT;
        }
        else
        {
            n_channel = TEMP_N_OUT;
        }
        
    }
    else if (pCON->info.ucCONID == 1)
    {
        a_channel = TEMP_N_OUT;
        b_channel = TEMP_N_OUT;
        c_channel = TEMP_N_OUT;
        n_channel = TEMP_N_OUT;
    }
    switch (ucLineID)
    {
    case defLineA:
        pLine->status.dTemp = get_dc_massage(a_channel);
        break;
    case defLineB:
        pLine->status.dTemp = get_dc_massage(b_channel);
        break;
    case defLineC:
        pLine->status.dTemp = get_dc_massage(c_channel);
        break;
    case defLineN:
        pLine->status.dTemp = get_dc_massage(n_channel);
        break;
    default:
        break;
    }
#endif
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
