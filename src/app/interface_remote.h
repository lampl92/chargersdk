/**
* @file interface_remote.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-16
*/
#ifndef  __INTERFACE_REMOTE_H
#define  __INTERFACE_REMOTE_H

#include "libEcharge/ech_protocol.h"
#include <time.h>

typedef struct _Heartbeat
{
    uint8_t ucEVSEID[defEVSEIDLength];
    uint8_t ucCONID;                    //�ӿ�ID 0~255
    uint8_t ucChargeState;  //0x00   ��ʾ�����ã�δע�ᣩ
                            //0x01   ��ʾ����
                            //0x02   ��ʾ��ԤԼ
                            //0x03   ��ʾ��ǹ���ӣ���������
                            //0x04   ��ʾǹ������������δ���
                            //0x05   �����
                            //0x0f   ���ϣ������ã�
    double dChargingVoltage;
    double dChargingCurrent;
    double dTotalPower;     //�ѳ����
    uint32_t ulTotalTime;      //�ѳ�ʱ��(��)
    double dTemp;
    double dTotalFee;
    uint8_t ucServiceFeeType;
    double  dServiceFee;                //�����
    uint32_t ulRemainTime;      //ʣ����ʱ��
    uint8_t ucSOC;             //�ѳ�SOC 1%~100%
}Heartbeat_t;


ErrorCode_t RemoteIF_SendRegist(EVSE_t *pEVSE, echProtocol_t *pProto);
ErrorCode_t RemoteIF_RecvRegist(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );

ErrorCode_t RemoteIF_SendHeart(EVSE_t *pEVSE, echProtocol_t *pProto);
ErrorCode_t RemoteIF_RecvHeart(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );

ErrorCode_t RemoteIF_SendReset(EVSE_t *pEVSE, echProtocol_t *pProto, uint32_t succ);
ErrorCode_t RemoteIF_RecvReset(echProtocol_t *pProto, uint32_t *pulOptSN, int *psiRetVal);

ErrorCode_t RemoteIF_SendStatus(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON);
ErrorCode_t RemoteIF_RecvStatus(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );

ErrorCode_t RemoteIF_SendRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ, uint8_t reason);
ErrorCode_t RemoteIF_RecvRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pid, uint8_t *pctrl, int *psiRetVal );

ErrorCode_t RemoteIF_SendRTData(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t ctrl, uint8_t reason);
ErrorCode_t RemoteIF_SendCardRTData(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t ctrl, uint8_t reason);

ErrorCode_t RemoteIF_SendOrder(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON);
ErrorCode_t RemoteIF_RecvOrder(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );

ErrorCode_t RemoteIF_RecvSetPowerFee(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal );
ErrorCode_t RemoteIF_RecvSetServFee(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal );
ErrorCode_t RemoteIF_RecvSetCyc(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteIF_RecvSetTimeSeg(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal );
ErrorCode_t RemoteIF_RecvSetKey(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteIF_RecvSetQR(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t flag_set, int *psiRetVal );

ErrorCode_t RemoteIF_RecvReq(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal);

ErrorCode_t RemoteIF_SendCardStart(EVSE_t *pEVSE, echProtocol_t *pProto, RFIDDev_t *pRfid);
ErrorCode_t RemoteIF_RecvCardStart(echProtocol_t *pProto, RFIDDev_t *pRfid, uint8_t *pucVaild, int *psiRetVal);

ErrorCode_t RemoteIF_SendCardStartRes(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ);
ErrorCode_t RemoteIF_RecvCardStartRes(echProtocol_t *pProto, int *psiRetVal);

ErrorCode_t RemoteIF_SendCardStopRes(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON);
ErrorCode_t RemoteIF_RecvCardStopRes(echProtocol_t *pProto, int *psiRetVal);
#endif
