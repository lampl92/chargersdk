/**
* @file connector.h
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#ifndef  __Connector_H
#define  __Connector_H

#include "stm32f4xx.h"

typedef enum
{
    CONNECTOR_NO_ERR = 0,
    //
    CONNECTOR_CP_ERR,
    CONNECTOR_CP_12V,
    CONNECTOR_CP_9V,
    CONNECTOR_CP_9V_PWM,
    CONNECTOR_CP_6V_PWM,
    //
    CONNECTOR_CC_PE,
    CONNECTOR_CC_NO,
    //
    CONNECTOR_LOCK,
    CONNECTOR_UNLOCK


}ConnectorStateType; 

typedef struct _Connector
{
    uint8_t  ucConnectorID;           // 枪号
    ConnectorStateType uiCPState;     // 检测点1 CP state --12V / 9V / 9V_PWM / 6V_PWM
    ConnectorStateType uiCCState;     // 检测点4 CC state --PE

    ConnectorStateType (*GetCPState)(uint8_t  ucConnectorID);
    ConnectorStateType (*GetCCState)(uint8_t  ucConnectorID);
    uint32_t (*GetACLTemp)(uint8_t  ucConnectorID);
    uint32_t (*GetACNTemp)(uint8_t  ucConnectorID);
    uint32_t (*GetBTypeConnectorTemp1)(uint8_t  ucConnectorID);
    uint32_t (*GetBTypeConnectorTemp2)(uint8_t  ucConnectorID);
    ConnectorStateType (*LockBTypeConnector)(uint8_t  ucConnectorID);
    ConnectorStateType (*GetBTypeConnectorState)(uint8_t  ucConnectorID); //lock unlock
    ConnectorStateType (*StartTransaction)(uint8_t  ucConnectorID);
    ConnectorStateType (*StopTransaction)(uint8_t  ucConnectorID);
} Connector_t;

#endif
