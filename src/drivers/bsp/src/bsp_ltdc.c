/**
* @file bsp_ltdc.c
* @brief 
* @author rgw
* @version v1.0
* @date 2016-11-10
*/
#include "bsp.h"

/* 硬件自定义参数 */
#define LCD_PIXFORMAT			        LTDC_PIXEL_FORMAT_RGB565


LTDC_HandleTypeDef LTDC_Handler;

//根据不同的颜色格式,定义帧缓存数组
#if LCD_PIXFORMAT == LTDC_PIXEL_FORMAT_ARGB8888||LCD_PIXFORMAT == LTDC_PIXEL_FORMAT_RGB888
	u32 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
#else
	u16 ltdc_lcd_framebuf[1280][800] __attribute__((at(LCD_FRAME_BUF_ADDR)));	//定义最大屏分辨率时,LCD所需的帧缓存数组大小
#endif

u32 *ltdc_framebuf[2];					//LTDC LCD帧缓存数组指针,必须指向对应大小的内存区域
_ltdc_dev lcdltdc;						//管理LCD LTDC的重要参数


//LTDC,层颜窗口设置,窗口以LCD面板坐标系为基准
//注意:此函数必须在LTDC_Layer_Parameter_Config之后再设置.
//layerx:层值,0/1.
//sx,sy:起始坐标
//width,height:宽度和高度
void LTDC_Layer_Window_Config(u8 layerx,u16 sx,u16 sy,u16 width,u16 height)
{
    HAL_LTDC_SetWindowPosition(&LTDC_Handler,sx,sy,layerx);  //设置窗口的位置
    HAL_LTDC_SetWindowSize(&LTDC_Handler,width,height,layerx);//设置窗口大小    
}

//LTDC,基本参数设置.
//注意:此函数,必须在LTDC_Layer_Window_Config之前设置.
//layerx:层值,0/1.
//bufaddr:层颜色帧缓存起始地址
//pixformat:颜色格式.0,ARGB8888;1,RGB888;2,RGB565;3,ARGB1555;4,ARGB4444;5,L8;6;AL44;7;AL88
//alpha:层颜色Alpha值,0,全透明;255,不透明
//alpha0:默认颜色Alpha值,0,全透明;255,不透明
//bfac1:混合系数1,4(100),恒定的Alpha;6(101),像素Alpha*恒定Alpha
//bfac2:混合系数2,5(101),恒定的Alpha;7(111),像素Alpha*恒定Alpha
//bkcolor:层默认颜色,32位,低24位有效,RGB888格式
//返回值:无
void LTDC_Layer_Parameter_Config(u8 layerx,u32 bufaddr,u8 pixformat,u8 alpha,u8 alpha0,u8 bfac1,u8 bfac2,u32 bkcolor)
{
	LTDC_LayerCfgTypeDef pLayerCfg;
	
	pLayerCfg.WindowX0=0;                       //窗口起始X坐标
	pLayerCfg.WindowY0=0;                       //窗口起始Y坐标
	pLayerCfg.WindowX1=lcdltdc.pwidth;          //窗口终止X坐标
	pLayerCfg.WindowY1=lcdltdc.pheight;         //窗口终止Y坐标
	pLayerCfg.PixelFormat=pixformat;		    //像素格式
	pLayerCfg.Alpha=alpha;				        //Alpha值设置，0~255,255为完全不透明
	pLayerCfg.Alpha0=alpha0;			        //默认Alpha值
	pLayerCfg.BlendingFactor1=(u32)bfac1<<8;    //设置层混合系数
	pLayerCfg.BlendingFactor2=(u32)bfac2<<8;	//设置层混合系数
	pLayerCfg.FBStartAdress=bufaddr;	        //设置层颜色帧缓存起始地址
	pLayerCfg.ImageWidth=lcdltdc.pwidth;        //设置颜色帧缓冲区的宽度    
	pLayerCfg.ImageHeight=lcdltdc.pheight;      //设置颜色帧缓冲区的高度
	pLayerCfg.Backcolor.Red=(u8)(bkcolor&0X00FF0000)>>16;   //背景颜色红色部分
	pLayerCfg.Backcolor.Green=(u8)(bkcolor&0X0000FF00)>>8;  //背景颜色绿色部分
	pLayerCfg.Backcolor.Blue=(u8)bkcolor&0X000000FF;        //背景颜色蓝色部分
	HAL_LTDC_ConfigLayer(&LTDC_Handler,&pLayerCfg,layerx);   //设置所选中的层
}  


