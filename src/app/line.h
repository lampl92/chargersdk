/**
* @file connector.h
* @brief 定义充电接口需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __CON_LINE_H
#define  __CON_LINE_H

#include "stm32f4xx.h"
#include "errorcode.h"

typedef ErrorCode_t(*pLine_ft)(void *pvLine, uint8_t ucPhase);


typedef struct _Line
{
    struct _info
    {
        uint8_t ucPhase;//1,2,3=>A,B,C
        double dRatedCurr;
    }info;
    struct _status
    {
        double dVolt;
        double dCurr;
        double dFreq;
        double dPower;
        double dEnergy;
        double dTemp;
        uint8_t ucRelayState;
    }status;
    pLine_ft GetVolt;
    pLine_ft GetCurr;
    pLine_ft GetFreq;
    pLine_ft GetPower;
    pLine_ft GetEnergy;
    pLine_ft GetTemp;
    
}Line_t;

#endif
