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
    uint8_t ucCONID;                    //接口ID 0~255
    uint8_t ucChargeState;  //0x00   表示不可用（未注册）
                            //0x01   表示空闲
                            //0x02   表示被预约
                            //0x03   表示车枪链接（不正常）
                            //0x04   表示枪车连接正常，未充电
                            //0x05   充电中
                            //0x0f   故障（不可用）
    double dChargingVoltage;
    double dChargingCurrent;
    double dTotalPower;     //已充电量
    uint32_t ulTotalTime;      //已充时间(秒)
    double dTemp;
    double dTotalFee;
    uint8_t ucServiceFeeType;
    double  dServiceFee;                //服务费
    uint32_t ulRemainTime;      //剩余充电时间
    uint8_t ucSOC;             //已充SOC 1%~100%
}Heartbeat_t;


ErrorCode_t RemoteRegist(EVSE_t *pEVSE, echProtocol_t *pProto);
ErrorCode_t RemoteRegistRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteHeart(EVSE_t *pEVSE, echProtocol_t *pProto);
ErrorCode_t RemoteHeartRes(EVSE_t *pEVSE, echProtocol_t *pProto, int *psiRetVal );
ErrorCode_t RemoteStatus(EVSE_t *pEVSE, echProtocol_t *pProto, CON_t *pCON);
ErrorCode_t RemoteGetTime(struct tm *pTimeBlock);
ErrorCode_t RemoteGetBalance(uint8_t *pucID, uint8_t ucIDLength, uint8_t *pucAccountStatus, double *pdBalance);

#endif
