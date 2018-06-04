/**
* @file interface_diag.h
* @brief
* @author rgw
* @version v1.0
* @date 2017-03-13
*/
#ifndef  __INTERFACE_DIAG_H
#define  __INTERFACE_DIAG_H

void DiagVoltageError(CON_t *pCON);
void DiagCurrentError(CON_t *pCON);
void DiagCONTempError(CON_t *pCON);
void DiagFreqError(CON_t *pCON);
void DiagPlugError(CON_t *pCON);
void DiagLockError(CON_t *pCON);
void DiagEVSEError(CON_t *pCON);
void DiagEVSETempError(EVSE_t *pEVSE);

#endif
