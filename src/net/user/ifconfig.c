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
#include "evse_globals.h"

ifconfig_t ifconfig;

void ifconfig_get(void)
{
    MacAddr macaddr;
    cfg_get_uint8(pathNetCfg, &(ifconfig.info.ucAdapterSel), "%s", jnNetAdapter);
    cfg_get_uint8(pathNetCfg, &(ifconfig.info.ucDHCPEnable), "%s", jnNetDHCP);
    cfg_get_string(pathNetCfg, ifconfig.info.strHostName, "%s", jnNetHostName);
    if (g_ucNandUID[0] || g_ucNandUID[1] || g_ucNandUID[2] || g_ucNandUID[3] ||
        g_ucNandUID[4] || g_ucNandUID[5] || g_ucNandUID[6] || g_ucNandUID[7])
    {
        for (int i = 0; i < 6; i++)
        {
            macaddr.b[i] = g_ucNandUID[i + 2];
        }
        macAddrToString(&macaddr, ifconfig.info.strMAC);
    }
    else
    {
        cfg_get_string(pathNetCfg, ifconfig.info.strMAC, "%s", jnNetMAC);
    }
    cfg_get_string(pathNetCfg, ifconfig.info.strIP, "%s", jnNetIP);
    cfg_get_string(pathNetCfg, ifconfig.info.strMask, "%s", jnNetMask);
    cfg_get_string(pathNetCfg, ifconfig.info.strGate, "%s", jnNetGate);
    cfg_get_string(pathNetCfg, ifconfig.info.strDNS1, "%s", jnNetDNS1);
    cfg_get_string(pathNetCfg, ifconfig.info.strDNS2, "%s", jnNetDNS2);
}

void ifconfig_init(void)
{
    uint8_t tmp = 1;
    memset(&ifconfig, 0, sizeof(ifconfig_t));
//    cfg_set_uint8(pathNetCfg, &tmp, "%s", jnNetDHCP);
//    tmp = 2;
//    cfg_set_uint8(pathNetCfg, &tmp, "%s", jnNetAdapter);
    ifconfig_get();
    
}
