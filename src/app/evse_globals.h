/**
* @file evse_globals.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-22
*/
#ifndef  __EVSE_GLOBALS_H
#define  __EVSE_GLOBALS_H

#include "EVSE.h"
#include "interface_rfid.h"
#include "userlib_list.h"


#define SWITCH_ON   (1)
#define SWITCH_OFF  (0)

#define BIT_0  ( 1 << 0 )
#define BIT_1  ( 1 << 1 )
#define BIT_2  ( 1 << 2 )
#define BIT_3  ( 1 << 3 )
#define BIT_4  ( 1 << 4 )
#define BIT_5  ( 1 << 5 )
#define BIT_6  ( 1 << 6 )
#define BIT_7  ( 1 << 7 )
#define BIT_8  ( 1 << 8 )
#define BIT_9  ( 1 << 9 )
#define BIT_10 ( 1 << 10 )
#define BIT_11 ( 1 << 11 )
#define BIT_12 ( 1 << 12 )
#define BIT_13 ( 1 << 13 )
#define BIT_14 ( 1 << 14 )
#define BIT_15 ( 1 << 15 )
#define BIT_16 ( 1 << 16 )
#define BIT_17 ( 1 << 17 )
#define BIT_18 ( 1 << 18 )
#define BIT_19 ( 1 << 19 )
#define BIT_20 ( 1 << 20 )
#define BIT_21 ( 1 << 21 )
#define BIT_22 ( 1 << 22 )
#define BIT_23 ( 1 << 23 )
#define BIT_24 ( 1 << 24 )
#define BIT_25 ( 1 << 25 )
#define BIT_26 ( 1 << 26 )
#define BIT_27 ( 1 << 27 )
#define BIT_28 ( 1 << 28 )
#define BIT_29 ( 1 << 29 )
#define BIT_30 ( 1 << 30 )
#define BIT_31 ( 1 << 31 )

/*---------------------------------------------------------------------------/
/ xEventGroup
/---------------------------------------------------------------------------*/
//定义pRFIDDev->xHandleEventGroupRFID
#define defEventBitGotIDtoRFID         BIT_0             //获取到ID，发送到RFID任务
#define defEventBitGotIDtoHMI          BIT_1               //获取到ID，发送到HMI
#define defEventBitIsNewID             BIT_2             //此卡在本桩没有刷过
#define defEventBitGetAccountStatus     BIT_3               //获取帐户状态

//xHandleEventData
#define defEventBitAddOrder             BIT_0
#define defEventBitAddOrderOK           BIT_1
//xHandleEventRemote
#define defEventBitRemote
//xHandleEventDiag
#define defEventBitDiagTempW            BIT_0              //温度报警
#define defEventBitDiagTemp              BIT_1
#define defEventBitDiagLockState         BIT_2
#define defEventBitDiagPlugState         BIT_3
#define defEventBitDiagChargingData      BIT_4
#define defEventBitDiagEVSEState         BIT_5
//定义pCON->status.xHandleEventException
#define defEventBitExceptionTempW       BIT_0   //Warning
#define defEventBitExceptionTempC       BIT_1   //Critical
#define defEventBitExceptionVolt        BIT_2
#define defEventBitExceptionCurr        BIT_3
#define defEventBitExceptionCritical    defEventBitExceptionTempC
#define defEventBitExceptionVoltTimer   BIT_4
#define defEventBitExceptionCurrTimer   BIT_5
#define defEventBitExceptionChargeTimer BIT_6

//定义pCON->status.xHandleEventCharge
#define defEventBitCONAuthed      BIT_0       //帐户认证OK
#define defEventBitCONLocked      BIT_1
#define defEventBitCONVoltOK      BIT_2
#define defEventBitCONCurrOK      BIT_3
#define defEventBitCONFreqOK      BIT_4
#define defEventBitCONS2Closed    BIT_5
#define defEventBitCONS2Opened    BIT_6
#define defEventBitCONCCOK        BIT_7
#define defEventBitCONSocketTempOK  BIT_8
#define defEventBitCONACTempOK    BIT_9
#define defEventBitCONPlugOK      BIT_10
#define defEventBitCONStartOK     BIT_11
//#define defEventBitCONStopOK      BIT_12
#define defEventBitEVSEScramOK      BIT_13
#define defEventBitEVSEPEOK         BIT_14
#define defEventBitEVSEKnockOK      BIT_15
#define defEventBitEVSEArresterOK   BIT_16
#define defEventBitEVSEPowerOffOK   BIT_17

#define defEventBitEVSEReady        defEventBitEVSEScramOK |    \
                                    defEventBitEVSEPEOK |       \
                                    defEventBitEVSEKnockOK |    \
                                    defEventBitEVSEArresterOK | \
                                    defEventBitEVSEPowerOffOK

#define defEventBitCPSwitchCondition    defEventBitCONLocked |      \
                                        defEventBitCONVoltOK |      \
                                        defEventBitCONSocketTempOK |  \
                                        defEventBitCONACTempOK |    \
                                        defEventBitCONPlugOK | \
                                        defEventBitEVSEReady

#define defEventBitChargeCondition  defEventBitCPSwitchCondition | \
                                    defEventBitCONAuthed | \
                                    defEventBitCONCurrOK | \
                                    defEventBitCONFreqOK | \
                                    defEventBitCONS2Closed



//定义xHandleEventTimerCBNotify
#define defEventBitTimerCBTemp              BIT_0
#define defEventBitTimerCBLockState         BIT_1
#define defEventBitTimerCBPlugState         BIT_2
#define defEventBitTimerCBChargingData      BIT_3
#define defEventBitTimerCBEVSEState         BIT_4
#define defEventBitTimerCBRFID              BIT_5
#define defEventBitTimerCBDataRefresh       BIT_6
#define defEventBitTimerCBHeartbeat         BIT_7





extern EVSE_t *pEVSE;
extern UserList_t *pListCON;
extern RFIDDev_t *pRFIDDev;

#endif
