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
#define pathNetCfg          "/nand/system/ifconfig.cfg"
#define pathBin             "/nand/system/chargesdk.bin.new"

typedef enum _filelist
{
    FLIST_BIN = 1,
    FLIST_EVSE_CFG,
    FLIST_PROTO_CFG,
    FLIST_SYS_CFG,
    FLIST_BLACKLIST_CFG,
    FLIST_WHITELIST_CFG,
    FLIST_AD_BMP
}filelist_e;
//**********************
//公用图片路径
#define  pathQuit           "nand/system/quit.dta"
#define  pathQuitPress      "nand/system/quitpress.dta"
#define pathbackquit        "nand/system/backquit.dta"
#define pathbackquitpress   "nand/system/backquitpress.dta"
#define pathADPic           "nand/system/ADPic.dta"
//**********************

//**********
//选枪图片路径
#define pathSelectGunBack                   "nand/system/selectgunback.dta"
#define pathSelectGunAbottonNotpress        "nand/system/selectgunAbuttonnotpress.dta"
#define pathSelectGunBbottonNotpress        "nand/system/selectgunBbuttonnotpress.dta"
#define pathSelectGunAbottonPress           "nand/system/selectgunAbuttonpress.dta"
#define pathSelectGunBbottonPress           "nand/system/selectgunBbuttonpress.dta"
//*********

//************
//选模式图片

#define pathselectpatternback               "nand/system/selectpatternback.dta"
#define pathselectpatternelectricnumber             "nand/system/selectpatternelectricnumber.dta"
#define pathselectpatternfull               "nand/system/selectpatternfull.dta"
#define pathselectpatternmoneynumber                "nand/system/selectpatternmoneynumber.dta"
#define pathselectpatterntime               "nand/system/selectpatterntime.dta"

#define pathselectpatternelectricnumberpress                "nand/system/selectpatternelectricnumberpress.dta"
#define pathselectpatternfullpress              "nand/system/selectpatternfullpress.dta"
#define pathselectpatternmoneynumberpress               "nand/system/selectpatternmoneynumberpress.dta"
#define pathselectpatterntimepress              "nand/system/selectpatterntimepress.dta"

#define pathselectpatternunityuan               "nand/system/selectpatternunityuan.dta"
#define pathselectpatternunitdu               "nand/system/selectpatternunitdu.dta"
#define pathselectpatternunitfen               "nand/system/selectpatternunitfen.dta"
#define pathselectpatternunitno               "nand/system/selectpatternunitno.dta"

#define pathselectpatternkeyboard              "nand/system/selectpatternkeyboard.dta"
#define pathselectpatternkeyboardpress         "nand/system/selectpatternkeyboardpress.dta"

#define pathKeyboardback                    "nand/system/kwyboardback.dta"

#define pathKeyboard0                       "nand/system/kwyboard0.dta"
#define pathKeyboard1                       "nand/system/kwyboard1.dta"
#define pathKeyboard2                       "nand/system/kwyboard2.dta"
#define pathKeyboard3                       "nand/system/kwyboard3.dta"
#define pathKeyboard4                       "nand/system/kwyboard4.dta"
#define pathKeyboard5                       "nand/system/kwyboard5.dta"
#define pathKeyboard6                       "nand/system/kwyboard6.dta"
#define pathKeyboard7                       "nand/system/kwyboard7.dta"
#define pathKeyboard8                       "nand/system/kwyboard8.dta"
#define pathKeyboard9                       "nand/system/kwyboard9.dta"
#define pathKeyboardBackspace               "nand/system/kwyboardbackspace.dta"
#define pathKeyboardesc                     "nand/system/kwyboardesc.dta"
#define pathKeyboardok                      "nand/system/kwyboardok.dta"
#define pathKeyboardpoint                   "nand/system/kwyboardpoint.dta"

#define pathKeyboard0press                 "nand/system/kwyboard0press.dta"
#define pathKeyboard1press                 "nand/system/kwyboard1press.dta"
#define pathKeyboard2press                 "nand/system/kwyboard2press.dta"
#define pathKeyboard3press                 "nand/system/kwyboard3press.dta"
#define pathKeyboard4press                 "nand/system/kwyboard4press.dta"
#define pathKeyboard5press                 "nand/system/kwyboard5press.dta"
#define pathKeyboard6press                 "nand/system/kwyboard6press.dta"
#define pathKeyboard7press                 "nand/system/kwyboard7press.dta"
#define pathKeyboard8press                 "nand/system/kwyboard8press.dta"
#define pathKeyboard9press                 "nand/system/kwyboard9press.dta"
#define pathKeyboardBackspacepress         "nand/system/kwyboardbackspacepress.dta"
#define pathKeyboardescpress               "nand/system/kwyboardescpress.dta"
#define pathKeyboardokpress                "nand/system/kwyboardokpress.dta"
#define pathKeyboardpointpress             "nand/system/kwyboardpointpress.dta"
//*************

