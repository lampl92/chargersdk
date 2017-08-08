/**
* @file D:\Documents\Projects\chargersdk\src\app\libEcharge\ech_globals.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-18
*/
#include "stm32f4xx.h"
uint8_t EchRemoteIDtoCONID(uint8_t remote_id)
{
    uint8_t id;
    if(remote_id == 0)
    {
        id = 0;
    }
    else // 1. 2. 3 ...
    {
        id = remote_id - 1;
    }
    return id;
}

uint8_t EchCONIDtoRemoteID(uint8_t con_id, uint8_t total)
{
    uint8_t id;
    if(total > 1)
    {
        id = con_id + 1;
    }
    else
    {
        id = 0;
    }
    return id;
}
