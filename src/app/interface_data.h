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
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    double  dBalance;           //���
    uint8_t ucCONID;          //�ӿ�ID 0~255
    uint8_t ucCardID[defCardIDLength];    //����//��taskrfid�и�ֵ
}RfidOrderData_t;


ErrorCode_t CreateOrderFile(void);

#endif
