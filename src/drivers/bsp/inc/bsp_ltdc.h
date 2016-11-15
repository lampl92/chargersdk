/**
* @file bsp_ltdc.h
* @brief 
* @author rgw
* @version v1.0
* @date 2016-11-10
*/
#ifndef BSP_LTDC_H
#define BSP_LTDC_H


#define LCD_PIXEL_FORMAT_ARGB8888       0x00
#define LCD_PIXEL_FORMAT_RGB888         0X01    
#define LCD_PIXEL_FORMAT_RGB565         0X02       
#define LCD_PIXEL_FORMAT_ARGB1555       0X03      
#define LCD_PIXEL_FORMAT_ARGB4444       0X04     
#define LCD_PIXEL_FORMAT_L8             0X05     
#define LCD_PIXEL_FORMAT_AL44           0X06     
#define LCD_PIXEL_FORMAT_AL88           0X07     


//LCD LTDC重要参数集
typedef struct  
{
	uint32_t pwidth;			//LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
	uint32_t pheight;		//LCD面板的高度,固定参数,不随显示方向改变
	uint16_t hsw;			//水平同步宽度
	uint16_t vsw;			//垂直同步宽度
	uint16_t hbp;			//水平后廊
	uint16_t vbp;			//垂直后廊
	uint16_t hfp;			//水平前廊
	uint16_t vfp;			//垂直前廊 
	uint8_t  activelayer;		//当前层编号:0/1	
	uint8_t  dir;				//0,竖屏;1,横屏;
	uint16_t width;			//LCD宽度
	uint16_t height;			//LCD高度
	uint32_t pixsize;		//每个像素所占字节数
}_ltdc_dev; 

void bsp_LTDC_Init(void);
void LTDC_Draw_Point(u16 x,u16 y,u32 color);
u32 LTDC_Read_Point(u16 x,u16 y);
void LTDC_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u32 color);



#endif

