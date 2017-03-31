#include "myiic.h"
//#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F429开发板
//IIC驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

//IIC初始
IO_chip1 Chip1;
IO_chip2 Chip2;
unsigned char pca9554_1=0,pca9554_2=0;
extern void Delay_ms(unsigned long long);
uint8_t test;
void delay_us(u32 nus)
{
	uint16_t i,j;
	j=90*nus;
for(i=0;i<j;i++)
	{
	;
	}
}
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();   //Ê¹ÄÜGPIOHÊ±ÖÓ

    //PH4,5³õÊ¼»¯ÉèÖÃ
    GPIO_Initure.Pin=GPIO_PIN_6|GPIO_PIN_7;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //ÍÆÍìÊä³ö
    GPIO_Initure.Pull=GPIO_PULLUP;          //ÉÏÀ­
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //¿ìËÙ
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    IIC_SDA=1;
    IIC_SCL=1;
}

//²úÉúIICÆðÊ¼ÐÅºÅ
void IIC_Start(void)
{
	SDA_OUT();     //sdaÏßÊä³ö
	IIC_SDA=1;
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low
	delay_us(4);
	IIC_SCL=0;//Ç¯×¡I2C×ÜÏß£¬×¼±¸·¢ËÍ»ò½ÓÊÕÊý¾Ý
}
//²úÉúIICÍ£Ö¹ÐÅºÅ
void IIC_Stop(void)
{
	SDA_OUT();//sdaÏßÊä³ö
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1;
	IIC_SDA=1;//·¢ËÍI2C×ÜÏß½áÊøÐÅºÅ
	delay_us(4);
}
//µÈ´ýÓ¦´ðÐÅºÅµ½À´
//·µ»ØÖµ£º1£¬½ÓÊÕÓ¦´ðÊ§°Ü
//        0£¬½ÓÊÕÓ¦´ð³É¹¦
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDAÉèÖÃÎªÊäÈë
	IIC_SDA=1;delay_us(1);
	IIC_SCL=1;delay_us(1);
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//Ê±ÖÓÊä³ö0
	return 0;
}
//²úÉúACKÓ¦´ð
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//²»²úÉúACKÓ¦´ð
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}
//IIC·¢ËÍÒ»¸ö×Ö½Ú
//·µ»Ø´Ó»úÓÐÎÞÓ¦´ð
//1£¬ÓÐÓ¦´ð
//0£¬ÎÞÓ¦´ð
void IIC_Send_Byte(u8 txd)
{
    u8 t;
	SDA_OUT();
    IIC_SCL=0;//À­µÍÊ±ÖÓ¿ªÊ¼Êý¾Ý´«Êä
    for(t=0;t<8;t++)
    {
        IIC_SDA=(txd&0x80)>>7;
        txd<<=1;
		delay_us(2);   //¶ÔTEA5767ÕâÈý¸öÑÓÊ±¶¼ÊÇ±ØÐëµÄ
		IIC_SCL=1;
		delay_us(2);
		IIC_SCL=0;
		delay_us(2);
    }
}
//¶Á1¸ö×Ö½Ú£¬ack=1Ê±£¬·¢ËÍACK£¬ack=0£¬·¢ËÍnACK
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDAÉèÖÃÎªÊäÈë
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0;
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;
		delay_us(1);
    }
    if (!ack)
        IIC_NAck();//·¢ËÍnACK
    else
        IIC_Ack(); //·¢ËÍACK
    return receive;
}
void PCA9554_init(void)
{
	//Æ¬1È«²¿ÊÇÊä³ö
	IIC_Start();
  IIC_Send_Byte(0X40+0x00);   //·¢ËÍÆ÷¼þµØÖ·01,Ð´Êý¾Ý
	IIC_Wait_Ack();
	IIC_Send_Byte(0x03);     //03ÅäÖÃ¼Ä´æÆ÷
	IIC_Wait_Ack();
	IIC_Send_Byte(0x00);     //È«²¿ÅäÖÃ³ÉÊä³ö
	IIC_Wait_Ack();
   IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
	Delay_ms(10);
//Æ¬2ÓÐÊäÈëºÍÊä³ö
	IIC_Start();
  IIC_Send_Byte(0X40+0x02);   //Æ÷¼þµØÖ·02
	IIC_Wait_Ack();
	IIC_Send_Byte(0x03);     //·¢ËÍ×Ö½Ú
	IIC_Wait_Ack();
	IIC_Send_Byte(0x3F);     //·¢ËÍ×Ö½Ú
	IIC_Wait_Ack();
   IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
	Delay_ms(10);
}

void write_pca9554_1(void)
{
	uint8_t PCAP554A_date;
  PCAP554A_date=Chip1.a_select|Chip1.b_select<<1|Chip1.c_select<<2
	|Chip1.d_select<<3|Chip1.cs1_select<<4|Chip1.RESET_3G<<5|Chip1.KEY_BT<<6|Chip1.wifi_key<<7;
  IIC_Start();
  IIC_Send_Byte(0X40+0X00);   //·¢ËÍÆ÷¼þµØÖ·0XA0,Ð´Êý¾Ý
	IIC_Wait_Ack();
	IIC_Send_Byte(0x01);     //·¢ËÍÃüÁî
	IIC_Wait_Ack();
  IIC_Send_Byte(PCAP554A_date);     //·¢ËÍ×Ö½Ú
	IIC_Wait_Ack();
  IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ b
	Delay_ms(10);
}
void write_pca9554_2(void)
{
	uint8_t PCAP554B_date;
  PCAP554B_date=Chip2.in1|Chip2.in2|Chip2.pause|Chip2.in6|Chip2.in7|Chip2.in8|Chip2.cs_zl|Chip2.buzzer;
  IIC_Start();
  IIC_Send_Byte(0X40+0X02);   //·¢ËÍÆ÷¼þµØÖ·0XA0,Ð´Êý¾Ý
	IIC_Wait_Ack();
	IIC_Send_Byte(0X01);     //·¢ËÍ×Ö½Ú
	IIC_Wait_Ack();
  IIC_Send_Byte(PCAP554B_date);//·¢ËÍ×Ö½Ú
	IIC_Wait_Ack();
  IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
	Delay_ms(10);
}
uint8_t read_pca9554_2(void)
{
  u8 temp=0;
  IIC_Start();
  IIC_Send_Byte(0X40+0X02);   //·¢ËÍÆ÷¼þµØÖ·0XA0,Ð´Êý¾Ý
	IIC_Wait_Ack();
  IIC_Send_Byte(0X00);   //·¢ËÍ¶Á¼Ä´æÆ÷ÃüÁî
	IIC_Wait_Ack();

	IIC_Start();
	IIC_Send_Byte(0X40+0X03);   //·¢ËÍÆ÷¼þµØÖ·0XA0,¶ÁÊý¾Ý
	IIC_Wait_Ack();
//	IIC_Send_Byte(0X00);   //¶ÁÊäÈë¼Ä´æÆ÷ÃüÁî
//	IIC_Wait_Ack();
  test=IIC_Read_Byte(0);
  IIC_Stop();//²úÉúÒ»¸öÍ£Ö¹Ìõ¼þ
	return temp;
}

