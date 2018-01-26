#include "net_device.h"
#include "net_eth.h"

error_t net_dev_init(net_device_t *net_dev)
{
    error_t error;
    
    error = net_eth_init(net_dev);
    
    return error;
}

error_t net_dev_connect(void)
{
    error_t error;
    
    error = net_eth_connect();
    
    return error;
}