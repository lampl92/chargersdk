/**
* @file bsp_ltdc.h
* @brief 
* @author rgw
* @version v1.0
* @date 2016-11-10
*/
#ifndef BSP_LTDC_H
#define BSP_LTDC_H






//LCD LTDC重要参数集
typedef struct  
{							 
	u32 pwidth;			//LCD面板的宽度,固定参数,不随显示方向改变,如果为0,说明没有任何RGB屏接入
	u32 pheight;		//LCD面板的高度,固定参数,不随显示方向改变
	u16 hsw;			//水平同步宽度
	u16 vsw;			//垂直同步宽度
	u16 hbp;			//水平后廊
	u16 vbp;			//垂直后廊
	u16 hfp;			//水平前廊
	u16 vfp;			//垂直前廊 
	u8 activelayer;		//当前层编号:0/1	
	u8 dir;				//0,竖屏;1,横屏;
	u16 width;			//LCD宽度
	u16 height;			//LCD高度
	u32 pixsize;		//每个像素所占字节数
}_ltdc_dev; 

void bsp_LTDC_Init(void);


#endif

