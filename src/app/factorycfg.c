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
    \"EVSEID\":\"2000000000000002\",\n \
    \"EVSEType\":2,\n \
    \"TotalCON\":2,\n \
    \"Lng\":116.275833,\n \
    \"Lat\":39.831944,\n \
    \"ServiceFeeType\":1,\n \
    \"ServiceFee\":1,\n \
    \"DefSegFee\":1.2,\n \
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
            \"QRCode\":\"2000000000000002\"\n \
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
            \"QRCode\":\"2000000000000013\"\n \
        }\n \
    ],\n \
    \"TemplSegArray\":[]\n \
}\n";

const char *strProtoCfg =
"{\
    \"ServerIP\": \"123.56.113.123\",\n \
    \"ServerPort\": 6677,\n \
    \"UserName\": \"esaasusr\",\n \
    \"UserPwd\": \"esaaspasswrd\",\n \
    \"Key\": \"0123456789abcdeg\",\n \
    \"NewKey\": \"\",\n \
    \"NewKeyChangeTime\": 4294967295,\n \
    \"OptSN\": 0,\n \
    \"ProtoVer\": 104,\n \
    \"HeartBeatCyc_ms\": 15000,\n \
    \"StatusCyc_ms\": 120000,\n \
    \"RTDataCyc_ms\": 10000,\n \
    \"ResetAct\": 0,\n \
    \"PowerFee_sharp\": 1,\n \
    \"PowerFee_peak\": 0,\n \
    \"PowerFee_shoulder\": 0,\n \
    \"PowerFee_off_peak\": 0,\n \
    \"ServiceFee_sharp\": 0.8,\n \
    \"ServiceFee_peak\": 0,\n \
    \"ServiceFee_shoulder\": 0,\n \
    \"ServiceFee_off_peak\": 0,\n \
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
    \"xfac\": -0.21276596188545227,\n \
    \"yfac\": 0.1423487514257431,\n \
    \"xoff\": 830,\n \
    \"yoff\": 65486,\n \
    \"Version\":\"01.01.1222\",\n \
    \"USE_GPRSModem\":2,\n \
    \"USE_Meter\": 1,\n \
    \"USE_RFID\": 1,\n \
    \"USE_GUI\":1\n \
}\n";
    
    
//Adapter   1:以太网  2: 2G  3: 3G  4: 4G 5: WIFI
//DHCP   1:使能  0:禁止
const char *strNetCfg =
"{\n \
    \"Adapter\": 1,\n \
    \"DHCP\": 1,\n \
    \"HostName\":\"RGW1 Charger\",\n \
    \"MAC\":\"00-AB-CD-EF-04-29\",\n \
    \"IP\":\"192.168.153.105\",\n \
    \"Mask\":\"192.168.30.1\",\n \
    \"Gate\":\"255.255.255.0\",\n \
    \"DNS1\":\"192.168.30.1\",\n \
    \"DNS2\":\"0.0.0.0\"\n \
}\n";
        
const char *strWhiteListCfg ="[]\n";
const char *strBlackListCfg ="[]\n";
    
const char *strOrderCfg ="[]\n";
const char *strLogCfg ="[]\n";
