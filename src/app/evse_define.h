
#ifndef _EVSE_DEFINE_H
#define _EVSE_DEFINE_H

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
#define defEventBitOrderTmp                      BIT_0    //获取刷卡数据后通知taskdata将tmpOrder填充到枪的order中。
#define defEventBitOrderMakeOK                   BIT_1    //充电前数据准备完成, Clear in makeCmdCardCtrlResBodyCtx
#define defEventBitOrderUpdateOK                 BIT_2

#define defEventBitOrder_HMIDispOK               BIT_3
#define defEventBitOrder_RemoteOrderOK           BIT_4
#define defEventBitOrder_RemoteRTDataOK          BIT_5
#define defEventBitOrder_StoreOK                 BIT_6

#define defEventBitOrderStopTypeCurr             BIT_7
#define defEventBitOrderStopTypeScram            BIT_8
#define defEventBitOrderStopTypeLimitFee         BIT_9
#define defEventBitOrderStopTypeLimitTime        BIT_10
#define defEventBitOrderStopTypeLimitPower       BIT_11
#define defEventBitOrderStopTypeRemoteStop       BIT_12
#define defEventBitOrderStopTypeRFIDStop         BIT_13
#define defEventBitOrderStopTypeFull             BIT_14
#define defEventBitOrderStopTypeUnPlug           BIT_15

#define defEventBitOrderMakeFinish               BIT_16  //等待处不清除, 该事件置位后整个订单完成
//#define defEventBitOrderFinishToChargetask       BIT_17
#define defEventBitOrderFinishToHMI              BIT_18

#define defEventBitOrderTmpTimer                 BIT_19

#define defEventBitOrderUseless      (defEventBitOrder_RemoteOrderOK | \
                                      defEventBitOrder_RemoteRTDataOK)

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
#define defEventBitHMI_ChargeReqClick       BIT_3
#define defEventBitHMI_ChargeReqClickOK     BIT_4
#define defEventBitHMI_ChargeReqLockLcdOK   BIT_5
#define defEventBitHMI_RFIDOLD              BIT_6
#define defEventBitHMI_UP_FAILD             BIT_7
    

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
#define defEventBitExceptionTempW       BIT_0   
#define defEventBitExceptionTempC       BIT_1   //Critical
#define defEventBitExceptionVolt        BIT_2
//#define defEventBitExceptionCurr        BIT_3
#define defEventBitExceptionVoltTimer   BIT_4
#define defEventBitExceptionCurrTimer   BIT_5
#define defEventBitExceptionChargeTimer BIT_6
#define defEventBitExceptionRFID        BIT_7
#define defEventBitExceptionMeter       BIT_8
#define defEventBitExceptionFreqTimer   BIT_9

#define defEventBitExceptionLimitFee    BIT_10  //把LimitFee放在这里，Exception名字虽说有点不搭，但都是满足条件即停止充电。
#define defEventBitExceptionLimitTime   BIT_16  //把LimitTime放在这里，Exception名字虽说有点不搭，但都是满足条件即停止充电。
#define defEventBitExceptionRemoteStop  BIT_11
#define defEventBitExceptionRFIDStop    BIT_12  //刷卡停止
#define defEventBitExceptionCPSwitch    BIT_13
#define defEventBitExceptionTempSensor  BIT_14
#define defEventBitExceptionSocketTempSensor  BIT_15

#define defEventBitExceptionDevFault    (defEventBitExceptionRFID | \
                                         defEventBitExceptionMeter)

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
#define defEventBitChargeRTDataTimer    BIT_19


#define defEventBitEVSEReady            (defEventBitEVSEScramOK |    \
                                        defEventBitEVSEPEOK |       \
                                        defEventBitEVSEKnockOK |    \
                                        defEventBitEVSEArresterOK | \
                                        defEventBitEVSEPowerOffOK)
//(defEventBitCONLocked |
#define defEventBitCPSwitchCondition    (defEventBitCONVoltOK |      \
                                        defEventBitCONCurrOK | \
                                        defEventBitCONFreqOK | \
                                        defEventBitCONSocketTempOK |  \
                                        defEventBitCONACTempOK |    \
                                        defEventBitCONPlugOK | \
                                        defEventBitEVSEReady)

#define defEventBitChargeCondition      (defEventBitCPSwitchCondition | \
                                        defEventBitCONS2Closed | \
                                        defEventBitCONAuthed | \
                                        defEventBitCONLocked)



