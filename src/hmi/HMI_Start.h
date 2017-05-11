#ifndef __HMI_START_H
#define __HMI_START_H

void MainTask(void);

void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
