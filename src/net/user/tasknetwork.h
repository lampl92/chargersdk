#ifndef TASKNETWORK_H
#define TASKNETWORK_H

#include "net_device.h"

void netChangeState(net_device_t *net_dev, net_state new_state);
uint32_t get_reconnect_delay_time_ms(void);
uint32_t get_reconnect_remain_time(void);

#endif // !TASKNETWORK_H
