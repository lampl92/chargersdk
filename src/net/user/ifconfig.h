/**
* @file ifconfig.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#ifndef  __IFCONFIG_H
#define  __IFCONFIG_H

#include "lwip/ip_addr.h"

typedef struct _ifconfig
{
    ip_addr_t local_ip;
    ip_addr_t gateway;
    ip_addr_t netmask;
    ip_addr_t server_ip;
    ip_addr_t server_ip_bak;
    uint16_t server_port;
    uint16_t server_port_bak;
    uint16_t local_port;
    uint8_t server_domain[50];
}ifconfig_t;

extern ifconfig_t ifconfig;

void ifconfig_init(void);
void ifconfig_set(void);
#endif
