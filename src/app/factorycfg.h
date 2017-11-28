/**
* @file factorycfg.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
#ifndef  __FACTORYCFG_H
#define  __FACTORYCFG_H

const uint8_t *strEVSECfg = 
"{\
    \"EVSESN\":\"0102030405060708\",\
    \"EVSEID\":\"3000000000000002\",\
    \"EVSEType\":2,\
    \"TotalCON\":1,\
    \"Lng\":116.275833,\
    \"Lat\":39.831944,\
    \"ServiceFeeType\":1,\
    \"ServiceFee\":1,\
    \"DefSegFee\":1.2,\
    \"CONArray\":[\
        {\
            \"ID\":0,\
            \"Type\":2,\
            \"SocketType\":67,\
            \"VolatageUpperLimits\":260,\
            \"VolatageLowerLimits\":176,\
            \"ACTempUpperLimits\":95,\
            \"ACTempLowerLimits\":-40,\
            \"SocketTempUpperLimits\":95,\
            \"SocketTempLowerLimits\":-40,\
            \"RatedCurrent\":32,\
            \"RatedPower\":7,\
            \"QRCode\":\"3000000000000002\"\
        }\
    ],\
    \"TemplSegArray\":[]\
}";

const uint8_t *strProtoCfg =
"{\
    \"ServerIP\": \"123.56.113.123\",\
    \"ServerPort\": 6677,\
    \"UserName\": \"esaasusr\",\
    \"UserPwd\": \"esaaspasswrd\",\
    \"Key\": \"0123456789abcdeg\",\
    \"NewKey\": \"\",\
    \"NewKeyChangeTime\": 4294967295,\
    \"OptSN\": 0,\
    \"ProtoVer\": 104,\
    \"HeartBeatCyc_ms\": 15000,\
    \"StatusCyc_ms\": 120000,\
    \"RTDataCyc_ms\": 10000,\
    \"ResetAct\": 0,\
    \"PowerFee_sharp\": 1,\
    \"PowerFee_peak\": 0,\
    \"PowerFee_shoulder\": 0,\
    \"PowerFee_off_peak\": 0,\
    \"ServiceFee_sharp\": 0.8,\
    \"ServiceFee_peak\": 0,\
    \"ServiceFee_shoulder\": 0,\
    \"ServiceFee_off_peak\": 0,\
    \"SegTime_sharp\": {\
        \"SegCont\": 1,\
        \"Start1\": 0,\
        \"End1\": 24,\
        \"Start2\": 255,\
        \"End2\": 255,\
        \"Start3\": 255,\
        \"End3\": 255,\
        \"Start4\": 255,\
        \"End4\": 255,\
        \"Start5\": 255,\
        \"End5\": 255\
    },\
    \"SegTime_peak\": {\
        \"SegCont\": 0,\
        \"Start1\": 255,\
        \"End1\": 255,\
        \"Start2\": 255,\
        \"End2\": 255,\
        \"Start3\": 255,\
        \"End3\": 255,\
        \"Start4\": 255,\
        \"End4\": 255,\
        \"Start5\": 255,\
        \"End5\": 255\
    },\
    \"SegTime_shoulder\": {\
        \"SegCont\": 0,\
        \"Start1\": 255,\
        \"End1\": 255,\
        \"Start2\": 255,\
        \"End2\": 255,\
        \"Start3\": 255,\
        \"End3\": 255,\
        \"Start4\": 255,\
        \"End4\": 255,\
        \"Start5\": 255,\
        \"End5\": 255\
    },\
    \"SegTime_off_peak\": {\
        \"SegCont\": 0,\
        \"Start1\": 255,\
        \"End1\": 255,\
        \"Start2\": 255,\
        \"End2\": 255,\
        \"Start3\": 255,\
        \"End3\": 255,\
        \"Start4\": 255,\
        \"End4\": 255,\
        \"Start5\": 255,\
        \"End5\": 255\
    }\
}";

const uint8_t *strFtpCfg =
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

const uint8_t *strSysCfg =
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
    \"Version\":\"0.2.1.2552\",\n \
    \"USE_GPRSModem\":2,\n \
    \"USE_Meter\": 1,\n \
    \"USE_RFID\": 1,\n \
    \"USE_GUI\":1\n \
}\n";
   
        
const uint8_t *strWhiteListCfg ="[]";
const uint8_t *strBlackListCfg ="[]";
    
const uint8_t *strOrderCfg ="[]";
const uint8_t *strLogCfg ="[]";

#endif
