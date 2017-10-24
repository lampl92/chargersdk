#ifndef __HMI_START_H
#define __HMI_START_H

extern GUI_HMEM    qr_hmem;

void MainTask(void);

void PutOut_Home();
void PutOut_Charging();
void PutOut_Card_Info();//(OrderData_t *order)
void PutOut_Charge_Done();
#endif
