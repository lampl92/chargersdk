#include "net_device.h"
#include "net_eth.h"
#include "net_modem.h"
#include "ifconfig.h"
#include "debug.h"

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

error_t net_dev_init(net_device_t *net_dev)
{
    error_t error;
    
    switch (ifconfig.info.ucAdapterSel)
    {
    case 1:
        error = net_eth_init(net_dev);
        break;
    default:
        TRACE_ERROR("!!! NO THIS DEVICE !!!\n");
        error = ERROR_FAILURE;
        break;
    }
    
    return error;
}

error_t net_dev_connect(void)
{
    error_t error;
    
    switch (ifconfig.info.ucAdapterSel)
    {
    case 1:
        error = net_eth_connect();
        break;   
    default:
        error = ERROR_FAILURE;
        TRACE_ERROR("!!! NO THIS DEVICE !!!\n");
        break;
    }
    
    return error;
}

error_t net_dev_disconnect(void)
{
    error_t error;
    
    switch (ifconfig.info.ucAdapterSel)
    {
    case 1:
        net_eth_disconnect();
        break;   
    default:
        error = ERROR_FAILURE;
        TRACE_ERROR("!!! NO THIS DEVICE !!!\n");
        break;
    }
    
    return error;
}