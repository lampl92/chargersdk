#ifndef ST_LIS2DH12_H_INCLUDED
#define ST_LIS2DH12_H_INCLUDED
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#define radian  57.29578
float get_angle_max(void);
void Lis2dh12_init(void);
int16_t yy_test_x,yy_test_y,yy_test_z,yy_test_x_l,yy_test_x_h;//acceleration
float x_acceleration,y_acceleration,z_acceleration;
uint8_t yy_test_relay;
double angle_x,angle_y,angle_z;
#endif /* ST_LIS2DH12_H_INCLUDED */
