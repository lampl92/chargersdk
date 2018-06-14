/*
* @Author: RGW
* @Date:   2018-01-04 11:45:15
* @Last Modified by:   RGW
* @Last Modified time: 2018-01-04 12:27:57
*/

#include "line.h"
#include "evse_globals.h"
#include "user_app.h"
#include "interface.h"
#include <string.h>


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
        if (pEVSE->info.ucPhaseLine == 3)
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
        if (pEVSE->info.ucPhaseLine == 3)
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

    pLine->GetTemp = GetLineTemp;

}
