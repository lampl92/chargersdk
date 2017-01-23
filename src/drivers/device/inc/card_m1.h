/**
* @file card_m1.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-23
*/
#ifndef  __CARD_M1_H
#define  __CARD_M1_H

typedef struct __CardInfo
{
    uint8_t ucUID[4];
    uint8_t ucSN[8];
    uint8_t ucOwedOrder
}CardInfo_t;

#endif
