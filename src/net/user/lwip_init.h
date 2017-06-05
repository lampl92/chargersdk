/**
* @file lwip_init.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-05-02
*/
#ifndef  __LWIP_INIT_H
#define  __LWIP_INIT_H

extern int ppp;

int lwip_init_task(void);
void ppp_on_status(void *ctx, int errCode, void *arg);

#endif
