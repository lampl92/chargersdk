/**
* @file interface_data.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-15
*/
#ifndef  __INTERFACE_DATA_H
#define  __INTERFACE_DATA_H

#include "includes.h"



typedef struct _RfidOrderData
{
    uint8_t ucAccountStatus;    //ÕÊ»§×´Ì¬ 1£º×¢²á¿¨ 0£ºÎ´×¢²á¿¨
    double  dBalance;           //Óà¶î
    uint8_t ucCONID;          //½Ó¿ÚID 0~255
    uint8_t ucCardID[defCardIDLength];    //¿¨ºÅ//ÔÚtaskrfidÖÐ¸³Öµ
}RfidOrderData_t;


ErrorCode_t CreateOrderFile(void);

#endif
