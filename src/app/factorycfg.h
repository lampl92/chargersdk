/**
* @file factorycfg.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-04-18
*/
#ifndef  __FACTORYCFG_H
#define  __FACTORYCFG_H

const uint8_t *strEVSECfg = "{\"EVSESN\":\"0102030405060708\",\"EVSEID\":\"0102030405060708\",\"EVSEType\":2,\"TotalCON\":2,\"Lng\":116.275833,\"Lat\":39.831944,\"ServiceFeeType\":1,\"ServiceFee\":1,\"DefSegFee\":1.2,\"CONArray\":[{\"ID\":0,\"Type\":2,\"SocketType\":\"C\",\"VolatageUpperLimits\":250,\"VolatageLowerLimits\":190,\"ACTempUpperLimits\":105,\"ACTempLowerLimits\":-40,\"SocketTempUpperLimits\":105,\"SocketTempLowerLimits\":-40,\"RatedCurrent\":32,\"RatedPower\":7}],\"TemplSegArray\":[]}";

/*
{
    "EVSESN": "0102030405060708",
    "EVSEID": "0102030405060708",
    "EVSEType": 2,
    "TotalCON": 1,
    "Lng": 116.275833,
    "Lat": 39.831944,
    "ServiceFeeType": 1,
    "ServiceFee": 1,
    "DefSegFee": 1.2,
    "CONArray": [
        {
            "ID": 0,
            "Type": 2,
            "SocketType": "C",
            "VolatageUpperLimits": 250,
            "VolatageLowerLimits": 190,
            "ACTempUpperLimits": 105,
            "ACTempLowerLimits": -40,
            "SocketTempUpperLimits": 105,
            "SocketTempLowerLimits": -40,
            "RatedCurrent": 32,
            "RatedPower": 7
        }
    ],
    "TemplSegArray": []
}
*/

#endif
