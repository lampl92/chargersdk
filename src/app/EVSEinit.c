/**
* @file EVSEinit.c
* @brief EVSE³õÊ¼»¯
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "globals.h"
#include "userlib_list.h"
#include "chargepoint.h"

void EVSEinit(void)
{

    pListChargePoint = UserListCreate();
    ChargePoint_t *pchargepoint_0;
    pchargepoint_0 = ChargePointCreate(0);

    pListChargePoint->Add(pListChargePoint, pchargepoint_0);
}
