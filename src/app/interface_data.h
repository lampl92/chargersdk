/**
* @file interface_data.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __INTERFACE_DATA_H
#define  __INTERFACE_DATA_H

#include "includes.h"


//ErrorCode_t CreateOrderFile(void);
ErrorCode_t OrderDBCreate(void);
ErrorCode_t OrderDBInsertItem(OrderData_t *pOrder);
#endif
