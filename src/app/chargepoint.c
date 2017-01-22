/**
* @file chargepoint.c
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "chargepoint.h"

ChargePoint_t *ChargePointCreate(uint8_t ucPointID )
{
    ChargePoint_t *pChargePoint;
    pChargePoint = (ChargePoint_t *)malloc(sizeof(ChargePoint_t));
    if(pChargePoint == NULL)
    {
        return NULL;
    }

    pChargePoint->ucPointID = ucPointID;

}
