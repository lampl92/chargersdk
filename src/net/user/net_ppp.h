#ifndef _NET_PPP_H
#define _NET_PPP_H

#include "net_device.h"

extern Socket *socket_ppp; //Socket 套接字 

net_device_t *net_ppp_create(void);

#endif