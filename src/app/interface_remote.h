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


ErrorCode_t RemoteRegist(EVSE_t *pEVSE, echProtocol_t *pProto);
ErrorCode_t RemoteRegistRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteHeart(EVSE_t *pEVSE, echProtocol_t *pProto);
ErrorCode_t RemoteHeartRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteStatus(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON);
ErrorCode_t RemoteStatusRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteRemoteCtrl(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t succ, uint8_t reason);
ErrorCode_t RemoteRemoteCtrlRes(EVSE_t *pEVSE, echProtocol_t *pProto, uint8_t *pid, uint8_t *pctrl, int *psiRetVal );
ErrorCode_t RemoteRTData(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON, uint8_t ctrl, uint8_t reason);
ErrorCode_t RemoteOrder(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON);
ErrorCode_t RemoteOrderRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );

ErrorCode_t RemoteGetTime(struct tm *pTimeBlock);
ErrorCode_t RemoteGetBalance(uint8_t *pucID, uint8_t ucIDLength, uint8_t *pucAccountStatus, double *pdBalance);

#endif
