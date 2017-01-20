/**
* @file chargepoint.h
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __CHARGEPOINT_H
#define  __CHARGEPOINT_H

#include "stm32f4xx.h"

typedef enum
{
    POINT_NO_ERR = 0,
    //
    POINT_CP_ERR,
    POINT_CP_12V,
    POINT_CP_9V,
    POINT_CP_9V_PWM,
    POINT_CP_6V_PWM,
    //
    POINT_CC_PE,
    POINT_CC_NO,
    //
    POINT_LOCK,
    POINT_UNLOCK


}PointStateType; //Charge point error number

typedef struct _ChargePoint
{
    uint8_t  ucPointID;           // 枪号
    PointStateType uiCPState;     // 检测点1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    PointStateType uiCCState;     // 检测点4 CC state --PE

    PointStateType (*GetCPState)(uint8_t  ucPointID);
    PointStateType (*GetCCState)(uint8_t  ucPointID);
    uint32_t (*GetACLTemp)(uint8_t  ucPointID);
    uint32_t (*GetACNTemp)(uint8_t  ucPointID);
    uint32_t (*GetBTypeConnectorTemp1)(uint8_t  ucPointID);
    uint32_t (*GetBTypeConnectorTemp2)(uint8_t  ucPointID);
    PointStateType (*LockBTypeConnector)(uint8_t  ucPointID);
    PointStateType (*GetBTypeConnectorState)(uint8_t  ucPointID); //lock unlock
    PointStateType (*StartTransaction)(uint8_t  ucPointID);
    PointStateType (*StopTransaction)(uint8_t  ucPointID);
} ChargePoint_t;

#endif
