/**
* @file EVSEinit.c
* @brief EVSE≥ı ºªØ
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "includes.h"
#include "globals.h"
#include "userlib_list.h"
#include "interface.h"

void EVSEinit(void)
{
    pListChargePoint = UserListCreate();
    ChargePoint_t *pchargepoint[2];
    int i;
    for(i = 0; i < 2; i++)
    {
        pchargepoint[i] = ChargePointCreate(i);
        pListChargePoint->Add(pListChargePoint, pchargepoint[i]);
    }
}
