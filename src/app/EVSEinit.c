/**
* @file EVSEinit.c
* @brief EVSE初始化
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "globals.h"
#include "userlib_list.h"
#include "interface.h"

EVSE_t *EVSECreate(void)
{
    EVSE_t *pEVSE;
    pEVSE = (EVSE_t *)malloc(sizeof(EVSE_t));


    return pEVSE;
}

void EVSEinit(void)
{
    static ChargePoint_t *pchargepoint[2];  //在堆中定义

    pEVSE = EVSECreate();
    pListChargePoint = UserListCreate();
    int i;
    for(i = 0; i < 2; i++)
    {
        pchargepoint[i] = ChargePointCreate(i);
        pListChargePoint->Add(pListChargePoint, pchargepoint[i]);
    }
}