//卡信息图片路径
#define pathcardinfoback                "nand/system/cardinfoback.dta"
#define pathcardinfocardconditionnotok              "nand/system/cardinfocardconditionnotok.dta"
#define pathcardinfochargingok              "nand/system/cardinfochargingok.dta"
#define pathcardinfonettimeout              "nand/system/cardinfonettimeout.dta"
#define pathcardinfopleaseplug              "nand/system/cardinfopleaseplug.dta"
#define pathcardinfoplugtimeout                 "nand/system/cardinfoplugtimeout.dta"
#define pathcardinforeadystart              "nand/system/cardinforeadystart.dta"
//#define pathcardinfoarrears        "nand/system/cardinfoarrears.dta"
//#define pathcardinfoUnavailable        "nand/system/cardinfoUnavailable.dta"
//#define pathcardinfostartfail        "nand/system/cardinfostartfail.dta"
//#define pathcardinfoback        "nand/system/cardinfoback.dta"
//#define pathcardinforeadycharging        "nand/system/cardinforeadycharging.dta"
//#define pathcardinfoget        "nand/system/cardinfoget.dta"
//#define pathcardinfoplug        "nand/system/cardinfoplug.dta"
//#define pathcardinfounregister        "nand/system/cardinfounregister.dta"
//******************

//主页图片路径
#define pathhomeback                "/nand/system/homeback.dta"
#define pathhomegunAchargedone                  "/nand/system/homegunAchargedone.dta"
#define pathhomegunAcharging                "/nand/system/homegunAcharging.dta"
#define pathhomegunAerror                   "/nand/system/homegunAerror.dta"
#define pathhomegunAfree                "/nand/system/homegunAfree.dta"
#define pathhomegunBchargedone                  "/nand/system/homegunBchargedone.dta"
#define pathhomegunBcharging                "/nand/system/homegunBcharging.dta"
#define pathhomegunBerror                   "/nand/system/homegunBerror.dta"
#define pathhomegunBfree                "/nand/system/homegunBfree.dta"
#define pathhomegunlookinfo                 "/nand/system/homegunlookinfo.dta"
#define pathhomegunscancode                 "/nand/system/homegunscancode.dta"
#define pathhomegunlookinfopress                 "/nand/system/homegunlookinfopress.dta"
#define pathhomegunscancodepress                 "/nand/system/homegunscancodepress.dta"
#define pathhomegunerror                 "/nand/system/homegunerror.dta"
#define pathhomesignal0  "nand/system/homesignal0.dta"
#define pathhomesignal1  "nand/system/homesignal1.dta"
#define pathhomesignal2  "nand/system/homesignal2.dta"
#define pathhomesignal3  "nand/system/homesignal3.dta"
#define pathhomesignal4  "nand/system/homesignal4.dta"
#define pathhomesignal5  "nand/system/homesignal5.dta"
#define pathhomechargedoneinfo          "/nand/system/homechargedoneinfo.dta"
#define pathhomecharginginfo            "/nand/system/homecharginginfo.dta"
#define pathhomechargehelp          "/nand/system/homechargehelp.dta"
#define pathhomechargehelppress         "/nand/system/homechargehelppress.dta"
#define pathhomehelpinfo            "/nand/system/homehelpinfo.dta"


//提示信息页图片
#define pathchargedoneinfo      "nand/system/chargedoneinfo.dta"
#define pathcharginginfo        "nand/system/charginginfo.dta"



#define pathCheckboxDta             "/nand/system/chosen.dta"
#define pathCheckboxNotDta          "/nand/system/notchosen.dta"
#define pathCheckboxDisable          "/nand/system/checkboxdisable.dta"
#define pathBeijingDta             "/nand/system/beijing.dta"

#define pathstSIF12                 "/nand/system/stSIF12.sif"
#define pathstSIF16                 "/nand/system/stSIF16.sif"
#define pathstSIF24                 "/nand/system/stSIF24.sif"
#define pathstSIF36                 "/nand/system/stSIF36.sif"

#define pathfontwryhct12aa4         "/nand/system/fontwryhct12aa4.sif"
#define pathfontwryhct16aa4         "/nand/system/fontwryhct16aa4.sif"
#define pathfontwryhct24aa4         "/nand/system/fontwryhct24aa4.sif"
#define pathfontwryhct36aa4         "/nand/system/fontwryhct36aa4.sif"

#define pathfontwryhcg12aa4         "/nand/system/fontwryhcg12aa4.sif"
#define pathfontwryhcg16aa4         "/nand/system/fontwryhcg16aa4.sif"
#define pathfontwryhcg24aa4         "/nand/system/fontwryhcg24aa4.sif"
#define pathfontwryhcg36aa4         "/nand/system/fontwryhcg36aa4.sif"

