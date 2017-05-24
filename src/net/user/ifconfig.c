/**
* @file ifconfig.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#include "lwipopts.h"
#include "ifconfig.h"


ifconfig_t ifconfig;

void ifconfig_set(void)
{
#ifdef LWIP_DEBUG
    printf_safe("local ip = %s\n", ipaddr_ntoa(&ifconfig.local_ip));
    printf_safe("gateway = %s\n", ipaddr_ntoa(&ifconfig.gateway));
    printf_safe("netmask = %s\n", ipaddr_ntoa(&ifconfig.netmask));
#endif
    /** @todo (zshare#1#): 向文件系统写入当前配置 */
}

void ifconfig_init(void)
{
    ifconfig.local_ip.addr = 0;
    ifconfig.gateway.addr = 0;
    ifconfig.netmask.addr = 0;
    //IP4_ADDR(&ifconfig.server_ip,124,207,112,70);
    IP4_ADDR(&ifconfig.server_ip,123,56,113,123);
    ifconfig.server_ip_bak.addr = 0;
    ifconfig.server_port = 6677;//8051;
    ifconfig.server_port_bak = 0;
    ifconfig.local_port = 0;
    ifconfig.server_domain[0] = '\0';
}
