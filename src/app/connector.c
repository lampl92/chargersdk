/**
* @file Connector.c
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "Connector.h"

Connector_t *ConnectorCreate(uint8_t ucConnectorID )
{
    Connector_t *pConnector;
    pConnector = (Connector_t *)malloc(sizeof(Connector_t));
    if(pConnector == NULL)
    {
        return NULL;
    }

    pConnector->ucConnectorID = ucConnectorID;

}
