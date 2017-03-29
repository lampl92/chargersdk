#include "myiic.h"
//#include "delay.h"
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

//IIC��ʼ
IO_chip1 Chip1;
IO_chip2 Chip2;
unsigned char pca9554_1 = 0, pca9554_2 = 0;
extern void Delay_ms(unsigned long long);
uint8_t test;
void delay_us(u32 nus)
{
    uint16_t i, j;
    j = 90 * nus;
    for(i = 0; i < j; i++)
    {
        ;
    }
}
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();   //ʹ��GPIOHʱ��

    //PH4,5��ʼ������
    GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //�������
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_FAST;   //����
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    IIC_SDA = 1;
    IIC_SCL = 1;
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
    SDA_OUT();     //sda�����
    IIC_SDA = 1;
    IIC_SCL = 1;
    delay_us(4);
    IIC_SDA = 0; //START:when CLK is high,DATA change form high to low
    delay_us(4);
    IIC_SCL = 0; //ǯסI2C���ߣ�׼�����ͻ��������
}
//����IICֹͣ�ź�
void IIC_Stop(void)
{
    SDA_OUT();//sda�����
    IIC_SCL = 0;
    IIC_SDA = 0; //STOP:when CLK is high DATA change form low to high
    delay_us(4);
    IIC_SCL = 1;
    IIC_SDA = 1; //����I2C���߽����ź�
    delay_us(4);
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();      //SDA����Ϊ����
    IIC_SDA = 1;
    delay_us(1);
    IIC_SCL = 1;
    delay_us(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL = 0; //ʱ�����0
    return 0;
}
//����ACKӦ��
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}
//������ACKӦ��
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    delay_us(2);
    IIC_SCL = 1;
    delay_us(2);
    IIC_SCL = 0;
}
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL = 0; //����ʱ�ӿ�ʼ���ݴ���
    for(t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        delay_us(2);   //��TEA5767��������ʱ���Ǳ����
        IIC_SCL = 1;
        delay_us(2);
        IIC_SCL = 0;
        delay_us(2);
    }
}
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();//SDA����Ϊ����
    for(i = 0; i < 8; i++ )
    {
        IIC_SCL = 0;
        delay_us(2);
        IIC_SCL = 1;
        receive <<= 1;
        if(READ_SDA)
        {
            receive++;
        }
        delay_us(1);
    }
    if (!ack)
    {
        IIC_NAck();    //����nACK
    }
    else
    {
        IIC_Ack();    //����ACK
    }
    return receive;
}
void PCA9554_init(void)
{
    //Ƭ1ȫ�������
    IIC_Start();
    IIC_Send_Byte(0X40 + 0x00); //����������ַ01,д����
    IIC_Wait_Ack();
    IIC_Send_Byte(0x03);     //03���üĴ���
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);     //ȫ�����ó����
    IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ����
    Delay_ms(10);
//Ƭ2����������
    IIC_Start();
    IIC_Send_Byte(0X40 + 0x02); //������ַ02
    IIC_Wait_Ack();
    IIC_Send_Byte(0x03);     //�����ֽ�
    IIC_Wait_Ack();
    IIC_Send_Byte(0x3F);     //�����ֽ�
    IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ����
    Delay_ms(10);
}

void write_pca9554_1(void)
{
    uint8_t PCAP554A_date;
    PCAP554A_date = Chip1.a_select | Chip1.b_select << 1 | Chip1.c_select << 2
                    | Chip1.d_select << 3 | Chip1.cs1_select << 4 | Chip1.RESET_3G << 5 | Chip1.KEY_BT << 6 | Chip1.wifi_key << 7;
    IIC_Start();
    IIC_Send_Byte(0X40 + 0X00); //����������ַ0XA0,д����
    IIC_Wait_Ack();
    IIC_Send_Byte(0x01);     //��������
    IIC_Wait_Ack();
    IIC_Send_Byte(PCAP554A_date);     //�����ֽ�
    IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ���� b
    Delay_ms(10);
}
void write_pca9554_2(void)
{
    uint8_t PCAP554B_date;
    PCAP554B_date = Chip2.in1 | Chip2.in2 | Chip2.pause | Chip2.in6 | Chip2.in7 | Chip2.in8 | Chip2.cs_zl | Chip2.buzzer;
    IIC_Start();
    IIC_Send_Byte(0X40 + 0X02); //����������ַ0XA0,д����
    IIC_Wait_Ack();
    IIC_Send_Byte(0X01);     //�����ֽ�
    IIC_Wait_Ack();
    IIC_Send_Byte(PCAP554B_date);//�����ֽ�
    IIC_Wait_Ack();
    IIC_Stop();//����һ��ֹͣ����
    Delay_ms(10);
}
uint8_t read_pca9554_2(void)
{
    u8 temp = 0;
    IIC_Start();
    IIC_Send_Byte(0X40 + 0X02); //����������ַ0XA0,д����
    IIC_Wait_Ack();
    IIC_Send_Byte(0X00);   //���Ͷ��Ĵ�������
    IIC_Wait_Ack();

    IIC_Start();
    IIC_Send_Byte(0X40 + 0X03); //����������ַ0XA0,������
    IIC_Wait_Ack();
//  IIC_Send_Byte(0X00);   //������Ĵ�������
//  IIC_Wait_Ack();
    test = IIC_Read_Byte(0);
    IIC_Stop();//����һ��ֹͣ����
    return temp;
}

