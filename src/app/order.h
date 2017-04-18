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
#include "gdsl_list.h"

typedef enum _OrderState
{
    STATE_ORDER_IDLE,
    STATE_ORDER_TMP,
    STATE_ORDER_WAITSTART,
    STATE_ORDER_MAKE,
    STATE_ORDER_UPDATE,
    STATE_ORDER_FINISH
}OrderState_t;
typedef enum _OrderSegState
{
    STATE_ORDERSEG_DEF,          //����û�н���SEG,��Ĭ�϶���
    STATE_ORDERSEG_START,//������һ�ν���SEG
    STATE_ORDERSEG_IN,

    STATE_ORDERSEG_END
}OrderSegState_t;
typedef enum _ChargeSegState
{
    STATE_SEG_START,//��ǰSEG��һ�ν���
    STATE_SEG_IN,
    STATE_SEG_END
}ChargeSegState_t;

typedef struct _ChargeSegment
{
    time_t tStartTime;
    time_t tEndTime;
    double dSegStartPower;
    double dSegPower;
    double dSegFee;
    ChargeSegState_t state;
    uint8_t ucTemplPos;//��¼��ǰ����Templ�е�λ�ã������жϵ�ǰʱ���Ƿ��ڵ�ǰ����
}ChargeSegment_t;

typedef struct _OrderData
{
    OrderState_t statOrder;//��¼����״̬
    OrderSegState_t statOrderSeg;//��¼����ʱ���״̬

    uint8_t ucCardID[defCardIDLength];    //����//��taskrfid�и�ֵ
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 2:Ƿ�� 0��δע�Ῠ
    double  dBalance;           //���
    uint8_t ucCONID;
    //����ʱ
    time_t tStartTime;                    //�������ʱ��
    double  dStartPower;
    uint8_t ucServiceFeeType;           //���������
    //������
    double  dTotalPower;                  //�ܵ���
    double  dTotalPowerFee;             //�ܵ��
    double  dTotalServiceFee;                //�ܷ����
    double  dTotalFee;                      //�ܷ���
    uint8_t ucTotalSegment;             //�����ϸ����
    double  dDefSegStartPower;          //Ĭ�϶���ʼ�������˱���Ϊ�м����������Ϊ��¼ÿһ�ν���Ĭ�϶�ʱ����ʼ����
    double  dDefSegPower;               //Ĭ�϶ε���
    double  dDefSegFee;                //Ĭ�϶ε��
    gdsl_list_t plChargeSegment;
    //ֹͣʱ
    uint8_t ucPayType;                    //֧����ʽ
    uint8_t ucStopType;                   //ֹͣ����

    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;

void OrderCreate(OrderData_t *pOrder);
void OrderInit(OrderData_t *pOrder);

#endif
