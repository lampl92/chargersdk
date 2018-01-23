/**
* @file lwip_init.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#ifndef  __INTERFACE_NETWORK_H
#define  __INTERFACE_NETWORK_H

#include "net_device.h"
#include "core/net.h"

error_t net_stack_init(void);
Socket *net_connect_server_via_tcp(net_device_t *net_dev, const char *server_ip, uint16_t port, error_t *perror);
void ifconfig_update(net_device_t *net_dev);

int netSend(uint8_t *pbuff, uint32_t len);
int netRecv(uint8_t *pbuff, uint32_t buff_len);

#endif
