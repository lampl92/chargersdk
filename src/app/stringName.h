/**
* @file stringName.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-24
*/
#ifndef STRINGNAME_H_INCLUDED
#define STRINGNAME_H_INCLUDED

#define pathOrder       "system\\order.txt"
#define pathEVSELog     "system\\evse.log"
#define pathEVSECfg     "system\\evse.cfg"
#define pathSysconf     "system\\sysconf.cfg"
#define pathProtoCfg    "system\\protocol.cfg"
#define pathWhiteList   "system\\whitelist.cfg"
#define pathBlackList   "system\\blacklist.cfg"


#define jnEVSESN        "EVSESN"
#define jnEVSEID        "EVSEID"
#define jnEVSEType      "EVSEType"
#define jnTotalCON      "TotalCON"
#define jnLng            "Lng"
#define jnLat            "Lat"
#define jnServiceFeeType         "ServiceFeeType"
#define jnServiceFee             "ServiceFee"
#define jnDefSegFee              "DefSegFee"
#define jnCONArray               "CONArray"
/*CONArray Item*/
#define jnID                        "ID"
#define jnType                      "Type"
#define jnSocketType                "SocketType"
#define jnVolatageUpperLimits       "VolatageUpperLimits"
#define jnVolatageLowerLimits       "VolatageLowerLimits"
#define jnACTempUpperLimits         "ACTempUpperLimits"
#define jnACTempLowerLimits         "ACTempLowerLimits"
#define jnSocketTempUpperLimits     "SocketTempUpperLimits"
#define jnSocketTempLowerLimits     "SocketTempLowerLimits"
#define jnRatedCurrent              "RatedCurrent"
#define jnRatedPower                "RatedPower"
#define jnQRCode                    "QRCode"
/*end of CONArray Item*/



#define jnCardID            "CardID"
#define jnIndex             "Index"
#define jnOrder             "Order"
#define jnMaxIndex          "MaxIndex"
#define jnTemplSegArray     "TemplSegArray"
#define jnStartTime         "StartTime"
#define jnEndTime           "EndTime"
#define jnSegFee            "SegFee"

#define jnOrderStartType                 "StartType"
#define jnOrderOrderSN                   "OrderSN"
#define jnOrderCONID                     "CONID"
#define jnOrderCardID                    "CardID"
#define jnOrderStartPower                "StartPower"
#define jnOrderStopPower                 "StopPower"
#define jnOrderTotalPowerFee             "TotalPowerFee"
#define jnOrderTotalServFee              "TotalServFee"
#define jnOrderPowerFee_sharp            "PowerFee_sharp"
#define jnOrderServFee_sharp             "ServFee_sharp"
#define jnOrderTotalPower_sharp          "TotalPower_sharp"
#define jnOrderTotalPowerFee_sharp       "TotalPowerFee_sharp"
#define jnOrderTotalServFee_sharp        "TotalServFee_sharp"
#define jnOrderTotalTime_sharp           "TotalTime_sharp"
#define jnOrderPowerFee_peak             "PowerFee_peak"
#define jnOrderServFee_peak              "ServFee_peak"
#define jnOrderTotalPower_peak           "TotalPower_peak"
#define jnOrderTotalPowerFee_peak        "TotalPowerFee_peak"
#define jnOrderTotalServFee_peak         "TotalServFee_peak"
#define jnOrderTotalTime_peak            "TotalTime_peak"
#define jnOrderPowerFee_shoulder         "PowerFee_shoulder"
#define jnOrderServFee_shoulder          "ServFee_shoulder"
#define jnOrderTotalPower_shoulder       "TotalPower_shoulder"
#define jnOrderTotalPowerFee_shoulder    "TotalPowerFee_shoulder"
#define jnOrderTotalServFee_shoulder     "TotalServFee_shoulder"
#define jnOrderTotalTime_shoulder        "TotalTime_shoulder"
#define jnOrderPowerFee_off_peak         "PowerFee_off_peak"
#define jnOrderServFee_off_peak          "ServFee_off_peak"
#define jnOrderTotalPower_off_peak       "TotalPower_off_peak"
#define jnOrderTotalPowerFee_off_peak    "TotalPowerFee_off_peak"
#define jnOrderTotalServFee_off_peak     "TotalServFee_off_peak"
#define jnOrderTotalTime_off_peak        "TotalTime_off_peak"
#define jnOrderStartTime                 "StartTime"
#define jnOrderTotalTime                 "TotalTime"
#define jnOrderStopType                  "StopType"
#define jnOrderSOC                       "SOC"
#define jnOrderPayStatus                 "PayStatus"
#define jnOrderStopTime                  "StopTime"

/*protocol.cfg*/
#define jnProtoServerIP            "ServerIP"
#define jnProtoServerPort          "ServerPort"
#define jnProtoUserName            "UserName"
#define jnProtoUserPwd             "UserPwd"
#define jnProtoKey                 "Key"
#define jnProtoNewKey              "NewKey"
#define jnProtoNewKeyChangeTime    "NewKeyChangeTime"
#define jnProtoOptSN               "OptSN"
#define jnProtoProtoVer            "ProtoVer"
#define jnProtoHeartBeatCyc_ms     "HeartBeatCyc_ms"
#define jnProtoStatusCyc_ms        "StatusCyc_ms"
#define jnProtoRTDataCyc_ms        "RTDataCyc_ms"
#define jnProtoResetAct            "ResetAct"
#define jnProtoPowerFee_sharp      "PowerFee_sharp"
#define jnProtoPowerFee_peak       "PowerFee_peak"
#define jnProtoPowerFee_shoulder   "PowerFee_shoulder"
#define jnProtoPowerFee_off_peak   "PowerFee_off_peak"
#define jnProtoServFee_sharp       "ServiceFee_sharp"
#define jnProtoServFee_peak        "ServiceFee_peak"
#define jnProtoServFee_shoulder    "ServiceFee_shoulder"
#define jnProtoServFee_off_peak    "ServiceFee_off_peak"
#define jnProtoSegTime_sharp       "SegTime_sharp"
#define jnProtoSegTime_peak        "SegTime_peak"
#define jnProtoSegTime_shoulder    "SegTime_shoulder"
#define jnProtoSegTime_off_peak    "SegTime_off_peak"
#define jnProtoSegCont             "SegCont"

//#define jnProtoStart(n)              "Start"#n
//#define jnProtoEnd(n)                "End"#n

extern const uint8_t	*jnProtoSegStart[];
extern const uint8_t	*jnProtoSegEnd[];

#endif
