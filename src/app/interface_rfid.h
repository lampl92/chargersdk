/**
* @file interface_rfid.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-06
*/
#ifndef  __INTERFACE_RFID_H
#define  __INTERFACE_RFID_H

#include "RFIDReader_mt626.h"

void *CreateRFIDDev(void);
int GetUID(void *RFIDDev, uint8_t *pUID);
#endif
