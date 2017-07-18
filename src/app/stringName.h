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
#define pathEVSECfg     "system\\evse.cfg"
#define pathSysconf     "system\\sysconf.cfg"
#define pathProtoCfg    "system\\protocol.cfg"


#define jnEVSESN        "EVSESN"
#define jnEVSEID        "EVSEID"
#define jnEVSEType      "EVSEType"
#define jnTotalCON      "TotalCON"
#define jnLng            "Lng"
#define jnLat            "Lat"
#define jnServiceFeeType        "ServiceFeeType"
#define jnServiceFee        "ServiceFee"
#define jnDefSegFee         "DefSegFee"
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

/*protocol.cfg*/
#define jnProtoServerIP            "ServerIP"
#define jnProtoServerPort          "ServerPort"
#define jnProtoUserName            "UserName"
#define jnProtoUserPwd             "UserPwd"
#define jnProtoKey                 "Key"
#define jnProtoNewKey              "NewKey"
#define jnProtoNewKeyChangeTime    "NewKeyChangeTime"
#define jnProtoProtoVer            "ProtoVer"
#define jnProtoHeartBeatCyc_ms     "HeartBeatCyc_ms"
#define jnProtoStatusCyc_ms        "StatusCyc_ms"
#define jnProtoRTDataCyc_ms        "RTDataCyc_ms"
#define jnProtoResetAct            "ResetAct"
#define jnProtoPowerFee_sharp      "PowerFee_sharp"
#define jnProtoPowerFee_peak       "PowerFee_peak"
#define jnProtoPowerFee_shoulder   "PowerFee_shoulder"
#define jnProtoPowerFee_off_peak   "PowerFee_off_peak"
#define jnProtoServiceFee_sharp    "ServiceFee_sharp"
#define jnProtoServiceFee_peak     "ServiceFee_peak"
#define jnProtoServiceFee_shoulder "ServiceFee_shoulder"
#define jnProtoServiceFee_off_peak "ServiceFee_off_peak"
#define jnProtoSegTime_sharp       "SegTime_sharp"
#define jnProtoSegTime_peak        "SegTime_peak"
#define jnProtoSegTime_shoulder    "SegTime_shoulder"
#define jnProtoSegTime_off_peak    "SegTime_off_peak"
#define jnProtoSegCont             "SegCont"

//#define jnProtoStart(n)              "Start"#n
//#define jnProtoEnd(n)                "End"#n

#endif