/*------xHandleEventTimerCBNotify*/
#define defEventBitTimerCBTemp              BIT_0
#define defEventBitTimerCBLockState         BIT_1
#define defEventBitTimerCBPlugState         BIT_2
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
    
////////////////////////////////////////////
//Signal原则：0-关  1-开  0-正常  1-异常  
////////////////////////////////////////////
/*EVSE Signal Pool*/
    //ulSignalState
#define defSignalEVSE_State_Network_Online      BIT_0//
#define defSignalEVSE_State_Network_Registed    BIT_1//
    //ulSignalAlarm
#define defSignalEVSE_Alarm_Scram                BIT_0//
#define defSignalEVSE_Alarm_Knock                BIT_1//
#define defSignalEVSE_Alarm_PE                   BIT_2//
#define defSignalEVSE_Alarm_PowerOff             BIT_3//
#define defSignalEVSE_Alarm_Arrester             BIT_4//
#define defSignalEVSE_Alarm_AC_A_Temp_War        BIT_5        //
#define defSignalEVSE_Alarm_AC_B_Temp_War        BIT_6        //
#define defSignalEVSE_Alarm_AC_C_Temp_War        BIT_7        //
#define defSignalEVSE_Alarm_AC_N_Temp_War        BIT_8        //
#define defSignalEVSE_Alarm_AC_A_Temp_Cri        BIT_9        //
#define defSignalEVSE_Alarm_AC_B_Temp_Cri        BIT_10       //
#define defSignalEVSE_Alarm_AC_C_Temp_Cri        BIT_11       //
#define defSignalEVSE_Alarm_AC_N_Temp_Cri        BIT_12       //
    //ulSignalFault
#define defSignalEVSE_Fault_RFID                BIT_0//
#define defSignalEVSE_Fault_Bluetooth           BIT_1
#define defSignalEVSE_Fault_Wifi                BIT_2
#define defSignalEVSE_Fault_GPRS                BIT_3
#define defSignalEVSE_Fault_GSensor             BIT_4
#define defSignalEVSE_Fault_AC_A_Temp           BIT_5
#define defSignalEVSE_Fault_AC_B_Temp           BIT_6
#define defSignalEVSE_Fault_AC_C_Temp           BIT_7
#define defSignalEVSE_Fault_AC_N_Temp           BIT_8
/*CON Signal Pool*/
    //ulSignalState
#define defSignalCON_State_Standby              BIT_0//
#define defSignalCON_State_Working              BIT_1//
#define defSignalCON_State_Stopping             BIT_2//
#define defSignalCON_State_Fault                BIT_3//
#define defSignalCON_State_S2                   BIT_4        // 
#define defSignalCON_State_CC                   BIT_5        //CC 状态 
#define defSignalCON_State_Plug                 BIT_6        //插枪状态
#define defSignalCON_State_SocketLock           BIT_7        //B型枪锁状态
#define defSignalCON_State_AC_A_Relay           BIT_8        //A(L)继电器状态
#define defSignalCON_State_AC_B_Relay           BIT_9        //B
#define defSignalCON_State_AC_C_Relay           BIT_10        //C
#define defSignalCON_State_AC_N_Relay           BIT_11        //N 继电器状态
#define defSignalCON_State_CurrUpProtection     BIT_12       //过流保护动作@

    //ulSignalAlarm
