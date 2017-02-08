/**
* @file chargepoint.c
* @brief ������С��絥Ԫ��Ҫ�����������
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include <stdlib.h>
#include "chargepoint.h"

ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID )
{
    ChargePoint_t *pChargePoint;
    pChargePoint = (ChargePoint_t *)malloc(sizeof(ChargePoint_t));
    if(pChargePoint == NULL)
    {
        return NULL;
    }

    pChargePoint->ucChargePointID = ucChargePointID;

    pChargePoint->xHandleEventGroupStartCharge = xEventGroupCreate();
    pChargePoint->xHandleEventGroupStopCharge = xEventGroupCreate();
}
