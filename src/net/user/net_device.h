#include "core/net.h"
typedef struct _net_device
{
    NetInterface *interface;                          ///<Underlying network interface
    char name[16];
}net_device_t;