#define defSignalCON_Alarm_SocketLock           BIT_0
#define defSignalCON_Alarm_SocketTemp1_War      BIT_1        //
#define defSignalCON_Alarm_SocketTemp2_War      BIT_2        //
#define defSignalCON_Alarm_SocketTemp1_Cri      BIT_3        //
#define defSignalCON_Alarm_SocketTemp2_Cri      BIT_4        //
#define defSignalCON_Alarm_AC_A_Temp_War        BIT_5        //
#define defSignalCON_Alarm_AC_B_Temp_War        BIT_6        //
#define defSignalCON_Alarm_AC_C_Temp_War        BIT_7        //
#define defSignalCON_Alarm_AC_N_Temp_War        BIT_8        //
#define defSignalCON_Alarm_AC_A_Temp_Cri        BIT_9        //
#define defSignalCON_Alarm_AC_B_Temp_Cri        BIT_10       //
#define defSignalCON_Alarm_AC_C_Temp_Cri        BIT_11       //
#define defSignalCON_Alarm_AC_N_Temp_Cri        BIT_12       //
#define defSignalCON_Alarm_AC_A_VoltUp          BIT_13       //
#define defSignalCON_Alarm_AC_B_VoltUp          BIT_14       //*
#define defSignalCON_Alarm_AC_C_VoltUp          BIT_15       //*
#define defSignalCON_Alarm_AC_A_VoltLow         BIT_16       //
#define defSignalCON_Alarm_AC_B_VoltLow         BIT_17       //*
#define defSignalCON_Alarm_AC_C_VoltLow         BIT_18       //*
#define defSignalCON_Alarm_AC_A_CurrUp_War      BIT_19       //
#define defSignalCON_Alarm_AC_B_CurrUp_War      BIT_20       //*
#define defSignalCON_Alarm_AC_C_CurrUp_War      BIT_21       //*
#define defSignalCON_Alarm_AC_A_CurrUp_Cri      BIT_22       //
#define defSignalCON_Alarm_AC_B_CurrUp_Cri      BIT_23       //*
#define defSignalCON_Alarm_AC_C_CurrUp_Cri      BIT_24       //*
#define defSignalCON_Alarm_AC_Freq_Cri        BIT_25       //

    //ulSignalFault
#define defSignalCON_Fault_SocketLock           BIT_0
#define defSignalCON_Fault_AC_A_Temp            BIT_1
#define defSignalCON_Fault_AC_B_Temp            BIT_2
#define defSignalCON_Fault_AC_C_Temp            BIT_3
#define defSignalCON_Fault_AC_N_Temp            BIT_4
#define defSignalCON_Fault_RelayPaste           BIT_5
//#define defSignalCON_Fault_AC_B_RelayPaste      BIT_6
//#define defSignalCON_Fault_AC_C_RelayPaste      BIT_7
//#define defSignalCON_Fault_AC_N_RelayPaste      BIT_8
#define defSignalCON_Fault_CP                   BIT_9        //CP传感故障
#define defSignalCON_Fault_Plug                 BIT_10
#define defSignalCON_Fault_Meter                BIT_11       //电表或电能芯片

//#define defSignalGroupCON_Fault_AC_RelayPaste       (defSignalCON_Fault_AC_A_RelayPaste | \
//                                                    defSignalCON_Fault_AC_B_RelayPaste | \
//                                                    defSignalCON_Fault_AC_C_RelayPaste | \
//                                                    defSignalCON_Fault_AC_N_RelayPaste )
    
#define defSignalGroupCON_Alarm_Temp_War           (defSignalCON_Alarm_SocketTemp1_War | \
                                                defSignalCON_Alarm_SocketTemp2_War | \
                                                defSignalCON_Alarm_AC_A_Temp_War | \
                                                defSignalCON_Alarm_AC_B_Temp_War | \
                                                defSignalCON_Alarm_AC_C_Temp_War | \
                                                defSignalCON_Alarm_AC_N_Temp_War)   
#define defSignalGroupCON_Alarm_Temp_Cri           (defSignalCON_Alarm_SocketTemp1_Cri | \
                                                defSignalCON_Alarm_SocketTemp2_Cri | \
                                                defSignalCON_Alarm_AC_A_Temp_Cri | \
                                                defSignalCON_Alarm_AC_B_Temp_Cri | \
                                                defSignalCON_Alarm_AC_C_Temp_Cri | \
                                                defSignalCON_Alarm_AC_N_Temp_Cri)
#define defSignalGroupEVSE_Alarm_Temp_War      (defSignalEVSE_Alarm_AC_A_Temp_War | \
                                                defSignalEVSE_Alarm_AC_B_Temp_War | \
                                                defSignalEVSE_Alarm_AC_C_Temp_War | \
                                                defSignalEVSE_Alarm_AC_N_Temp_War)
#define defSignalGroupEVSE_Alarm_Temp_Cri      (defSignalEVSE_Alarm_AC_A_Temp_Cri | \
                                                defSignalEVSE_Alarm_AC_B_Temp_Cri | \
                                                defSignalEVSE_Alarm_AC_C_Temp_Cri | \
                                                defSignalEVSE_Alarm_AC_N_Temp_Cri)
#define CON_MAX_SIGNAL_BLOCK            4
#define EVSE_MAX_SIGNAL_BLOCK           2
    
#endif