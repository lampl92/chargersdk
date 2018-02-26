#include "net_device.h"
#include "net_eth.h"
#include "net_ppp.h"
#include "ifconfig.h"
#include "debug.h"

#if 0
net_device_t *get_net_device_handler(uint8_t dev_num)
{
    switch (dev_num)
    {
    case 1:
        return &eth_dev;
    case 2:
        return &modem_dev;
    default:
        return NULL;
    }
}
#endif
net_device_t *net_dev;

net_device_t *net_device_create(uint8_t dev_num)
{
    switch (dev_num)
    {
    case 1:
        return net_eth_create();
    case 2:
        return net_ppp_create();
    default:
        return NULL;
    }
}
