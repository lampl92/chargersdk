#ifndef _NET_DEVICE_
#define _NET_DEVICE_

#include "core/net.h"

typedef enum
{
    DS_NET_INIT,
    DS_NET_CONNECT,
    DS_NET_OK
}net_state;

typedef struct _net_device
{
    NetInterface *interface;
    char name[16];
    net_state state;
}net_device_t;

#endif
