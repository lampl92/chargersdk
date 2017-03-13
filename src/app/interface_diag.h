/**
* @file interface_diag.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#ifndef  __INTERFACE_DIAG_H
#define  __INTERFACE_DIAG_H

void DiagVoltageError(ChargePoint_t *pPoint);
void DiagCurrentError(ChargePoint_t *pPoint);
void DiagTempError(ChargePoint_t *pPoint);
void DiagFreqError(ChargePoint_t *pPoint);
void DiagPlugError(ChargePoint_t *pPoint);
void DiagLockError(ChargePoint_t *pPoint);
void DiagEVSEError(ChargePoint_t *pPoint);

#endif
