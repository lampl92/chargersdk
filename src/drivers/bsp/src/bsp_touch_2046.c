#include "bsp.h"
#include "stdlib.h"
#include "math.h"

u8 CMD_RDX = 0XD0;
u8 CMD_RDY = 0X90;
//SPIд����
//������ICд��1byte����
//num:Ҫд�������
void TP_Write_Byte(u8 num)
{
    u8 count = 0;
    for(count = 0; count < 8; count++)
    {
        if(num & 0x80)
        {
            TDIN = 1;
        }
        else
        {
            TDIN = 0;
        }
        num <<= 1;
        TCLK = 0;
        bsp_DelayUS(5);
        TCLK = 1;   //��������Ч
    }
}
//SPI������
//�Ӵ�����IC��ȡadcֵ
//CMD:ָ��
//����ֵ:����������
u16 TP_Read_AD(u8 CMD)
{
    u8 count = 0;
    u16 Num = 0;
    TCLK = 0;   //������ʱ��
    TDIN = 0;   //����������
    TCS = 0;    //ѡ�д�����IC
    TP_Write_Byte(CMD);//����������
    bsp_DelayUS(200);//ADS7846��ת��ʱ���Ϊ6us
    TCLK = 0;
    bsp_DelayUS(1);
    TCLK = 1;   //��1��ʱ�ӣ����BUSY
    bsp_DelayUS(1);
    TCLK = 0;
    for(count = 0; count < 16; count++) //����16λ����,ֻ�и�12λ��Ч
    {
        Num <<= 1;
        TCLK = 0; //�½�����Ч
        bsp_DelayUS(1);
        TCLK = 1;
        if(DOUT)
        {
            Num |= 1;
        }
    }
    Num >>= 4;  //ֻ�и�12λ��Ч.
    TCS = 1;    //�ͷ�Ƭѡ
    return(Num);
}
//��ȡһ������ֵ(x����y)
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ
//xy:ָ�CMD_RDX/CMD_RDY��
//����ֵ:����������
#define READ_TIMES 5    //��ȡ����
#define LOST_VAL 1      //����ֵ
u16 TP_Read_XOY(u8 xy)
{
    u16 i, j;
    u16 buf[READ_TIMES];
    u16 sum = 0;
    u16 temp;
    for(i = 0; i < READ_TIMES; i++)
    {
        buf[i] = TP_Read_AD(xy);
    }
    for(i = 0; i < READ_TIMES - 1; i++) //����
    {
        for(j = i + 1; j < READ_TIMES; j++)
        {
            if(buf[i] > buf[j]) //��������
            {
                temp = buf[i];
                buf[i] = buf[j];
                buf[j] = temp;
            }
        }
    }
    sum = 0;
    for(i = LOST_VAL; i < READ_TIMES - LOST_VAL; i++)
    {
        sum += buf[i];
    }
    temp = sum / (READ_TIMES - 2 * LOST_VAL);
    return temp;
}
//��ȡx,y����
//��Сֵ��������100.
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
u8 TP_Read_XY(u16 *x, u16 *y)
{
    u16 xtemp, ytemp, z1;
    float pressure;
    float xTPr = 738, yTPr = 222;
    z1 = TP_Read_XOY(0xB4);
    xtemp = TP_Read_XOY(CMD_RDX);
    ytemp = TP_Read_XOY(CMD_RDY);

    if(xtemp < 100 || ytemp < 100)
    {
        return 0;    //����ʧ��
    }
    *x = xtemp;
    *y = ytemp;
//    pressure = (xTPr * (float)xtemp) / 4096.0f * (4096.0f / (float)z1 - 1) - yTPr * (1 - (float)ytemp / 4096.0f);
//    if(pressure < 400 || pressure > 800)
//    {
//        return 0;
//    }
//    else
    {
        return 1;    //�����ɹ�
    }
}
//����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
//ERR_RANGE,��������,����Ϊ������ȷ,�����������.
//�ú����ܴ�����׼ȷ��
//x,y:��ȡ��������ֵ
//����ֵ:0,ʧ��;1,�ɹ���
#define ERR_RANGE 100 //��Χ
u8 TP_Read_XY2(u16 *x, u16 *y)
{
    u16 x1, y1;
    u16 x2, y2;
    u8 flag;
    flag = TP_Read_XY(&x1, &y1);
    if(flag == 0)
    {
        return(0);
    }
    flag = TP_Read_XY(&x2, &y2);
    if(flag == 0)
    {
        return(0);
    }
    if(((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //ǰ�����β�����+-50��
            && ((y2 <= y1 && y1 < y2 + ERR_RANGE) || (y1 <= y2 && y2 < y1 + ERR_RANGE)))
    {
        *x = (x1 + x2) / 2;
        *y = (y1 + y2) / 2;
        return 1;
    }
    else
    {
        return 0;
    }
}

u8 TP_Read_Pressure(float *pressure)
{
    u16 x, y, z1;
    float xTPr = 738, yTPr = 222;
    z1 = TP_Read_XOY(0xB4);
    TP_Read_XY2(&x, &y);
    *pressure = (xTPr * (float)x) / 4096.0f * (4096.0f / (float)z1 - 1) - yTPr * (1 - (float)y / 4096.0f);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
//��������ɨ��
//tp:0,��Ļ����;1,��������(У׼�����ⳡ����)
//����ֵ:��ǰ����״̬.
//0,�����޴���;1,�����д���
void TP_Scan(u16 *adc_x, u16 *adc_y)
{
    u8 tp_pressed;
    u8 invalid_count = 0;
    tp_pressed = PEN ? 0 : 1;
    //      1, ����������
    //      0, �������ͷ�
    if(tp_pressed == 1)
    {
        while(TP_Read_XY2(adc_x, adc_y) != 1 && invalid_count < 20)
        {
            invalid_count++;
        }
        if(invalid_count >= 20)
        {
            *adc_x = 0xffff;
            *adc_y = 0xffff;
        }

    }
    else
    {
        *adc_x = 0xffff;
        *adc_y = 0xffff;
    }
}

//����У׼����
void TP_Save_Adjdata(void)
{

}
//�õ�������EEPROM�����У׼ֵ
//����ֵ��1���ɹ���ȡ����
//        0����ȡʧ�ܣ�Ҫ����У׼
u8 TP_Get_Adjdata(void)
{

    return 0;
}

//��������ʼ��
//����ֵ:0,û�н���У׼
//       1,���й�У׼
u8 bsp_Touch_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;


    __HAL_RCC_GPIOH_CLK_ENABLE();           //����GPIOHʱ��
    __HAL_RCC_GPIOI_CLK_ENABLE();           //����GPIOIʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();           //����GPIOGʱ��

    //PH6   T_SCK
    GPIO_Initure.Pin = GPIO_PIN_6;          //PH6
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //�������
    GPIO_Initure.Pull = GPIO_PULLUP;        //����
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //����
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);    //��ʼ��

    //PI3,8   T_MOSI/T_CS
    GPIO_Initure.Pin = GPIO_PIN_3 | GPIO_PIN_8; //PI3,8
    HAL_GPIO_Init(GPIOI, &GPIO_Initure);    //��ʼ��

    //PH7  T_PEN
    GPIO_Initure.Pin = GPIO_PIN_7;          //PH7
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //����
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);    //��ʼ��

    //PG3  TMISO
    GPIO_Initure.Pin = GPIO_PIN_3;          //PG3
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);    //��ʼ��

    return 1;
}









