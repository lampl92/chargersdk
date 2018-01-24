/**
* @file ifconfig.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#ifndef  __IFCONFIG_H
#define  __IFCONFIG_H

#include "core/net.h"
#include "errorcode.h"

typedef struct _ifconfig
{
    struct
    {
        uint8_t ucAdapterSel;
        uint8_t ucDHCPEnable;
        char strHostName[16 + 1];
        char strMAC[18];
        char strIP[16];
        char strMask[16];
        char strGate[16];
        char strDNS1[16];
        char strDNS2[16];
    }info;
    struct
    {
        char strIP[16];
        char strMask[16];
        char strGate[16];
        char strDNS1[16];
        char strDNS2[16];
    }status;
}ifconfig_t;

extern ifconfig_t ifconfig;

void ifconfig_get(void);
void ifconfig_init(void);
#endif