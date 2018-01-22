/**
* @file lwip_init.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#ifndef  __INTERFACE_NETWORK_H
#define  __INTERFACE_NETWORK_H

int_t eth_init(void);
void ifconfig_update(void);
int_t eth_connect(void);

int netSend(uint8_t *pbuff, uint32_t len);
int netRecv(uint8_t *pbuff, uint32_t buff_len);

#endif
