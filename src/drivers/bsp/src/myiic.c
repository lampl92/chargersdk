#include "myiic.h"
#include "user_app.h"
unsigned char pca9554_1 = 0, pca9554_2 = 0;
extern void Delay_ms(unsigned long long);
uint8_t test;
void delay_us(u32 nus)
{
//    uint16_t i, j;
//    j = 90 * nus;
//    for(i = 0; i < j; i++)
//    {
//        ;
//    }
    bsp_DelayUS(nus);
}
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOH_CLK_ENABLE();   //使能GPIOH时钟

    //PH4,5初始化设置
    GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_FAST;   //快速
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);

    IIC_SDA = 1;
    IIC_SCL = 1;
}

//产生IIC起始信号
void IIC_Start(void)
{
    SDA_OUT();     //sda线输出
    IIC_SDA = 1;
    IIC_SCL = 1;
    bsp_DelayUS(4);
    IIC_SDA = 0; //START:when CLK is high,DATA change form high to low
    bsp_DelayUS(4);
    IIC_SCL = 0; //钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void IIC_Stop(void)
{
    SDA_OUT();//sda线输出
    IIC_SCL = 0;
    IIC_SDA = 0; //STOP:when CLK is high DATA change form low to high
    bsp_DelayUS(4);
    IIC_SCL = 1;
    IIC_SDA = 1; //发送I2C总线结束信号
    bsp_DelayUS(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;
    SDA_IN();      //SDA设置为输入
    IIC_SDA = 1;
    bsp_DelayUS(1);
    IIC_SCL = 1;
    bsp_DelayUS(1);
    while(READ_SDA)
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_SCL = 0; //时钟输出0
    return 0;
}
//产生ACK应答
void IIC_Ack(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 0;
    bsp_DelayUS(2);
    IIC_SCL = 1;
    bsp_DelayUS(2);
    IIC_SCL = 0;
}
//不产生ACK应答
void IIC_NAck(void)
{
    IIC_SCL = 0;
    SDA_OUT();
    IIC_SDA = 1;
    bsp_DelayUS(2);
    IIC_SCL = 1;
    bsp_DelayUS(2);
    IIC_SCL = 0;
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();
    IIC_SCL = 0; //拉低时钟开始数据传输
    for(t = 0; t < 8; t++)
    {
        IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        bsp_DelayUS(2);   //对TEA5767这三个延时都是必须的
        IIC_SCL = 1;
        bsp_DelayUS(2);
        IIC_SCL = 0;
        bsp_DelayUS(2);
    }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 IIC_Read_Byte(unsigned char ack)
{
    unsigned char i, receive = 0;
    SDA_IN();//SDA设置为输入
    for(i = 0; i < 8; i++ )
    {
        IIC_SCL = 0;
        bsp_DelayUS(2);
        IIC_SCL = 1;
        receive <<= 1;
        if(READ_SDA)
        {
            receive++;
        }
        bsp_DelayUS(1);
    }
    if (!ack)
    {
        IIC_NAck();    //发送nACK
    }
    else
    {
        IIC_Ack();    //发送ACK
    }
    return receive;
}
void PCA9554_init(void)
{
    //片1全部是输出
    IIC_Start();
    IIC_Send_Byte(0X40 + 0x00); //发送器件地址01,写数据
    IIC_Wait_Ack();
    IIC_Send_Byte(0x03);     //03配置寄存器
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);     //全部配置成输出
    IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件
    bsp_DelayMS(10);
//片2有输入和输出
    IIC_Start();
    IIC_Send_Byte(0X40 + 0x02); //器件地址02
    IIC_Wait_Ack();
    IIC_Send_Byte(0x03);     //发送字节
    IIC_Wait_Ack();
    IIC_Send_Byte(0x3F);     //发送字节
    IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件
    bsp_DelayMS(10);
}

void write_pca9554_1(void)
{
    uint8_t PCAP554A_date;
    PCAP554A_date = Chip1.a_select | Chip1.b_select << 1 | Chip1.c_select << 2
                    | Chip1.d_select << 3 | Chip1.cs1_select << 4 | Chip1.RESET_3G << 5 | Chip1.KEY_BT << 6 | Chip1.GPRS_key << 7;
    IIC_Start();
    IIC_Send_Byte(0X40 + 0X00); //发送器件地址0XA0,写数据
    IIC_Wait_Ack();
    IIC_Send_Byte(0x01);     //发送命令
    IIC_Wait_Ack();
    IIC_Send_Byte(PCAP554A_date);     //发送字节
    IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件 b
    bsp_DelayUS(10);
}
void write_pca9554_2(void)
{
    uint8_t PCAP554B_date;
    PCAP554B_date = Chip2.cs_zl<<6 | Chip2.buzzer<<7;
    IIC_Start();
    IIC_Send_Byte(0X40 + 0X02); //发送器件地址0XA0,写数据
    IIC_Wait_Ack();
    IIC_Send_Byte(0X01);     //发送字节
    IIC_Wait_Ack();
    IIC_Send_Byte(PCAP554B_date);//发送字节
    IIC_Wait_Ack();
    IIC_Stop();//产生一个停止条件
    bsp_DelayUS(10);
}
uint8_t read_pca9554_2(void)
{
    u8 temp = 0;
    IIC_Start();
    IIC_Send_Byte(0X40 + 0X02); //发送器件地址0XA0,写数据
    IIC_Wait_Ack();
    IIC_Send_Byte(0X00);   //发送读寄存器命令
    IIC_Wait_Ack();

    IIC_Start();
    IIC_Send_Byte(0X40 + 0X03); //发送器件地址0XA0,读数据
    IIC_Wait_Ack();
//  IIC_Send_Byte(0X00);   //读输入寄存器命令
//  IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);
    IIC_Stop();//产生一个停止条件
    return temp;
}

