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
#include "libEcharge/ech_protocol.h"

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
/*------pRFIDDev->xHandleEventGroupRFID*/
#define defEventBitGotIDtoRFID          BIT_0             //获取到ID，发送到RFID任务
#define defEventBitGotIDtoHMI           BIT_1               //获取到ID，发送到HMI
//#define defEventBitIsNewID              BIT_2             //此卡在本桩没有刷过
#define defEventBitGetAccountStatus     BIT_3               //获取帐户状态
#define defEventBitRFIDNewID            BIT_4
#define defEventBitRFIDOldID            BIT_5
#define defEventBitGoodIDReqDisp        BIT_6
#define defEventBitGoodIDReqDispOK      BIT_7
#define defEventBitBadIDReqDisp        BIT_8
#define defEventBitBadIDReqDispOK      BIT_9
#define defEventBitOweIDReqDisp        BIT_10
#define defEventBitOwdIDReqDispOK      BIT_11

/*------xHandleEventData*/


/*------pCON->status.xHandleEventOrder*/
#define defEventBitOrderTmp             BIT_0    //获取刷卡数据后通知taskdata将tmpOrder填充到枪的order中。
#define defEventBitOrderMakeOK          BIT_1
#define defEventBitOrderUpdateOK        BIT_2
#define defEventBitAddOrder             BIT_3
#define defEventBitAddOrderOK           BIT_4

#define defEventBitOrder_HMIDispOK      BIT_7
#define defEventBitOrder_RemoteOrderOK  BIT_8
#define defEventBitOrder_RemoteRTDataOK BIT_9
#define defEventBitOrder_StoreOK        BIT_10

#define defEventBitOrderStopTypeLimitFee    BIT_15
#define defEventBitOrderStopTypeRemoteStop  BIT_16
#define defEventBitOrderStopTypeRFIDStop    BIT_17
#define defEventBitOrderStopTypeFull        BIT_18

#define defEventBitOrderMakeFinish      BIT_19  //等待处不清除, 该事件置位后整个订单完成
#define defEventBitOrderFinishToRemote  BIT_20  //Order订单命令使用，使用后清除

#define defEventBitOrderStopType    (defEventBitOrderStopTypeLimitFee | defEventBitOrderStopTypeRemoteStop | defEventBitOrderStopTypeRFIDStop)
#define defEventBitOrderUseless      (defEventBitOrder_RemoteOrderOK | defEventBitOrder_RemoteRTDataOK | defEventBitOrder_HMIDispOK)    //(defEventBitOrder_HMIDispOK | defEventBitOrder_RemoteOK |defEventBitOrder_StoreOK)


/*------xHandleEventRemote*/
#define defEventBitRemoteGetAccount     BIT_0
#define defEventBitRemoteGotAccount     BIT_1
/*------xHandleEventTCP*/
#define defEventBitTCPReConnect         BIT_2
#define defEventBitTCPClientSendReq     BIT_3
#define defEventBitTCPClientSendOK      BIT_4
#define defEventBitTCPConnectOK         BIT_5 //接收不清除， 服务器连接成功
#define defEventBitTCPConnectFail       BIT_6 //接收主动清除
#define defEventBitTCPClientRecvValid   BIT_7
#define defEventBitTCPClientFlushBuff   BIT_8

/*------xHandleEventHMI*/
#define defEventBitHMITimeOutToRFID         BIT_0
#define defEventBitHMI_ChargeReqDispDone    BIT_1
#define defeventBitHMI_ChargeReqDispDoneOK  BIT_2
#define defEventBitHMI_ChargeReqClick       BIT_3
#define defEventBitHMI_ChargeReqClickOK     BIT_4
#define defEventBitHMI_ChargeReqLockLcdOK   BIT_5
#define defEventBitHMI_RFIDOLD              BIT_6

//#define defEventBitHMI_ChargeReqDoneOK  BIT_4

/*------xHandleEventDiag*/
#define defEventBitDiagTempW            BIT_0              //温度报警
#define defEventBitDiagTemp             BIT_1
#define defEventBitDiagLockState        BIT_2
#define defEventBitDiagPlugState        BIT_3
#define defEventBitDiagVolt             BIT_4
#define defEventBitDiagChargingData     BIT_5
#define defEventBitDiagEVSEState        BIT_6
/*------pCON->status.xHandleEventException*/
#define defEventBitExceptionTempW       BIT_0   //Warning
//#define defEventBitExceptionTempC       BIT_1   //Critical
#define defEventBitExceptionVolt        BIT_2
#define defEventBitExceptionCurr        BIT_3
#define defEventBitExceptionVoltTimer   BIT_4
#define defEventBitExceptionCurrTimer   BIT_5
#define defEventBitExceptionChargeTimer BIT_6
#define defEventBitExceptionRFID        BIT_7
#define defEventBitExceptionMeter       BIT_8
#define defEventBitExceptionRelayPaste  BIT_9

