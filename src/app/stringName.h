/**
* @file stringName.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-24
*/
#ifndef STRINGNAME_H_INCLUDED
#define STRINGNAME_H_INCLUDED

#define pathOrder           "/nand/system/order.txt"
#define pathOrderTmp        "/nand/system/order.tmp"
#define pathEVSELog         "/nand/system/evse.log"
#define pathEVSECfg         "/nand/system/evse.cfg"
#define pathSysCfg          "/nand/system/sys.cfg"
#define pathFTPCfg          "/nand/system/ftp.cfg"
#define pathProtoCfg        "/nand/system/protocol.cfg"
#define pathWhiteList       "/nand/system/whitelist.cfg"
#define pathBlackList       "/nand/system/blacklist.cfg"
#define pathBin             "/nand/system/chargesdk.bin.new"

#define pathSignalImage0            "/nand/system/SignalImage0.bmp"
#define pathSignalImage1            "/nand/system/SignalImage1.bmp"
#define pathSignalImage2            "/nand/system/SignalImage2.bmp"
#define pathSignalImage3            "/nand/system/SignalImage3.bmp"
#define pathSignalImage4            "/nand/system/SignalImage4.bmp"
#define pathSignalImage5            "/nand/system/SignalImage5.bmp"
        
#define pathHomeImage                "/nand/system/HomeImage.bmp"
        
#define pathCardInfoImage           "/nand/system/CardInfoImage.bmp"
#define pathGetCardInfoImage        "/nand/system/GetCardInfoImage.bmp"
#define pathCardUnregisteredImage   "/nand/system/CardUnregisteredImage.bmp"
#define pathCardArrearsImage        "/nand/system/CardArrearsImage.bmp"
#define pathPleaseConnectPlugImage  "/nand/system/PleaseConnectPlugImage.bmp"
#define pathCardInfoVoidImage       "/nand/system/CardInfoVoidImage.bmp"
        
#define pathChargingImage           "/nand/system/ChargingImage.bmp"
#define pathCartoonImage0           "/nand/system/CartoonImage0.bmp"
#define pathCartoonImage1           "/nand/system/CartoonImage1.bmp"
#define pathCartoonImage2           "/nand/system/CartoonImage2.bmp"
#define pathCartoonImage3           "/nand/system/CartoonImage3.bmp"
#define pathCartoonImage4           "/nand/system/CartoonImage4.bmp"
#define pathCartoonImage5           "/nand/system/CartoonImage5.bmp"
#define pathStopByCardImage         "/nand/system/StopByCardImage.bmp"
#define pathStopByQRImage           "/nand/system/StopByQRImage.bmp"
#define pathChargingVoidImage       "/nand/system/ChargingVoidImage.bmp"

#define pathChargeDoneImage         "/nand/system/ChargeDoneImage.bmp"
#define pathOrderUploadImage        "/nand/system/OrderUploadImage.bmp"
#define pathNormalDoneImage         "/nand/system/NormalDoneImage.bmp"
#define pathFullDoneImage           "/nand/system/FullDoneImage.bmp"
#define pathDevErrDoneImage         "/nand/system/DevErrDoneImage.bmp"
#define pathMoneyNotEnoughDoneImage     "/nand/system/MoneyNotEnoughDoneImage.bmp"
#define pathChargeDoneVoidImage     "/nand/system/ChargeDoneVoidImage.bmp"
        
#define pathAdvertisementImage      "/nand/system/AdvertisementImage.bmp"

#define pathCheckboxDta             "/nand/system/chosen.dta"
#define pathCheckboxNotDta          "/nand/system/notchosen.dta"

#define pathstSIF12                 "/nand/system/stSIF12.sif"
#define pathstSIF16                 "/nand/system/stSIF16.sif"
#define pathstSIF24                 "/nand/system/stSIF24.sif"
#define pathstSIF36                 "/nand/system/stSIF36.sif"

#define YAFFS_MOUNT_POINT   "/nand/"
#define pathSystemDir       "/nand/system/"


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

/*order.txt*/
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

/*evse.log*/
#define jnLogTime        "Time"
#define jnLogDevice      "Device"
#define jnLogLevel       "Level"
#define jnLogState       "State"
#define jnLogMessage     "Message"

/*ftp.cfg*/
#define jnFtpServer         "Server"
#define jnFtpPort           "Port"
#define jnFtpUsername       "Username"
#define jnFtpPassword       "Password"
#define jnFtpNewVersion     "NewVersion"
#define jnFtpNewFilename    "NewFilename"
#define jnFtpDownloadStart  "DownloadStart"
#define jnFtpDownloadStatus "DownloadStatus"

/*sys.cfg*/
#define jnSysChargersdk_bin    "chargesdk_bin"
#define jnSysChargersdk_bin_crc32    "chargesdk_bin_crc32"
#define jnSysEvse_cfg          "evse_cfg"
#define jnSysProtocol_cfg      "protocol_cfg"
#define jnSysSys_cfg           "sys_cfg"
#define jnSysBlacklist_cfg     "blacklist_cfg"
#define jnSysWhitelist_cfg     "whitelist_cfg"
#define jnSysDispSleepTime     "DispSleepTime"
#define jnSystouchtype         "touchtype"
#define jnSysis_calibrate      "is_calibrate"
#define jnSysxfac              "xfac"
#define jnSysyfac              "yfac"
#define jnSysxoff              "xoff"
#define jnSysyoff              "yoff"
#define jnSysVersion           "Version"
#define jnSysUSE_GPRSModem         "USE_GPRSModem"
#define jnSysUSE_Meter             "USE_Meter"
#define jnSysUSE_RFID           "USE_RFID"
#define jnSysUSE_GUI           "USE_GUI"


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
