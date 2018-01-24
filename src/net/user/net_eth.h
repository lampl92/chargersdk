#ifndef _NET_ETH_H_
#define _NET_ETH_H_

#include "net_device.h"
#include "core/net.h"

extern net_device_t eth_dev;
extern Socket *socket_eth; //Socket 套接字 

int_t net_eth_init(net_device_t *net_dev, uint32_t n);
error_t net_eth_connect(void);
error_t net_eth_reconnect(void);

#endif
