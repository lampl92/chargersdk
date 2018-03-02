#ifndef _NET_ETH_H_
#define _NET_ETH_H_

#include "net_device.h"
#include "core/net.h"

extern Socket *socket_eth; //Socket 套接字 

net_device_t *net_eth_create(void);

#endif
