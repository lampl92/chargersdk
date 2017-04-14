#ifndef __BSP_TOUCH_2046_H
#define __BSP_TOUCH_2046_H

#include "bsp.h"

//������оƬ��������
#define PEN     PDin(3)    //T_PEN
#define DOUT    PGin(12)    //T_MISO
#define TDIN    PGout(14)   //T_MOSI
#define TCLK    PGout(13)   //T_SCK
#define TCS     PFout(9)   //T_CS

//����������
void TP_Write_Byte(uint8_t num);						//�����оƬд��һ������
uint16_t TP_Read_AD(uint8_t CMD);							//��ȡADת��ֵ
uint16_t TP_Read_XOY(uint8_t xy);							//���˲��������ȡ(X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);					//˫�����ȡ(X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);					//����ǿ�˲���˫���������ȡ
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);//��һ������У׼��
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	//��һ�����
void TP_Save_Adjdata(void);						//����У׼����
uint8_t TP_Get_Adjdata(void);						//��ȡУ׼����
void TP_Adjust(void);							//������У׼
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//��ʾУ׼��Ϣ
//������/������ ���ú���
void TP_Scan(uint16_t *adc_x, uint16_t *adc_y);								//ɨ��
uint8_t bsp_Touch_Init(void);								//��ʼ��

#endif

