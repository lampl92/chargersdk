/**
* @file ech_globals.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-23
*/
#ifndef __ECH_GLOBALS_H
#define __ECH_GLOBALS_H

#include "libEcharge/ech_protocol.h"

extern echProtocol_t *pechProto;

uint8_t EchRemoteIDtoCONID(uint8_t remote_id);
uint8_t EchCONIDtoRemoteID(uint8_t con_id, uint8_t total);

#endif /* __PS_GLOBALS_H */
