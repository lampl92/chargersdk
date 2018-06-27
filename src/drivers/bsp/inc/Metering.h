#ifndef _Metering_H
#define _Metering_H
#include "ade7953.h"

#include "ADE79xx_Data.h"

extern unsigned long ActiveEnergy;
extern unsigned long ReactiveEnergy;
extern unsigned long ActivePowerA;
extern unsigned long ActivePowerB;
extern unsigned long ReactivePowerA;
extern unsigned long ReactivePowerB;
extern unsigned long IrmsA;
extern unsigned long IrmsB;
extern unsigned long Vrms;
extern unsigned short int FrequencyA;
extern unsigned short int PowerFactorA;
extern unsigned short int PowerFactorB;
void ConfigAde7953(void);
void ReadAde7953DataRegister(void);
void setgain(void);

#endif
