/**
* @file order.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#ifndef  __ORDER_H
#define  __ORDER_H

#include <time.h>
#include "userlib_list.h"

typedef enum _OrderState
{
    STATE_ORDER_TEMP,
    STATE_ORDER_MAKE,
    STATE_ORDER_START,
    STATE_ORDER_UPDATE,
    STATE_ORDER_FINISH
}OrderState_t;


typedef struct _ChargeSegment
{
    time_t tStartTime;
    time_t tEndTime;
    double dSegStartPower;
    double dSegPower;
    double dSegFee;
}ChargeSegment_t;

typedef struct _OrderData
{
    OrderState_t state;
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 2:Ƿ�� 0��δע�Ῠ
    double  dBalance;           //���

    uint8_t ucCONID;
    double  dTotalPower;                  //�ܵ���
    uint8_t ucPayType;                    //֧����ʽ
    uint8_t ucStopType;                   //ֹͣ����
    uint8_t ucReserved;                   //����
    uint8_t ucCardID[defCardIDLength];    //����//��taskrfid�и�ֵ
    double  dTotalFee;                      //�ܷ���
    uint8_t strStartTime[7];             //������ˮ��
    uint8_t ucServiceFeeType;           //���������
    double  dServiceFee;                //�����
    uint8_t ucTotalSegment;             //�����ϸ����
    double  dDefSegPower;               //Ĭ�϶ε���
    double  dDefSegFee;                //Ĭ�϶ε��
    UserList_t *pChargeSegment;
    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;

void OrderInit(OrderData_t *pOrder);
#endif
