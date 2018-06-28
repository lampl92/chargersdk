#ifndef _NET_DEVICE_
#define _NET_DEVICE_

#include "core/net.h"

#define NET_DEVICE_ETH   1
#define NET_DEVICE_MODEM  2


typedef error_t (*net_dev_ft)(void *vnet_dev);
    
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
    NET_STATE_TCP_ON,
    NET_STATE_DISCONNECT,
    NET_STATE_ERR,
    
}net_state;

typedef struct _net_device 
{
    NetInterface *interface;
    char name[16];
    uint8_t is_dhcp_inited;
    net_state state;
    net_dev_ft init;
    net_dev_ft connect;
    net_dev_ft disconnect;
    net_dev_ft close_hard;
}net_device_t;

extern net_device_t *net_dev;

net_device_t *get_net_device_handler(uint8_t dev_num);
net_device_t *net_device_create(uint8_t dev_num);
#endif
