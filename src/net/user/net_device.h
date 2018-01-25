#ifndef _NET_DEVICE_
#define _NET_DEVICE_

#include "core/net.h"

typedef enum
{
    NET_STATE_INIT,
    NET_STATE_CONNECT,
    NET_STATE_WORKING
}net_state;

typedef struct _net_device 
{
    NetInterface *interface;
    char name[16];
    net_state state;
}net_device_t;

#endif
