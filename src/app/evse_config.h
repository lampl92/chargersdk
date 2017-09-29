/**
* @file evse_config.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#ifndef __EVSE_CONFIG_H
#define __EVSE_CONFIG_H

#define defTotalCON             2
//#define defUIDLength            4
#define defCardIDLength         8
#define defEVSESNLength         64
#define defEVSEIDLength         64
#define defFileNameLength       64
#define defNodeNameLength       64
#define defQRCodeLength         64
#define defOrderSNLength        32
#define defSoftVerLength        32

/* 单位：ms */
#define defMonitorTempCyc           5000
#define defMonitorLockStateCyc      1000
#define defMonitorPlugStateCyc      50
#define defMonitorChargingDataCyc   50
#define defMonitorEVSEStateCyc      50
#define defMonitorRFIDCyc           1000
#define defMonitorDataRefreshCyc    1000
#define defDiagVoltDummyCyc         3000
#define defDiagVoltRecoverCyc       5000
#define defDiagCurrInitCyc          500       //延时启动电流检测
#define defDiagCurrDummyCyc         2000
#define defChargeAntiShakeCyc       1000      //插枪防抖检测，状态1'->状态1时的延时
#define defRelayDelay               500
#define defRemoteHeartbeatCyc       15000     //心跳
#define defRemoteStatusCyc          120000    //状态上报
#define defRemoteRTDataCyc          10000     //充电实时数据

#define defMonitorTempPeriod        10  //(℃)
#define defMonitorVoltPeriod        10  //(V)
#define defMonitorCurrPeriod        1.1 //(倍)           //when rate is 32 then up is 32 + 3.2 //(A)
#define defMonitorFreqLower         50  //(Hz)
#define defMonitorFreqUpper         50  //(Hz)
#define defMonitorFreqPeriod        1   //(Hz)

#define defCfgOrderMaxItem          20
#define defCfgOrderRemoveOldItem    10
#define defCfgLogMaxItem            50
#define defCfgLogRemoveOldItem      10

#endif /* EVSE_CONFIG_H_INCLUDED */
