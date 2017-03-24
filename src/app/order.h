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

typedef struct _ChargeSegment
{
    uint8_t strStartTime[7];
    uint8_t strEndTime[7];
    double dSegPower;
    double dSegFee;
}ChargeSegment_t;

typedef struct _OrderData
{
    uint8_t OrderState;         //1:�����У�2������У�3��ֹͣ�У�4���ѽ�����5��δ֪
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 0��δע�Ῠ
    double  dBalance;           //���

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
}OrderData_t;


#endif
