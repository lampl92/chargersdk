/**
* @file evse_config.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-22
*/
#ifndef __EVSE_CONFIG_H
#define __EVSE_CONFIG_H

#define defTotalPoint           2
#define defUIDLength            4
#define defEVSESNLength         24
#define defEVSEIDLength         24
#define defFileNameLength       64
#define defNodeNameLength       64

/* ��λ��ms */
#define defMonitorTempCyc           5000
#define defMonitorLockStateCyc      1000
#define defMonitorPlugStateCyc      50
#define defMonitorChargingDataCyc   50
#define defMonitorEVSEStateCyc      50
#define defMonitorRFIDCyc           500
#define defMonitorDataRefreshCyc    1000
#define defDiagVoltDummyCyc         3000
#define defDiagVoltRecoverCyc       5000
#define defDiagCurrInitCyc          5000   //��ʱ�����������
#define defDiagCurrDummyCyc         2000
#define defRelayDelay               500

#define defMonitorTempPeriod        10  //(��)
#define defMonitorVoltPeriod        10  //(V)
#define defMonitorCurrPeriod        1.5 //(A)
#define defMonitorFreqLower         50  //(Hz)
#define defMonitorFreqUpper         60  //(Hz)
#define defMonitorFreqPeriod        5   //(Hz)



#endif /* EVSE_CONFIG_H_INCLUDED */
