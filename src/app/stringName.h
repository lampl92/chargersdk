/**
* @file stringName.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-24
*/
#ifndef STRINGNAME_H_INCLUDED
#define STRINGNAME_H_INCLUDED


/*************system************************/
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
#define pathMeterCfg        "/nand/system/meter.cfg"
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

/**************reource************************/
//**********************
//公用图片路径
#define  pathQuit           "nand/resource/quit.dta"
#define  pathQuitPress      "nand/resource/quitpress.dta"
#define pathbackquit        "nand/resource/backquit.dta"
#define pathbackquitpress   "nand/resource/backquitpress.dta"
#define pathADPic           "nand/resource/ADPic.dta"
//**********************

//**********
//选枪图片路径
#define pathSelectGunBack                    "nand/resource/selectgunback.dta"
#define pathSelectGunAbottonNotpress         "nand/resource/selectgunAbuttonnotpress.dta"
#define pathSelectGunBbottonNotpress         "nand/resource/selectgunBbuttonnotpress.dta"
#define pathSelectGunAbottonPress            "nand/resource/selectgunAbuttonpress.dta"
#define pathSelectGunBbottonPress            "nand/resource/selectgunBbuttonpress.dta"
#define pathSelectGunAbottonDisable          "nand/resource/selectgunAbuttondisable.dta"
#define pathSelectGunBbottonDisable          "nand/resource/selectgunBbuttondisable.dta"
//*********

//************
//选模式图片

#define pathselectpatternback                               "nand/resource/selectpatternback.dta"
#define pathselectpatternelectricnumber                     "nand/resource/selectpatternelectricnumber.dta"
#define pathselectpatternfull                               "nand/resource/selectpatternfull.dta"
#define pathselectpatternmoneynumber                        "nand/resource/selectpatternmoneynumber.dta"
#define pathselectpatterntime                               "nand/resource/selectpatterntime.dta"

#define pathselectpatternelectricnumberpress                "nand/resource/selectpatternelectricnumberpress.dta"
#define pathselectpatternfullpress                          "nand/resource/selectpatternfullpress.dta"
#define pathselectpatternmoneynumberpress                   "nand/resource/selectpatternmoneynumberpress.dta"
#define pathselectpatterntimepress                          "nand/resource/selectpatterntimepress.dta"

#define pathselectpatternunityuan                           "nand/resource/selectpatternunityuan.dta"
#define pathselectpatternunitdu                             "nand/resource/selectpatternunitdu.dta"
#define pathselectpatternunitfen                            "nand/resource/selectpatternunitfen.dta"
#define pathselectpatternunitno                             "nand/resource/selectpatternunitno.dta"

#define pathselectpatternkeyboard                           "nand/resource/selectpatternkeyboard.dta"
#define pathselectpatternkeyboardpress                      "nand/resource/selectpatternkeyboardpress.dta"

#define pathKeyboardback                                    "nand/resource/kwyboardback.dta"

#define pathKeyboard0                       "nand/resource/kwyboard0.dta"
#define pathKeyboard1                       "nand/resource/kwyboard1.dta"
#define pathKeyboard2                       "nand/resource/kwyboard2.dta"
#define pathKeyboard3                       "nand/resource/kwyboard3.dta"
#define pathKeyboard4                       "nand/resource/kwyboard4.dta"
#define pathKeyboard5                       "nand/resource/kwyboard5.dta"
#define pathKeyboard6                       "nand/resource/kwyboard6.dta"
#define pathKeyboard7                       "nand/resource/kwyboard7.dta"
#define pathKeyboard8                       "nand/resource/kwyboard8.dta"
#define pathKeyboard9                       "nand/resource/kwyboard9.dta"
#define pathKeyboardBackspace               "nand/resource/kwyboardbackspace.dta"
#define pathKeyboardesc                     "nand/resource/kwyboardesc.dta"
#define pathKeyboardok                      "nand/resource/kwyboardok.dta"
#define pathKeyboardpoint                   "nand/resource/kwyboardpoint.dta"