#define defEventBitExceptionLimitFee    BIT_10  //把LimitFee放在这里，Exception名字虽说有点不搭，但都是满足条件即停止充电。
#define defEventBitExceptionRemoteStop  BIT_11
#define defEventBitExceptionRFIDStop    BIT_12  //刷卡停止

#define defEventBitExceptionDevFault    (defEventBitExceptionRFID |defEventBitExceptionMeter|defEventBitExceptionRelayPaste)
#define defEventBitExceptionStopType    (defEventBitExceptionLimitFee | defEventBitExceptionRemoteStop | defEventBitExceptionRFIDStop)

/*------pCON->status.xHandleEventCharge*/
#define defEventBitCONAuthed            BIT_0       //帐户认证OK
#define defEventBitCONLocked            BIT_1
#define defEventBitCONVoltOK            BIT_2
#define defEventBitCONCurrOK            BIT_3
#define defEventBitCONFreqOK            BIT_4
#define defEventBitCONS2Closed          BIT_5
#define defEventBitCONS2Opened          BIT_6
#define defEventBitCONCCOK              BIT_7
#define defEventBitCONSocketTempOK      BIT_8
#define defEventBitCONACTempOK          BIT_9
#define defEventBitCONPlugOK            BIT_10
#define defEventBitCONStartOK           BIT_11
//#define defEventBitCONStopOK          BIT_12
#define defEventBitEVSEScramOK          BIT_13
#define defEventBitEVSEPEOK             BIT_14
#define defEventBitEVSEKnockOK          BIT_15
#define defEventBitEVSEArresterOK       BIT_16
#define defEventBitEVSEPowerOffOK       BIT_17

#define defEventBitCONOrderStart        BIT_18
#define defEventBitCONOrderFinish       BIT_19
#define defEventBitChargeRTDataTimer         BIT_20


#define defEventBitEVSEReady            (defEventBitEVSEScramOK |    \
                                        defEventBitEVSEPEOK |       \
                                        defEventBitEVSEKnockOK |    \
                                        defEventBitEVSEArresterOK | \
                                        defEventBitEVSEPowerOffOK)
//(defEventBitCONLocked |
#define defEventBitCPSwitchCondition    (defEventBitCONVoltOK |      \
                                        defEventBitCONSocketTempOK |  \
                                        defEventBitCONACTempOK |    \
                                        defEventBitCONPlugOK | \
                                        defEventBitEVSEReady)

#define defEventBitChargeCondition      (defEventBitCPSwitchCondition | \
                                        defEventBitCONAuthed | \
                                        defEventBitCONCurrOK | \
                                        defEventBitCONFreqOK | \
                                        defEventBitCONS2Closed | \
                                        defEventBitCONLocked)



/*------xHandleEventTimerCBNotify*/
#define defEventBitTimerCBTemp              BIT_0
#define defEventBitTimerCBLockState         BIT_1
#define defEventBitTimerCBPlugState         BIT_2
#define defEventBitTimerCBVolt              BIT_3
#define defEventBitTimerCBChargingData      BIT_4
#define defEventBitTimerCBEVSEState         BIT_5
#define defEventBitTimerCBRFID              BIT_6
#define defEventBitTimerCBDataRefresh       BIT_7
#define defEventBitTimerCBHeartbeat         BIT_8
#define defEventBitTimerCBStatus            BIT_9

/*pProto->pCMD[i]->xHandleEventCmd*/
#define defEventBitProtoCmdHandled          BIT_0


/*充电桩类型*/
#define defEVSEType_DC                  1
#define defEVSEType_AC                  2
#define defEVSEType_AC_DC               3
#define defEVSEType_Wireless            4
#define defEVSEType_Other               5

/*充电枪类型*/
#define defCONType_DC                   1
#define defCONType_AC                   2

/*充电桩接口类型*/
#define defSocketTypeB              ('B')//66
#define defSocketTypeC              ('C')//67

extern EVSE_t *pEVSE;
extern UserList_t *pListCON;
extern RFIDDev_t *pRFIDDev;
extern echProtocol_t *pechProto;

#endif
