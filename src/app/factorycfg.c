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
    \"EVSEID\":\"3000000000000002\",\n \
    \"EVSEType\":2,\n \
    \"TotalCON\":1,\n \
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
            \"QRCode\":\"3000000000000002\"\n \
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
        \"SegCont\": 1,\n \
        \"Start1\": 0,\n \
        \"End1\": 24,\n \
        \"Start2\": 255,\n \
        \"End2\": 255,\n \
        \"Start3\": 255,\n \
        \"End3\": 255,\n \
        \"Start4\": 255,\n \
        \"End4\": 255,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    },\n \
    \"SegTime_peak\": {\n \
        \"SegCont\": 0,\n \
        \"Start1\": 255,\n \
        \"End1\": 255,\n \
        \"Start2\": 255,\n \
        \"End2\": 255,\n \
        \"Start3\": 255,\n \
        \"End3\": 255,\n \
        \"Start4\": 255,\n \
        \"End4\": 255,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    },\n \
    \"SegTime_shoulder\": {\n \
        \"SegCont\": 0,\n \
        \"Start1\": 255,\n \
        \"End1\": 255,\n \
        \"Start2\": 255,\n \
        \"End2\": 255,\n \
        \"Start3\": 255,\n \
        \"End3\": 255,\n \
        \"Start4\": 255,\n \
        \"End4\": 255,\n \
        \"Start5\": 255,\n \
        \"End5\": 255\n \
    },\n \
    \"SegTime_off_peak\": {\n \
        \"SegCont\": 0,\n \
        \"Start1\": 255,\n \
        \"End1\": 255,\n \
        \"Start2\": 255,\n \
        \"End2\": 255,\n \
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
   
        
const char *strWhiteListCfg ="[]\n";
const char *strBlackListCfg ="[]\n";
    
const char *strOrderCfg ="[]\n";
const char *strLogCfg ="[]\n";
