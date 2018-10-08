/**
* @file factorycfg.c
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
#include "stm32f4xx.h"

const char *strEVSECfg = 
"{\
    \"EVSESN\":\"0102030405060708\",\n \
    \"EVSEID\":\"1234567890012001\",\n \
    \"EVSEType\":2,\n \
    \"PhaseLine\":1,\n \
    \"TotalCON\":1,\n \
    \"Lng\":116.275833,\n \
    \"Lat\":39.831944,\n \
    \"ServiceFeeType\":1,\n \
    \"ServiceFee\":0.8,\n \
    \"DefSegFee\":1.0,\n \
    \"CONArray\":[\n \
        {\n \
            \"ID\":0,\n \
            \"Type\":2,\n \
            \"SocketType\":67,\n \
            \"VolatageUpperLimits\":260,\n \
            \"VolatageLowerLimits\":176,\n \
            \"ACTempUpperLimits\":95,\n \
            \"ACTempLowerLimits\":-40,\n \
            \"SocketTempUpperLimits\":95,\n \
            \"SocketTempLowerLimits\":-40,\n \
            \"RatedCurrent\":32,\n \
            \"RatedPower\":7,\n \
            \"QRCode\":\"12345678900120010\"\n \
        },\n \
        {\n \
            \"ID\":1,\n \
            \"Type\":2,\n \
            \"SocketType\":67,\n \
            \"VolatageUpperLimits\":260,\n \
            \"VolatageLowerLimits\":176,\n \
            \"ACTempUpperLimits\":95,\n \
            \"ACTempLowerLimits\":-40,\n \
            \"SocketTempUpperLimits\":95,\n \
            \"SocketTempLowerLimits\":-40,\n \
            \"RatedCurrent\":32,\n \
            \"RatedPower\":7,\n \
            \"QRCode\":\"12345678900120012\"\n \
        }\n \
    ],\n \
    \"TemplSegArray\":[]\n \
}\n";

const char *strProtoCfg =
"{\
    \"ServerIP\": \"s.dpcpower.com\",\n \
    \"ServerPort\": 6999,\n \
    \"UserName\": \"dpcpower\",\n \
    \"UserPwd\": \"dpcpowerpass\",\n \
    \"Key\": \"0123456789abcdeg\",\n \
    \"NewKey\": \"\",\n \
    \"NewKeyChangeTime\": 2147483647,\n \
    \"OptSN\": 0,\n \
    \"ProtoVer\": 104,\n \
    \"HeartBeatCyc_ms\": 15000,\n \
    \"StatusCyc_ms\": 120000,\n \
    \"RTDataCyc_ms\": 10000,\n \
    \"ResetAct\": 0,\n \
    \"PowerFee_sharp\": 1,\n \
    \"PowerFee_peak\": 1,\n \
    \"PowerFee_shoulder\": 1,\n \
    \"PowerFee_off_peak\": 1,\n \
    \"ServiceFee_sharp\": 0.8,\n \
    \"ServiceFee_peak\": 0.8,\n \
    \"ServiceFee_shoulder\": 0.8,\n \
    \"ServiceFee_off_peak\": 0.8,\n \
    \"SegTime_sharp\": {\n \
        \"SegCont\": 2,\n \
        \"Start1\": 11,\n \
        \"End1\": 13,\n \
        \"Start2\": 16,\n \
        \"End2\": 17,\n \
        \"Start3\": 255,\n \
        \"End3\": 255,\n \
        \"Start4\": 255,\n \
        \"End4\": 255,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    },\n \
    \"SegTime_peak\": {\n \
        \"SegCont\": 3,\n \
        \"Start1\": 10,\n \
        \"End1\": 11,\n \
        \"Start2\": 13,\n \
        \"End2\": 15,\n \
        \"Start3\": 18,\n \
        \"End3\": 21,\n \
        \"Start4\": 255,\n \
        \"End4\": 255,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    },\n \
    \"SegTime_shoulder\": {\n \
        \"SegCont\": 4,\n \
        \"Start1\": 7,\n \
        \"End1\": 10,\n \
        \"Start2\": 15,\n \
        \"End2\": 16,\n \
        \"Start3\": 17,\n \
        \"End3\": 18,\n \
        \"Start4\": 21,\n \
        \"End4\": 23,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    },\n \
    \"SegTime_off_peak\": {\n \
        \"SegCont\": 2,\n \
        \"Start1\": 23,\n \
        \"End1\": 24,\n \
        \"Start2\": 0,\n \
        \"End2\": 7,\n \
        \"Start3\": 255,\n \
        \"End3\": 255,\n \
        \"Start4\": 255,\n \
        \"End4\": 255,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    }\n \
}\n";

const char *strFtpCfg =
"{\n \
    \"Server\": \"123.56.106.11\",\n \
    \"Port\": 21,\n \
    \"Username\" : \"username\",\n \
    \"Password\" : \"password\",\n \
    \"NewVersion\" : \"1234567890\",\n \
    \"NewFilename\" : \"1234567890\",\n \
    \"DownloadStart\" : 0,\n \
    \"DownloadStatus\": 123\n \
}\n";
//	"xfac":	-0.21240916848182678,
//	"yfac":	-0.13341419398784637,
//	"xoff":	827,
//	"yoff":	512,

const char *strSysCfg =
"{\n \
    \"chargesdk_bin\" : 0,\n \
    \"chargesdk_bin_crc32\" : 1901693291,\n \
    \"evse_cfg\" : 0,\n \
    \"protocol_cfg\" : 0,\n \
    \"sys_cfg\" : 0,\n \
    \"blacklist_cfg\" : 0,\n \
    \"whitelist_cfg\" : 0,\n \
    \"DispSleepTime\" : 60,\n \
    \"touchtype\": 0,\n \
    \"is_calibrate\": 170,\n \
    \"xfac\": -0.21240916848182678,\n \
    \"yfac\": -0.13341419398784637,\n \
    \"xoff\": 827,\n \
    \"yoff\": 512,\n \
    \"Version\":\"888888\",\n \
    \"USE_GPRSModem\":2,\n \
    \"USE_Meter\": 2,\n \
    \"USE_RFID\": 1,\n \
    \"USE_GUI\":1\n \
}\n";
    
    
//Adapter   1:以太网  2: 2G  3: 3G  4: 4G 5: WIFI
//DHCP   1:使能  0:禁止
const char *strNetCfg =
"{\n \
    \"Adapter\": 2,\n \
    \"DHCP\": 1,\n \
    \"HostName\":\"DUET Charger\",\n \
    \"MAC\":\"00-AB-CD-EF-04-29\",\n \
    \"IP\":\"192.168.1.100\",\n \
    \"Mask\":\"255.255.255.0\",\n \
    \"Gate\":\"192.168.1.1\",\n \
    \"DNS1\":\"192.168.1.1\",\n \
    \"DNS2\":\"0.0.0.0\"\n \
}\n";
     
const char *strMeterCfg =
"{\n \
    \"meter2\":\n \
    {\n \
        \"energy_addr\": 0,\n \
        \"volt_addr\": 11,\n \
        \"curr_addr\": 12,\n \
        \"pwr_addr\": 13,\n \
        \"freq_addr\": 17\n \
    },\n \
    \"meter3\":\n \
    {\n \
        \"energy_addr\": 0,\n \
        \"volt_addr\": 97,\n \
        \"curr_addr\": 100,\n \
        \"pwr_addr\": 362,\n \
        \"freq_addr\": 119\n \
    },\n \
    \"meter4\":\n \
    {\n \
        \"energy_addr\": 00,\n \
        \"volt_addr\": 66,\n \
        \"curr_addr\": 69,\n \
        \"pwr_addr\": -1,\n \
        \"freq_addr\": -1\n \
    }\n \
}\n";
const char *strWhiteListCfg ="[]\n";
const char *strBlackListCfg ="[]\n";
    
const char *strOrderCfg ="[]\n";
const char *strLogCfg ="[]\n";
