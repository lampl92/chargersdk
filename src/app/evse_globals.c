/**
* @file globals.c
* @brief EVSE应用中涉及到的全局变量
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#include "interface.h"
#include "userlib_list.h"
#include "net_device.h"

EVSE_t *pEVSE;
UserList_t *pListCON;
RFIDDev_t *pRFIDDev;
echProtocol_t *pechProto;
//net_device_t eth_dev; 定义在net_eth.c中