void bsp_LTDC_Init(void)
{
    //LTDC配置
    LTDC_Handler.Instance=LTDC;
    LTDC_Handler.Init.HSPolarity=LTDC_HSPOLARITY_AL;         //水平同步极性
    LTDC_Handler.Init.VSPolarity=LTDC_VSPOLARITY_AL;         //垂直同步极性
    LTDC_Handler.Init.DEPolarity=LTDC_DEPOLARITY_AL;         //数据使能极性
    LTDC_Handler.Init.PCPolarity=LTDC_PCPOLARITY_IPC;        //像素时钟极性
    LTDC_Handler.Init.HorizontalSync=lcdltdc.hsw-1;          //水平同步宽度
    LTDC_Handler.Init.VerticalSync=lcdltdc.vsw-1;            //垂直同步宽度
    LTDC_Handler.Init.AccumulatedHBP=lcdltdc.hsw+lcdltdc.hbp-1; //水平同步后沿宽度
    LTDC_Handler.Init.AccumulatedVBP=lcdltdc.vsw+lcdltdc.vbp-1; //垂直同步后沿高度
    LTDC_Handler.Init.AccumulatedActiveW=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth-1;//有效宽度
    LTDC_Handler.Init.AccumulatedActiveH=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight-1;//有效高度
    LTDC_Handler.Init.TotalWidth=lcdltdc.hsw+lcdltdc.hbp+lcdltdc.pwidth+lcdltdc.hfp-1;   //总宽度
    LTDC_Handler.Init.TotalHeigh=lcdltdc.vsw+lcdltdc.vbp+lcdltdc.pheight+lcdltdc.vfp-1;  //总高度
    LTDC_Handler.Init.Backcolor.Red=0;           //屏幕背景层红色部分
    LTDC_Handler.Init.Backcolor.Green=0;         //屏幕背景层绿色部分
    LTDC_Handler.Init.Backcolor.Blue=0;          //屏幕背景色蓝色部分
    HAL_LTDC_Init(&LTDC_Handler);
 	
	//层配置
	LTDC_Layer_Parameter_Config(0,(u32)ltdc_framebuf[0],LCD_PIXFORMAT,255,0,6,7,0X000000);//层参数配置
	LTDC_Layer_Window_Config(0,0,0,lcdltdc.pwidth,lcdltdc.pheight);	//层窗口配置,以LCD面板坐标系为基准,不要随便修改!
	
 	LTDC_Display_Dir(0);			//默认竖屏
	LTDC_Select_Layer(0); 			//选择第1层
    LCD_LED=1;         		        //点亮背光
    LTDC_Clear(0XFFFFFFFF);			//清屏

}

/* LTDC信号线   对应IO                             说明  */
/*-----------------------------------------------------------*/
/* LCD_CLK      PG7------ 像素时钟输出  */
/* LCD_HSYNC    PI10----- 水平同步  */
/* LCD_VSYNC    PI9------ 垂直同步  */
/* LCD_DE       PF10 ---- 数据使能  */
/* LCD_R[7:3]   PG6、PH12、PH11、PH10、PH9         红色数据线，LCD_R[2:0]未用到  */
/* LCD_G[7:2]   PI2、PI1、PI0、PH15、PH14、PH13    绿色数据线，LCD_G[1:0]未用到  */
/* LCD_B[7:3]   PI7、PI6、PI5、PI4、PG11           蓝色数据线，LCD_B[2:0]未用到  */

//LTDC底层IO初始化和时钟使能
//此函数会被HAL_LTDC_Init()调用
//hltdc:LTDC句柄
void HAL_LTDC_MspInit(LTDC_HandleTypeDef* hltdc)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_LTDC_CLK_ENABLE();                //使能LTDC时钟
    __HAL_RCC_DMA2D_CLK_ENABLE();               //使能DMA2D时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();               //使能GPIOB时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();               //使能GPIOF时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();               //使能GPIOG时钟
    __HAL_RCC_GPIOH_CLK_ENABLE();               //使能GPIOH时钟
    __HAL_RCC_GPIOI_CLK_ENABLE();               //使能GPIOI时钟
    
    //初始化PB5，背光引脚
    GPIO_Initure.Pin=GPIO_PIN_5;                //PB5推挽输出，控制背光
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;      //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;              //上拉        
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
    
    //初始化PF10
    GPIO_Initure.Pin=GPIO_PIN_10; 
    GPIO_Initure.Mode=GPIO_MODE_AF_PP;          //复用
    GPIO_Initure.Pull=GPIO_NOPULL;              
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;         //高速
    GPIO_Initure.Alternate=GPIO_AF14_LTDC;      //复用为LTDC
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
    
    //初始化PG6,7,11
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_11;
    HAL_GPIO_Init(GPIOG,&GPIO_Initure);
    
    //初始化PH9,10,11,12,13,14,15
    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|\
                     GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    //初始化PI0,1,2,4,5,6,7,9,10
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5|\
                     GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_9|GPIO_PIN_10;
    HAL_GPIO_Init(GPIOI,&GPIO_Initure); 
}

