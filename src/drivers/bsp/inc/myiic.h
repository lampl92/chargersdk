#ifndef _MYIIC_H
#define _MYIIC_H
#include "bsp.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F429������
//IIC��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2016/1/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////
//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//PH5����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //PH5���ģʽ
//IO����
#define IIC_SCL   PBout(6) //SCL
#define IIC_SDA   PBout(7) //SDA
#define READ_SDA  PBin(7)  //����SDA

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��
//void IIC_Start(void);				//����IIC��ʼ�ź�
//void IIC_Stop(void);	  			//����IICֹͣ�ź�
//void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
//u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
//u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
//void IIC_Ack(void);					//IIC����ACK�ź�
//void IIC_NAck(void);				//IIC������ACK�ź�

//void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 IIC_Read_One_Byte(u8 daddr,u8 addr);
void write_pca9554_2(void);
void PCA9554_init(void);
void write_pca9554_1(void);
void write_pca9554_2(void);
uint8_t read_pca9554_2(void);
#endif


