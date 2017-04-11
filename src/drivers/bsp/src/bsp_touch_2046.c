#include "bsp.h"
#include "stdlib.h"
#include "math.h"
extern IO_chip2 Chip2;

u8 CMD_RDX = 0XD0;
u8 CMD_RDY = 0X90;
//SPI写数据
//向触摸屏IC写入1byte数据
//num:要写入的数据
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
        bsp_DelayUS(5);
        TCLK = 0;
       bsp_DelayUS(10);
        TCLK = 1;   //上升沿有效
        bsp_DelayUS(5);
    }
}
//SPI读数据
//从触摸屏IC读取adc值
//CMD:指令
//返回值:读到的数据
u16 TP_Read_AD(u8 CMD)
{
    u8 count = 0;
    u16 Num = 0;
    TCLK = 0;   //先拉低时钟
    TDIN = 0;   //拉低数据线
    TCS = 0;    //选中触摸屏IC
    TP_Write_Byte(CMD);//发送命令字
    bsp_DelayUS(200);//ADS7846的转换时间最长为6us
    TCLK = 0;
    bsp_DelayUS(1);
    TCLK = 1;   //给1个时钟，清除BUSY
    bsp_DelayUS(1);
    TCLK = 0;
    for(count = 0; count < 16; count++) //读出16位数据,只有高12位有效
    {
        Num <<= 1;
        TCLK = 0; //下降沿有效
        bsp_DelayUS(1);
        TCLK = 1;
        if(DOUT)
        {
            Num |= 1;
        }
    }
    Num >>= 4;  //只有高12位有效.
    TCS = 1;    //释放片选
    return(Num);
}
//读取一个坐标值(x或者y)
//连续读取READ_TIMES次数据,对这些数据升序排列,
//然后去掉最低和最高LOST_VAL个数,取平均值
//xy:指令（CMD_RDX/CMD_RDY）
//返回值:读到的数据
#define READ_TIMES 5    //读取次数
#define LOST_VAL 1      //丢弃值
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
    for(i = 0; i < READ_TIMES - 1; i++) //排序
    {
        for(j = i + 1; j < READ_TIMES; j++)
        {
            if(buf[i] > buf[j]) //升序排列
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
//读取x,y坐标
//最小值不能少于100.
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
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
        return 0;    //读数失败
    }
    *x = xtemp;
    *y = ytemp;
    pressure = (xTPr * (float)xtemp) / 4096.0f * (4096.0f / (float)z1 - 1) - yTPr * (1 - (float)ytemp / 4096.0f);
    if(pressure < 400 || pressure > 800)
    {
        return 0;
    }
    else
    {
        return 1;    //读数成功
    }
}
//连续2次读取触摸屏IC,且这两次的偏差不能超过
//ERR_RANGE,满足条件,则认为读数正确,否则读数错误.
//该函数能大大提高准确度
//x,y:读取到的坐标值
//返回值:0,失败;1,成功。
#define ERR_RANGE 100 //误差范围
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
    if(((x2 <= x1 && x1 < x2 + ERR_RANGE) || (x1 <= x2 && x2 < x1 + ERR_RANGE)) //前后两次采样在+-50内
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
//触摸按键扫描
//tp:0,屏幕坐标;1,物理坐标(校准等特殊场合用)
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
void TP_Scan(u16 *adc_x, u16 *adc_y)
{
    u8 tp_pressed;
    u8 invalid_count = 0;
    tp_pressed = PEN ? 0 : 1;
    //      1, 触摸屏按下
    //      0, 触摸屏释放
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

//保存校准参数
void TP_Save_Adjdata(void)
{

}
//得到保存在EEPROM里面的校准值
//返回值：1，成功获取数据
//        0，获取失败，要重新校准
u8 TP_Get_Adjdata(void)
{

    return 0;
}

//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
u8 bsp_Touch_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOD_CLK_ENABLE();           //开启GPIOH时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();           //开启GPIOI时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();           //开启GPIOG时钟


    //PH6   T_SCK
    GPIO_Initure.Pin = GPIO_PIN_13 |GPIO_PIN_14;          //PH6
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);    //初始化

    //PI3,8   T_MOSI/T_CS
    GPIO_Initure.Pin = GPIO_PIN_3;          //PH7
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //ÊäÈë
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);    //³õÊ¼»¯

    GPIO_Initure.Pin = GPIO_PIN_12;          //PH7
    GPIO_Initure.Mode = GPIO_MODE_INPUT;    //ÊäÈë
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);    //³õÊ¼»¯

    //PH7  T_PEN
    GPIO_Initure.Pin = GPIO_PIN_9;          //PH6
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
    GPIO_Initure.Pull = GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;   //高速
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);    //初始化
    return 1;
}

/******************LIS2DH12驱动***************************/
u16 Read_Lis2ds12(u8 CMD)
{
    u8 count = 0;
    u8 Num = 0;
    TCLK = 0;   //先拉低时钟
    TDIN = 0;   //拉低数据线
    Chip2.cs_zl = 0;    //选中重力传感器片选
    write_pca9554_2();
    TP_Write_Byte(CMD);//发送命令字

    //TCLK = 0;
    //bsp_DelayUS(1);
   // TCLK = 1;   //给1个时钟，清除BUSY
   // bsp_DelayUS(1);
    //TCLK = 0;
    for(count = 0; count < 8; count++) //读出8位数据
    {
        Num <<= 1;
        TCLK = 0; //下降沿有效
        bsp_DelayUS(5);
        TCLK = 1;
        bsp_DelayUS(5);
        if(DOUT)
        {
            Num |= 1;
        }
    }
     Chip2.cs_zl = 1;    //释放片选
    write_pca9554_2();
    return(Num);
}

/******************END LIS2DH12驱动***************************/