#define pathKeyboard0press                 "nand/resource/kwyboard0press.dta"
#define pathKeyboard1press                 "nand/resource/kwyboard1press.dta"
#define pathKeyboard2press                 "nand/resource/kwyboard2press.dta"
#define pathKeyboard3press                 "nand/resource/kwyboard3press.dta"
#define pathKeyboard4press                 "nand/resource/kwyboard4press.dta"
#define pathKeyboard5press                 "nand/resource/kwyboard5press.dta"
#define pathKeyboard6press                 "nand/resource/kwyboard6press.dta"
#define pathKeyboard7press                 "nand/resource/kwyboard7press.dta"
#define pathKeyboard8press                 "nand/resource/kwyboard8press.dta"
#define pathKeyboard9press                 "nand/resource/kwyboard9press.dta"
#define pathKeyboardBackspacepress         "nand/resource/kwyboardbackspacepress.dta"
#define pathKeyboardescpress               "nand/resource/kwyboardescpress.dta"
#define pathKeyboardokpress                "nand/resource/kwyboardokpress.dta"
#define pathKeyboardpointpress             "nand/resource/kwyboardpointpress.dta"
//*************

//卡信息图片路径
#define pathcardinfostartup                         "nand/resource/cardinfostartup.dta"
#define pathcardinfostartone                        "nand/resource/cardinfostartone.dta"
#define pathcardinfostatrttwo                       "nand/resource/cardinfostarttwo.dta"
#define pathcardinfostatrtthree                     "nand/resource/cardinfostartthree.dta"
#define pathcardinfoback                            "nand/resource/cardinfoback.dta"
#define pathcardinfocardconditionnotok              "nand/resource/cardinfocardconditionnotok.dta"
#define pathcardinfochargingok                      "nand/resource/cardinfochargingok.dta"
#define pathcardinfonettimeout                      "nand/resource/cardinfonettimeout.dta"
#define pathcardinfopleaseplug                      "nand/resource/cardinfopleaseplug.dta"
#define pathcardinfopleasepluga                     "nand/resource/cardinfopleasepluga.dta"
#define pathcardinfopleaseplugb                     "nand/resource/cardinfopleaseplugb.dta"
#define pathcardinfoplugtimeout                     "nand/resource/cardinfoplugtimeout.dta"
#define pathcardinforeadystart                      "nand/resource/cardinforeadystart.dta"
#define pathcardinfoquit                            "nand/resource/cardinfoquit.dta"
#define pathcardinfoquitpress                       "nand/resource/cardinfoquitpress.dta"

#define pathcardinfoEquipmentFailureNoStart     "nand/resource/cardinfoEquipmentFailureNoStart.dta"
#define pathcardinfoPlugOk                 "nand/resource/cardinfoPlugOk.dta"

#define pathcardinfoPwdLimted              "nand/resource/cardinfoPwdLimted.dta"
#define pathcardinfoPwdError               "nand/resource/cardinfoPwdError.dta"
#define pathcardinfoPwdTest                "nand/resource/cardinfoPwdTest.dta"
//#define pathcardinfoarrears              "nand/resource/cardinfoarrears.dta"
//#define pathcardinfoUnavailable          "nand/resource/cardinfoUnavailable.dta"
//#define pathcardinfostartfail            "nand/resource/cardinfostartfail.dta"
//#define pathcardinfoback                 "nand/resource/cardinfoback.dta"
//#define pathcardinforeadycharging        "nand/resource/cardinforeadycharging.dta"
//#define pathcardinfoget                  "nand/resource/cardinfoget.dta"
//#define pathcardinfoplug                 "nand/resource/cardinfoplug.dta"
//#define pathcardinfounregister           "nand/resource/cardinfounregister.dta"
//******************
//密码页
#define pathPwdPromptPicture    "nand/resource/PwdPromptPicture.dta"
#define pathPwdInputBox         "nand/resource/PwdInputBox.dta"
//主页图片路径
#define pathhomebacksingle                           "/nand/resource/homebacksingle.dta"
#define pathhomegunchargedonesingle                  "/nand/resource/homegunchargedonesingle.dta"
#define pathhomegunchargingsingle                    "/nand/resource/homegunchargingsingle.dta"
#define pathhomegunerrorsingle                       "/nand/resource/homegunerrorsingle.dta"
#define pathhomegunfreesingle                        "/nand/resource/homegunfreesingle.dta"

