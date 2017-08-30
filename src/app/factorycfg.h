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
"{\r\n\
    \"EVSESN\":\"0102030405060708\",\r\n\
    \"EVSEID\":\"3000000000000004\",\r\n\
    \"EVSEType\":2,\r\n\
    \"TotalCON\":1,\r\n\
    \"Lng\":116.275833,\r\n\
    \"Lat\":39.831944,\r\n\
    \"ServiceFeeType\":1,\r\n\
    \"ServiceFee\":1,\r\n\
    \"DefSegFee\":1.2,\r\n\
    \"CONArray\":[\r\n\
        {\r\n\
            \"ID\":0,\r\n\
            \"Type\":2,\r\n\
            \"SocketType\":67,\r\n\
            \"VolatageUpperLimits\":250,\r\n\
            \"VolatageLowerLimits\":190,\r\n\
            \"ACTempUpperLimits\":105,\r\n\
            \"ACTempLowerLimits\":-40,\r\n\
            \"SocketTempUpperLimits\":105,\r\n\
            \"SocketTempLowerLimits\":-40,\r\n\
            \"RatedCurrent\":32,\r\n\
            \"RatedPower\":7,\r\n\
            \"QRCode\":\"3000000000000004\"\r\n\
        }\r\n\
    ],\r\n\
    \"TemplSegArray\":[]\r\n\
}";

const uint8_t *strProtoCfg =
"{ \r\n\
    \"ServerIP\": \"123.56.113.123\", \r\n\
    \"ServerPort\": 6677, \r\n\
    \"UserName\": \"esaasusr\", \r\n\
    \"UserPwd\": \"esaaspasswrd\", \r\n\
    \"Key\": \"0123456789abcdeg\", \r\n\
    \"NewKey\": \"\", \r\n\
    \"NewKeyChangeTime\": 4294967295, \r\n\
    \"OptSN\": 0, \r\n\
    \"ProtoVer\": 104, \r\n\
    \"HeartBeatCyc_ms\": 15000, \r\n\
    \"StatusCyc_ms\": 20000, \r\n\
    \"RTDataCyc_ms\": 10000, \r\n\
    \"ResetAct\": 0, \r\n\
    \"PowerFee_sharp\": 0, \r\n\
    \"PowerFee_peak\": 0, \r\n\
    \"PowerFee_shoulder\": 0, \r\n\
    \"PowerFee_off_peak\": 0, \r\n\
    \"ServiceFee_sharp\": 0, \r\n\
    \"ServiceFee_peak\": 0, \r\n\
    \"ServiceFee_shoulder\": 0, \r\n\
    \"ServiceFee_off_peak\": 0, \r\n\
    \"SegTime_sharp\": { \r\n\
        \"SegCont\": 0, \r\n\
        \"Start1\": 255, \r\n\
        \"End1\": 255, \r\n\
        \"Start2\": 255, \r\n\
        \"End2\": 255, \r\n\
        \"Start3\": 255, \r\n\
        \"End3\": 255, \r\n\
        \"Start4\": 255, \r\n\
        \"End4\": 255, \r\n\
        \"Start5\": 255, \r\n\
        \"End5\": 255 \r\n\
    }, \r\n\
    \"SegTime_peak\": { \r\n\
        \"SegCont\": 0, \r\n\
        \"Start1\": 255, \r\n\
        \"End1\": 255, \r\n\
        \"Start2\": 255, \r\n\
        \"End2\": 255, \r\n\
        \"Start3\": 255, \r\n\
        \"End3\": 255, \r\n\
        \"Start4\": 255, \r\n\
        \"End4\": 255, \r\n\
        \"Start5\": 255, \r\n\
        \"End5\": 255 \r\n\
    }, \r\n\
    \"SegTime_shoulder\": { \r\n\
        \"SegCont\": 0, \r\n\
        \"Start1\": 255, \r\n\
        \"End1\": 255, \r\n\
        \"Start2\": 255, \r\n\
        \"End2\": 255, \r\n\
        \"Start3\": 255, \r\n\
        \"End3\": 255, \r\n\
        \"Start4\": 255, \r\n\
        \"End4\": 255, \r\n\
        \"Start5\": 255, \r\n\
        \"End5\": 255 \r\n\
    }, \r\n\
    \"SegTime_off_peak\": { \r\n\
        \"SegCont\": 0, \r\n\
        \"Start1\": 255, \r\n\
        \"End1\": 255, \r\n\
        \"Start2\": 255, \r\n\
        \"End2\": 255, \r\n\
        \"Start3\": 255, \r\n\
        \"End3\": 255, \r\n\
        \"Start4\": 255, \r\n\
        \"End4\": 255, \r\n\
        \"Start5\": 255, \r\n\
        \"End5\": 255 \r\n\
    } \r\n\
}";

const uint8_t *strWhiteListCfg ="[]";
const uint8_t *strBlackListCfg ="[]";

#endif