#define pathfontwryhcg12aa4e         "/nand/system/fontwryhcg12aa4e.sif"
#define pathfontwryhcg16aa4e         "/nand/system/fontwryhcg16aa4e.sif"
#define pathfontwryhcg24aa4e         "/nand/system/fontwryhcg24aa4e.sif"
#define pathfontwryhcg36aa4e         "/nand/system/fontwryhcg36aa4e.sif"

#define pathfontwryhcg12e         "/nand/system/fontwryhcg12e.sif"
#define pathfontwryhcg16e         "/nand/system/fontwryhcg16e.sif"
#define pathfontwryhcg24e         "/nand/system/fontwryhcg24e.sif"
#define pathfontwryhcg30e         "/nand/system/fontwryhcg30e.sif"
#define pathfontwryhcg36e         "/nand/system/fontwryhcg36e.sif"

#define pathfontwryhcg44e         "/nand/system/fontwryhcg44e.sif"
#define pathfontwryhcg44aa4e         "/nand/system/fontwryhcg44aa4e.sif"


#define YAFFS_MOUNT_POINT   "/nand/"
#define pathSystemDir       "/nand/system/"
#define pathUpgradeDir      "/nand/upgrade/"

#define pathUpgradeTmp      "/nand/upgrade/upgrade.tmp"


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
#define jnOrderStartEnergy               "StartPower"
#define jnOrderStopEnergy                "StopPower"
#define jnOrderTotalEnergyFee            "TotalPowerFee"
#define jnOrderTotalServFee              "TotalServFee"
#define jnOrderEnergyFee_sharp           "PowerFee_sharp"
#define jnOrderServFee_sharp             "ServFee_sharp"
#define jnOrderTotalEnergy_sharp         "TotalPower_sharp"
#define jnOrderTotalEnergyFee_sharp      "TotalPowerFee_sharp"
#define jnOrderTotalServFee_sharp        "TotalServFee_sharp"
#define jnOrderTotalTime_sharp           "TotalTime_sharp"
#define jnOrderEnergyFee_peak            "PowerFee_peak"
#define jnOrderServFee_peak              "ServFee_peak"
#define jnOrderTotalEnergy_peak          "TotalPower_peak"
#define jnOrderTotalEnergyFee_peak       "TotalPowerFee_peak"
#define jnOrderTotalServFee_peak         "TotalServFee_peak"
#define jnOrderTotalTime_peak            "TotalTime_peak"
#define jnOrderEnergyFee_shoulder        "PowerFee_shoulder"
#define jnOrderServFee_shoulder          "ServFee_shoulder"
#define jnOrderTotalEnergy_shoulder      "TotalPower_shoulder"
#define jnOrderTotalEnergyFee_shoulder   "TotalPowerFee_shoulder"
#define jnOrderTotalServFee_shoulder     "TotalServFee_shoulder"
#define jnOrderTotalTime_shoulder        "TotalTime_shoulder"
#define jnOrderEnergyFee_off_peak        "PowerFee_off_peak"
#define jnOrderServFee_off_peak          "ServFee_off_peak"
#define jnOrderTotalEnergy_off_peak      "TotalPower_off_peak"
#define jnOrderTotalEnergyFee_off_peak   "TotalPowerFee_off_peak"
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
#define jnProtoEnergyFee_sharp      "PowerFee_sharp"
#define jnProtoEnergyFee_peak       "PowerFee_peak"
#define jnProtoEnergyFee_shoulder   "PowerFee_shoulder"
#define jnProtoEnergyFee_off_peak   "PowerFee_off_peak"
#define jnProtoServFee_sharp       "ServiceFee_sharp"
#define jnProtoServFee_peak        "ServiceFee_peak"
#define jnProtoServFee_shoulder    "ServiceFee_shoulder"
#define jnProtoServFee_off_peak    "ServiceFee_off_peak"
#define jnProtoSegTime_sharp       "SegTime_sharp"
#define jnProtoSegTime_peak        "SegTime_peak"
#define jnProtoSegTime_shoulder    "SegTime_shoulder"
#define jnProtoSegTime_off_peak    "SegTime_off_peak"
#define jnProtoSegCont             "SegCont"

/*ifconfig.cfg*/
#define jnNetAdapter        "Adapter"
#define jnNetDHCP           "DHCP"
#define jnNetHostName       "HostName"
#define jnNetMAC            "MAC"
#define jnNetIP             "IP"
#define jnNetMask           "Mask"
#define jnNetGate           "Gate"
#define jnNetDNS1           "DNS1"
#define jnNetDNS2           "DNS2"

//#define jnProtoStart(n)              "Start"#n
//#define jnProtoEnd(n)                "End"#n

extern const char *jnProtoSegStart[];
extern const char *jnProtoSegEnd[];

#endif