#define pathhomeback                             "/nand/resource/homeback.dta"
#define pathhomegunAchargedone                   "/nand/resource/homegunAchargedone.dta"
#define pathhomegunAcharging                     "/nand/resource/homegunAcharging.dta"
#define pathhomegunAerror                        "/nand/resource/homegunAerror.dta"
#define pathhomegunAfree                         "/nand/resource/homegunAfree.dta"
#define pathhomegunBchargedone                   "/nand/resource/homegunBchargedone.dta"
#define pathhomegunBcharging                     "/nand/resource/homegunBcharging.dta"
#define pathhomegunBerror                        "/nand/resource/homegunBerror.dta"
#define pathhomegunBfree                         "/nand/resource/homegunBfree.dta"
#define pathhomegunlookinfo                      "/nand/resource/homegunlookinfo.dta"
#define pathhomegunscancode                      "/nand/resource/homegunscancode.dta"
#define pathhomegunlookinfopress                 "/nand/resource/homegunlookinfopress.dta"
#define pathhomegunscancodepress                 "/nand/resource/homegunscancodepress.dta"
#define pathhomegunerror                         "/nand/resource/homegunerror.dta"
#define pathhomesignal0                          "nand/resource/homesignal0.dta"
#define pathhomesignal1                          "nand/resource/homesignal1.dta"
#define pathhomesignal2                          "nand/resource/homesignal2.dta"
#define pathhomesignal3                          "nand/resource/homesignal3.dta"
#define pathhomesignal4                          "nand/resource/homesignal4.dta"
#define pathhomesignal5                          "nand/resource/homesignal5.dta"

#define pathhomesignalligined     "nand/resource/homesignalligined.dta"
#define pathhomesignalnotlogined  "nand/resource/homesignalnotlogined.dta"

#define pathhomechargedoneinfo          "/nand/resource/homechargedoneinfo.dta"
#define pathhomecharginginfo            "/nand/resource/homecharginginfo.dta"
#define pathhomechargehelp              "/nand/resource/homechargehelp.dta"
#define pathhomechargehelppress         "/nand/resource/homechargehelppress.dta"
#define pathhomehelpinfo                "/nand/resource/homehelpinfo.dta"


//提示信息页图片
#define pathchargedoneinfo      "nand/resource/chargedoneinfo.dta"
#define pathcharginginfo        "nand/resource/charginginfo.dta"



#define pathCheckboxDta              "/nand/resource/chosen.dta"
#define pathCheckboxNotDta           "/nand/resource/notchosen.dta"
#define pathCheckboxDisable          "/nand/resource/checkboxdisable.dta"
#define pathBeijingDta               "/nand/resource/beijing.dta"

#define pathstSIF12                  "/nand/resource/stSIF12.sif"
#define pathstSIF16                  "/nand/resource/stSIF16.sif"
#define pathstSIF24                  "/nand/resource/stSIF24.sif"
#define pathstSIF36                  "/nand/resource/stSIF36.sif"

#define pathfontwryhct12aa4          "/nand/resource/fontwryhct12aa4.sif"
#define pathfontwryhct16aa4          "/nand/resource/fontwryhct16aa4.sif"
#define pathfontwryhct24aa4          "/nand/resource/fontwryhct24aa4.sif"
#define pathfontwryhct36aa4          "/nand/resource/fontwryhct36aa4.sif"

#define pathfontwryhcg12aa4          "/nand/resource/fontwryhcg12aa4.sif"
#define pathfontwryhcg16aa4          "/nand/resource/fontwryhcg16aa4.sif"
#define pathfontwryhcg24aa4          "/nand/resource/fontwryhcg24aa4.sif"
#define pathfontwryhcg36aa4          "/nand/resource/fontwryhcg36aa4.sif"

#define pathfontwryhcg12aa4e         "/nand/resource/fontwryhcg12aa4e.sif"
#define pathfontwryhcg16aa4e         "/nand/resource/fontwryhcg16aa4e.sif"
#define pathfontwryhcg24aa4e         "/nand/resource/fontwryhcg24aa4e.sif"
#define pathfontwryhcg36aa4e         "/nand/resource/fontwryhcg36aa4e.sif"

