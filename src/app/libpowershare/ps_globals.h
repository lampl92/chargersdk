/**
* @file ps_globals.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-23
*/
#ifndef __PS_GLOBALS_H
#define __PS_GLOBALS_H

typedef struct _ChargeSegment
{
    uint8_t ucElectricity;
    uint8_t ucFee;
}ChargeSegment_t;

typedef struct _Order
{
    uint8_t ucEquipmentID[8];           //�豸ID
    uint8_t ucConnectorID;              //�ӿ�ID
    uint8_t ucElectricityTotal[4];      //�ܵ���
    uint8_t ucPayment;                  //�������
    uint8_t ucStopType;                 //ֹͣ����
    uint8_t ucReserved;                 //����
    uint8_t ucCardID[8];                //����
    uint8_t ucFeeTotal;                 //�ܷ���
    uint8_t ucOrderSN[7];               //������ˮ��
    uint8_t ucServiceFeeType;           //���������
    uint8_t ucServiceFee[2];            //�����
    uint8_t ucSegmentTotal;             //�����ϸ����
    uint8_t ucDefaultSegElectricity;    //Ĭ�϶ε���
    uint8_t ucDefaultSegFee;            //Ĭ�϶ε��
    ChargeSegment_t *pChargeSegment;
}Order_t

#endif /* __PS_GLOBALS_H */
