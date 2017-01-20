/**
* @file chargepoint.c
* @brief 定义最小充电单元需要的数据与操作
* @author rgw
* @version v1.0
* @date 2017-01-18
*/
#include "chargepoint.h""

static uint32_t uiCheckPoint1State;    // CP state --12V / 9V / 9V_PWM / 6V_PWM
static uint32_t uiCheckPoint4State;    // CC state --PE
static uint32_t uiACLTemp;             //AC_L进线温度
static uint32_t uiACNTemp;             //AC_N进线温度

static uint32_t (*GetCheckPoint1State)();
static uint32_t (*GetCheckPoint4State)();
static uint32_t (*GetBTypeConnectorTemp1)();
static uint32_t (*GetBTypeConnectorTemp2)();
static ChargePointErr_e (*LockBTypeConnector)();
static ChargePointErr_e (*GetBTypeConnectorState)(); //lock unlock
static ChargePointErr_e (*StartTransaction)();
static ChargePointErr_e (*StopTransaction)();

ChargePoint_t *ChargePointCreate(uint8_t ucChargePointID, )
{
    ChargePoint_t *pChargePoint;
    pChargePoint = (ChargePoint_t *)malloc(sizeof(ChargePoint_t));
    if(pChargePoint == NULL)
    {
        return NULL;
    }
    pChargePoint->ucChargePointID = ucChargePointID;

}
