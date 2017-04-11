#ifndef __BSP_TOUCH_2046_H
#define __BSP_TOUCH_2046_H

#include "bsp.h"

//电阻屏芯片连接引脚
#define PEN     PDin(3)    //T_PEN
#define DOUT    PGin(12)    //T_MISO
#define TDIN    PGout(14)   //T_MOSI
#define TCLK    PGout(13)   //T_SCK
#define TCS     PFout(9)   //T_CS

//电阻屏函数
void TP_Write_Byte(uint8_t num);						//向控制芯片写入一个数据
uint16_t TP_Read_AD(uint8_t CMD);							//读取AD转换值
uint16_t TP_Read_XOY(uint8_t xy);							//带滤波的坐标读取(X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);					//双方向读取(X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);					//带加强滤波的双方向坐标读取
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);//画一个坐标校准点
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	//画一个大点
void TP_Save_Adjdata(void);						//保存校准参数
uint8_t TP_Get_Adjdata(void);						//读取校准参数
void TP_Adjust(void);							//触摸屏校准
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//显示校准信息
//电阻屏/电容屏 共用函数
void TP_Scan(uint16_t *adc_x, uint16_t *adc_y);								//扫描
uint8_t bsp_Touch_Init(void);								//初始化

#endif

