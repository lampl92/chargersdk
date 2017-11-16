#include "sys_types.h"
#include "bsp_define.h"
#include "bsp.h"
#include "ST_LIS2DH12.h"
#include "myiic.h"
#include "user_app.h"
#include "touch.h"
#include <utils.h>
#include <math.h>
uint16_t Read_Lis2ds12(uint8_t CMD)
{
    u8 count = 0;
    u8 Num = 0;
    TCLK = 0;   //?????
    TDIN = 0;   //?????
    Chip2.cs_zl = 0;    //?????????
    write_pca9554_2();
    TP_Write_Byte(CMD+0X80);//?????
    for(count = 0; count < 8; count++) //??8???
    {
        Num <<= 1;
        TCLK = 0; //?????
        bsp_DelayUS(10);
        TCLK = 1;
        bsp_DelayUS(10);
        if(DOUT)
        {
            Num |= 1;
        }
    }
     Chip2.cs_zl = 1;    //????
    write_pca9554_2();
    return(Num);
}

void Write_Lis2ds12(u8 add,uint8_t date)
{
    TCLK = 0;   //?????
    TDIN = 0;   //?????
    Chip2.cs_zl = 0;    //?????????
    write_pca9554_2();
    TP_Write_Byte(add+0X00);//?????
    TP_Write_Byte(date);

     Chip2.cs_zl = 1;    //????
    write_pca9554_2();
}
void GET_ANGLE(void)
{
		yy_test_x=Read_Lis2ds12(0x28)|Read_Lis2ds12(0x29)<<8;
		yy_test_y=Read_Lis2ds12(0x2a)|Read_Lis2ds12(0x2b)<<8;
		yy_test_z=Read_Lis2ds12(0x2c)|Read_Lis2ds12(0x2d)<<8;
		x_acceleration=yy_test_x*0.001;
		y_acceleration=yy_test_y*0.001;
		z_acceleration=yy_test_z*0.001;
		angle_x=radian*(atan(x_acceleration/sqrt((y_acceleration*y_acceleration)+(z_acceleration*z_acceleration))));
		angle_y=radian*(atan(y_acceleration/sqrt((x_acceleration*x_acceleration)+(z_acceleration*z_acceleration))));
		angle_z=radian*(atan(z_acceleration/sqrt((x_acceleration*x_acceleration)+(y_acceleration*y_acceleration))));
}
float get_angle_max(void)
{
    float angle_max;
    GET_ANGLE();
    if(utils_abs(angle_x)>utils_abs(angle_y))
    {
        angle_max=utils_abs(angle_x);
    }
    else
    {
         angle_max=utils_abs(angle_y);
    }
    if(angle_max<utils_abs(angle_z))
    {
        angle_max=utils_abs(angle_z);
    }
return angle_max;
}
void Lis2dh12_init(void)
{
    Write_Lis2ds12(0X20,0X77);
    Write_Lis2ds12(0X27,0XFF);
	Write_Lis2ds12(0X23,0X38);
}
