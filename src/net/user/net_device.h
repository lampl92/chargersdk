#ifndef _NET_DEVICE_
#define _NET_DEVICE_

#include "core/net.h"

typedef enum
{
    NET_FLAG_CONNECTED
}net_flags;

typedef enum
{
    NET_STATE_IDLE,
    NET_STATE_INIT,
    NET_STATE_CONNECT,
    NET_STATE_FTP,
    NET_STATE_TCP_ON
}net_state;

typedef struct _net_device 
{
    NetInterface *interface;
    char name[16];
    net_state state;
}net_device_t;

error_t net_dev_init(net_device_t *net_dev);
error_t net_dev_connect(void);

#endif
