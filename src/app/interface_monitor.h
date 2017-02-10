/**
* @file interface_monitor.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-02-09
*/
#ifndef  __INTERFACE_MONITOR_H
#define  __INTERFACE_MONITOR_H

double GetChargingVoltage(ChargePoint_t *pPoint);
double GetChargingCurrent(ChargePoint_t *pPoint);
double GetChargingFrequence(ChargePoint_t *pPoint);
uint32_t GetScramState(void);
uint32_t GetKnockState(void);
uint32_t GetPEState(void);
uint32_t GetPlugState(ChargePoint_t *pPoint);
uint32_t GetPowerOffState(void);
uint32_t GetArresterState(void);

#endif
