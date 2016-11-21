/**
* @file bsp_ltdc.h
* @brief 
* @author rgw
* @version v1.0
* @date 2016-11-10
*/
#ifndef BSP_LTDC_H
#define BSP_LTDC_H
#include "includes.h"

#define LCD_PIXEL_FORMAT_ARGB8888       0x00
#define LCD_PIXEL_FORMAT_RGB888         0X01    
#define LCD_PIXEL_FORMAT_RGB565         0X02       
#define LCD_PIXEL_FORMAT_ARGB1555       0X03      
#define LCD_PIXEL_FORMAT_ARGB4444       0X04     
#define LCD_PIXEL_FORMAT_L8             0X05     
#define LCD_PIXEL_FORMAT_AL44           0X06     
#define LCD_PIXEL_FORMAT_AL88           0X07     


//LCD LTDC��Ҫ������
typedef struct  
{
	uint32_t pwidth;			//LCD���Ŀ��,�̶�����,������ʾ����ı�,���Ϊ0,˵��û���κ�RGB������
	uint32_t pheight;		//LCD���ĸ߶�,�̶�����,������ʾ����ı�
	uint16_t hsw;			//ˮƽͬ�����
	uint16_t vsw;			//��ֱͬ�����
	uint16_t hbp;			//ˮƽ����
	uint16_t vbp;			//��ֱ����
	uint16_t hfp;			//ˮƽǰ��
	uint16_t vfp;			//��ֱǰ�� 
	uint8_t  activelayer;		//��ǰ����:0/1	
	uint8_t  dir;				//0,����;1,����;
	uint16_t width;			//LCD���
	uint16_t height;			//LCD�߶�
	uint32_t pixsize;		//ÿ��������ռ�ֽ���
}_ltdc_dev; 

void bsp_LTDC_Init(void);
void LTDC_Draw_Point(uint16_t x,uint16_t y,uint32_t color);
uint32_t LTDC_Read_Point(uint16_t x,uint16_t y);
void LTDC_Fill(uint16_t sx,uint16_t sy,uint16_t ex,uint16_t ey,uint32_t color);



#endif