#define pathfontwryhcg12e            "/nand/resource/fontwryhcg12e.sif"
#define pathfontwryhcg16e            "/nand/resource/fontwryhcg16e.sif"
#define pathfontwryhcg24e            "/nand/resource/fontwryhcg24e.sif"
#define pathfontwryhcg30e            "/nand/resource/fontwryhcg30e.sif"
#define pathfontwryhcg36e            "/nand/resource/fontwryhcg36e.sif"

#define pathfontwryhcg44e            "/nand/resource/fontwryhcg44e.sif"
#define pathfontwryhcg44aa4e         "/nand/resource/fontwryhcg44aa4e.sif"


#define YAFFS_MOUNT_POINT   "/nand/"
#define pathSystemDir       "/nand/system/"
#define pathUpgradeDir      "/nand/upgrade/"
#define pathDownloadDir     "/nand/download/"
#define pathResourceDir     "/nand/resource/"

#define pathDownloadList      "/nand/download/flist.json"
#define pathUpgradeTmp      "/nand/upgrade/upgrade.tmp"
#define pathBmpCheckTmp      "/nand/resource/bmpcheck.tmp"


#define jnEVSESN        "EVSESN"
#define jnEVSEID        "EVSEID"
#define jnEVSEType      "EVSEType"
#define jnPhaseLine     "PhaseLine"
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
#define jnSysChargersdk_bin          "chargesdk_bin"
#define jnSysChargersdk_bin_crc32    "chargesdk_bin_crc32"
#define jnSysEvse_cfg                "evse_cfg"
#define jnSysProtocol_cfg            "protocol_cfg"
#define jnSysSys_cfg                 "sys_cfg"
#define jnSysBlacklist_cfg           "blacklist_cfg"
#define jnSysWhitelist_cfg           "whitelist_cfg"
#define jnSysDispSleepTime           "DispSleepTime"
#define jnSystouchtype               "touchtype"
#define jnSysis_calibrate            "is_calibrate"
#define jnSysxfac                    "xfac"
#define jnSysyfac                    "yfac"
#define jnSysxoff                    "xoff"
#define jnSysyoff                    "yoff"
#define jnSysVersion                 "Version"
#define jnSysUSE_GPRSModem           "USE_GPRSModem"
#define jnSysUSE_Meter               "USE_Meter"
#define jnSysUSE_RFID                "USE_RFID"
#define jnSysUSE_GUI                 "USE_GUI"


/*protocol.cfg*/
#define jnProtoServerIP             "ServerIP"
#define jnProtoServerPort           "ServerPort"
#define jnProtoUserName             "UserName"
#define jnProtoUserPwd              "UserPwd"
#define jnProtoKey                  "Key"
#define jnProtoNewKey               "NewKey"
#define jnProtoNewKeyChangeTime     "NewKeyChangeTime"
#define jnProtoOptSN                "OptSN"
#define jnProtoProtoVer             "ProtoVer"
#define jnProtoHeartBeatCyc_ms      "HeartBeatCyc_ms"
#define jnProtoStatusCyc_ms         "StatusCyc_ms"
#define jnProtoRTDataCyc_ms         "RTDataCyc_ms"
#define jnProtoResetAct             "ResetAct"
#define jnProtoEnergyFee_sharp      "PowerFee_sharp"
#define jnProtoEnergyFee_peak       "PowerFee_peak"
#define jnProtoEnergyFee_shoulder   "PowerFee_shoulder"
#define jnProtoEnergyFee_off_peak   "PowerFee_off_peak"
#define jnProtoServFee_sharp        "ServiceFee_sharp"
#define jnProtoServFee_peak         "ServiceFee_peak"
#define jnProtoServFee_shoulder     "ServiceFee_shoulder"
#define jnProtoServFee_off_peak     "ServiceFee_off_peak"
#define jnProtoSegTime_sharp        "SegTime_sharp"
#define jnProtoSegTime_peak         "SegTime_peak"
#define jnProtoSegTime_shoulder     "SegTime_shoulder"
#define jnProtoSegTime_off_peak     "SegTime_off_peak"
#define jnProtoSegCont              "SegCont"

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

/*meter.cfg*/
#define jnMeter             "meter"
#define jnMeterEnergyAddr   "energy_addr"
#define jnMeterVoltAddr     "volt_addr"
#define jnMeterCurrAddr     "curr_addr"
#define jnMeterPwrAddr      "pwr_addr"
#define jnMeterFreqAddr     "freq_addr"

#endif
