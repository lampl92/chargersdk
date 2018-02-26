#include "interface_network.h"
#include "evse_globals.h"

#include "net_device.h"
#include "ppp/ppp.h"
#include "ppp/ppp_hdlc.h"
#include "drivers/uart/stm32f4x9_uart_driver.h"
#include "debug.h"

PppContext pppContext;
Socket *socket_ppp; //Socket 套接字

error_t net_ppp_connect(void *pvnet_dev)
{
    net_device_t *net_dev;
    error_t error;
    
    net_dev = (net_device_t*)pvnet_dev;
    error = pppConnect(net_dev->interface);
    
    socket_ppp = netif_connect_server_via_tcp(pvnet_dev, pechProto->info.strServerIP, pechProto->info.usServerPort, &error);
    
    return error;
}
error_t net_ppp_reconnect(void *pvnet_dev)
{
    error_t error;
    
    socketClose(socket_ppp);
    error = net_ppp_connect(pvnet_dev);
    
    return error;
}
error_t net_ppp_disconnect(void *pvnet_dev)
{
    socketClose(socket_ppp);
    socket_ppp = NULL;
    return NO_ERROR;
}
error_t net_ppp_init(void *pvnet_dev)
{
    net_device_t *net_dev;
    error_t error;
    NetInterface *interface;
    PppSettings pppSettings;
    
    net_dev = (net_device_t*)pvnet_dev;
    interface = net_dev->interface;
    //设置接口名称
    netSetInterfaceName(interface, net_dev->name);
    //初始化ppp uart,在netConfigInterface中会调用init, 因此要先初始化
    netSetUartDriver(interface, &stm32f4x9UartDriver);

    //PPP初始化
    pppGetDefaultSettings(&pppSettings);
    pppSettings.interface = interface;
    error = pppInit(&pppContext, &pppSettings);
    if (error)
    {
        TRACE_ERROR("PPP初始化失败!code = %d\r\n", error);
        return error;
    }
    //初始化网路接口
    error = netConfigInterface(interface);
    if (error)
    {
        TRACE_ERROR("配置接口 %s 失败!code = %d\r\n", interface->name, error);
        return error;
    }
    
    return error;
}

net_device_t *net_ppp_create(void)
{
    net_device_t *net;
    net = (net_device_t*)malloc(sizeof(net_device_t));
    memset(net, 0, sizeof(net_device_t));
    net->init = net_ppp_init;
    net->connect = net_ppp_connect;
    net->disconnect = net_ppp_disconnect;
    
    return net;
}
