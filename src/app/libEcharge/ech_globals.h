/**
* @file ech_globals.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-01-23
*/
#ifndef __ECH_GLOBALS_H
#define __ECH_GLOBALS_H

#define ECH_PROTO_VER			0x68
/* 基础操作 */
#define defEchCmdLoginReq        1 //C->S
#define defEchCmdLoginRes        2 //S->C
#define defEchCmdHeartReq        3 //C->S
#define defEchCmdHeartRes        4 //S->C
#define defEchCmdRestartReq      5 //S->C
#define defEchCmdRestartRes      6 //C->S
#define defEchCmdSetValidRes	 7 //C->S
#define defEchCmdSedInvalidRes	 8 //C->S
/* 有卡充电 */
#define defEchCmdRfidStartReq	 90 //C->S
#define defEchCmdRfidStartRes	 91 //S->C
#define defEchCmdRfidStartRetReq 92 //C->S
#define defEchCmdRfidStartRetRes 93 //S->C
#define defEchCmdRfidRTDataReq	 94 //C->S
#define defEchCmdRfidStopReq     95 //C->S
#define defEchCmdRfidStopRes	 96 //S->C

#endif /* __PS_GLOBALS_H */
