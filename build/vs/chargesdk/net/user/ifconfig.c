/**
* @file ifconfig.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-04
*/
#include "ifconfig.h"
#include "cfg_parse.h"
#include "stringName.h"


ifconfig_t ifconfig;



void ifconfig_get(void)
{
    cfg_get_string(pathNetCfg, ifconfig.info.strIP, "%s", jnNetIP);
    cfg_get_string(pathNetCfg, ifconfig.info.strMask, "%s", jnNetMask);
    cfg_get_string(pathNetCfg, ifconfig.info.strGate, "%s", jnNetGate);
    cfg_get_string(pathNetCfg, ifconfig.info.strMAC, "%s", jnNetMAC);
    cfg_get_string(pathNetCfg, ifconfig.info.strHostName, "%s", jnNetHostName);
    cfg_get_uint8(pathNetCfg, &(ifconfig.info.ucDHCPEnable), "%s", jnNetDHCP);
}

void ifconfig_init(void)
{
    memset(&ifconfig, 0, sizeof(ifconfig_t));
    ifconfig_get();
    
}
