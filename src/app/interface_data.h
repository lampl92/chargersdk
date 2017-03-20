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


typedef struct _ChargeSegment
{
    uint8_t strStartTime[7];
    uint8_t strStopTime[7];
    double dSegPower;
    double dSegFee;
}ChargeSegment_t;

typedef struct _OrderData
{
    uint8_t OrderState;         //1:�����У�2������У�3��ֹͣ�У�4���ѽ�����5��δ֪
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    double  dBalance;           //���

    double  dTotalPower;                  //�ܵ���
    uint8_t ucPaymentType;                //֧����ʽ
    uint8_t ucStopType;                   //ֹͣ����
    uint8_t ucReserved;                   //����
    uint8_t ucCardID[defCardIDLength];    //����//��taskrfid�и�ֵ
    double  dTotalFee;                 //�ܷ���
    uint8_t strOrderSN[7];             //������ˮ��
    uint8_t ucServiceFeeType;          //���������
    double  dServiceFee;                //�����
    uint8_t ucTotalSegment;             //�����ϸ����
    double  dDefSegPower;               //Ĭ�϶ε���
    double  dDefSegFee;                //Ĭ�϶ε��
    UserList_t *pChargeSegment;
}OrderData_t;

typedef struct _RfidOrderData
{
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    double  dBalance;           //���
    uint8_t ucCONID;          //�ӿ�ID 0~255
    uint8_t ucCardID[defCardIDLength];    //����//��taskrfid�и�ֵ
}RfidOrderData_t;


ErrorCode_t CreateOrderFile(void);

#endif
