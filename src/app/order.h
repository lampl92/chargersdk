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

/*ֹͣ���� StopType*/
#define defOrderStopType_Unknown        0
#define defOrderStopType_RFID           1
#define defOrderStopType_Full           2
#define defOrderStopType_Power          3
#define defOrderStopType_Time           4
#define defOrderStopType_Fee            5
#define defOrderStopType_NeedFee        6
#define defOrderStopType_Remote         7
#define defOrderStopType_NetLost        8
#define defOrderStopType_Poweroff       9
#define defOrderStopType_Scram          10
#define defOrderStopType_SocketError    11
#define defOrderStopType_SocketLost     12
#define defOrderStopType_OverVolt       13
#define defOrderStopType_UnderVolt      14
#define defOrderStopType_OverCurr       15
#define defOrderStopType_UnderCurr      16
#define defOrderStopType_Knock          17

/*֧����ʽ PayType*/
#define defOrderPayType_Online          0
#define defOrderPayType_Offline         1

/*���������*/
#define defOrderSerType_Order           0
#define defOrderSerType_Power           1

/*��ǰʱ������ʱ�������*/
typedef enum 
{
    STATE_SEG_SHARP,
    STATE_SEG_PEAK,
    STATE_SEG_SHOULDER,
    STATE_SET_OFF_PEAK
}SegTimeState_e;

/*��������״̬*/
typedef enum _OrderState
{
    STATE_ORDER_IDLE,
    STATE_ORDER_TMP,
    STATE_ORDER_WAITSTART,
    STATE_ORDER_MAKE,
    STATE_ORDER_UPDATE,
    STATE_ORDER_FINISH
}OrderState_t;

/*ÿ��ʱ�����Ҫ��¼����Ϣ*/
typedef struct _ChargeSegInfo
{
    time_t tStartTime;
    time_t tEndTime;
    double dStartPower;
    double dPower;
    double dPowerFee;
    double dServFee;
}ChargeSegInfo_t;

typedef struct _OrderData
{
    OrderState_t    statOrder;  //��¼����״̬
    SegTimeState_e statOrderSeg; //��¼����ʱ���״̬

    uint8_t ucCardID[defCardIDLength]; //����//��taskrfid�и�ֵ            2
    uint8_t ucAccountStatus;    //�ʻ�״̬ 1��ע�Ῠ 2:Ƿ�� 0��δע�Ῠ
    double  dBalance;           //���                                        3
    uint8_t ucCONID;            //4
    //����ʱ
    uint8_t strOrderSN[defOrderSNLength]; //������ˮ��       DBIdx 1

    time_t  tStartTime;         //�������ʱ��           5
    uint8_t ucStartType;        //4 �п� 5 �޿�         6
    double  dLimitFee;          //���������         7
    double  dStartPower;        //8
    uint8_t ucServiceFeeType;   //���������             //9
    //������
    double  dTotalPower;        //�ܵ���
    double  dTotalPowerFee;     //�ܵ��
    double  dTotalServiceFee;   //�ܷ����
    double  dTotalFee;          //�ܷ���
    ChargeSegInfo_t chargeSegInfo_sharp[5];     //����̲�����Ϣ
    ChargeSegInfo_t chargeSegInfo_peak[5];      //��
    ChargeSegInfo_t chargeSegInfo_shoulder[5];  //ƽ
    ChargeSegInfo_t chargeSegInfo_off_peak[5];  //��
    //ֹͣʱ
    uint8_t         ucPayType;  //֧����ʽ
    uint8_t         ucStopType; //ֹͣ����
    time_t          tStopTime;  //ֹͣʱ��          6

    void (*Delete)(struct _OrderData *pOrder);
}OrderData_t;


void OrderCreate(OrderData_t *pOrder);
void OrderInit(OrderData_t *pOrder);

#endif
