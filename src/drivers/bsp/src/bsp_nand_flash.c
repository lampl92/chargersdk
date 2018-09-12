#include "bsp.h"
#include "bsp_define.h"
#include "sys_types.h"
#include "bsp_nand_flash.h"

#define NAND_RB  				 PDin(6)	//NAND Flash的闲/忙引脚

/*
	如果在IAR或KEIL的编辑器中阅读，请将编辑器的字体设置为新宋体（9号/五号），缩进的TAB设置为4。
	否则，方框处出现不对齐的问题。

	【待完善的地方】
	（1）在操作NAND Flash时，如下语句是一个死循环。如果硬件出现异常，将导致软件死机
 		while( GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_6) == 0 )

 	（2）没有增加ECC校验功能。ECC可以检查1个或2个bit错误，如果只有1个bit错误，则可以修复这个bit。如果
 		多余2个bit错误，则可能检测不到。

 	（3）正常写文件操作时，会导致重建LUT。目前，重建LUT的代码执行效率还不够高，有待完善。

	【硬件说明】
	安富莱开发板配置的NAND Flahs为海力士的HY27UF081G2A,  439板子配的为 H27U4G8F2DTR
	（1）NAND Flash的片选信号连接到CPU的FMC_NCE2，这决定了NAND Flash的地址空间为 0x70000000（见CPU的数据
		手册的FSMC章节)
	（2）有FSMC总线上有多个总线设备（如TFT、SRAM、CH374T、NOR），因此必须确保其他总线设备的片选处于禁止
		状态，否则将出现总线冲突问题 （参见本文件初始化FSMC GPIO的函数）


	【NAND Flash 结构定义】
     备用区有16x4字节，每page 2048字节，每512字节一个扇区，每个扇区对应16自己的备用区：

	 每个PAGE的逻辑结构，前面512Bx4是主数据区，后面16Bx4是备用区
	┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐┌──────┐
	│ Main area  ││ Main area  ││ Main area  ││Main area   ││ Spare area ││ Spare area ││ Spare area ││Spare area  │
	│            ││            ││            ││            ││            ││            ││            ││            │
	│   512B     ││    512B    ││    512B    ││    512B    ││    16B     ││     16B    ││     16B    ││    16B     │
	└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘└──────┘

	 每16B的备用区的逻辑结构如下:(三星推荐标准）
	┌───┐┌───┐┌──┐┌──┐┌──┐┌───┐┌───┐┌───┐┌──┐┌──┐┌──┐┌──┐┌───┐┌───┐┌───┐┌───┐┌───┐
	│  BI  ││RESER ││LSN0││LSN1││LSN2││RESER ││RESER ││RESER ││ECC0││ECC1││ECC2││ECC0││S-ECC1││S-ECC0││RESER ││RESER ││RESER │
	│      ││ VED  ││    ││    ││    ││ VED  ││ VED  ││ VED  ││    ││    ││    ││    ││      ││      ││ VED  ││ VED  ││ VED  │
	└───┘└───┘└──┘└──┘└──┘└───┘└───┘└───┘└──┘└──┘└──┘└──┘└───┘└───┘└───┘└───┘└───┘

	K9F1G08U0A 和 HY27UF081G2A 是兼容的。芯片出厂时，厂商保证芯片的第1个块是好块。如果是坏块，则在该块的第1个PAGE的第1个字节
	或者第2个PAGE（当第1个PAGE坏了无法标记为0xFF时）的第1个字节写入非0xFF值。坏块标记值是随机的，软件直接判断是否等于0xFF即可。

	注意：网上有些资料说NAND Flash厂商的默认做法是将坏块标记定在第1个PAGE的第6个字节处。这个说法是错误。坏块标记在第6个字节仅针对部分小扇区（512字节）的NAND Flash
	并不是所有的NAND Flash都是这个标准。大家在更换NAND Flash时，请仔细阅读芯片的数据手册。


	为了便于在NAND Flash 上移植Fat文件系统，我们对16B的备用区采用以下分配方案:
	┌──┐┌──┐┌──┐┌──┐┌──┐┌──┐┌──┐┌──┐┌──┐┌──┐┌───┐┌───┐┌──┐┌──┐┌──┐┌──┐
	│ BI ││USED││LBN0││LBN1││ECC0││ECC1││ECC2││ECC3││ECC4││ECC5││S-ECC1││S-ECC0││RSVD││RSVD││RSVD││RSVD│
	│    ││    ││    ││    ││    ││    ││    ││    ││    ││    ││      ││      ││    ││    ││    ││    │
	└──┘└──┘└──┘└──┘└──┘└──┘└──┘└──┘└──┘└──┘└───┘└───┘└──┘└──┘└──┘└──┘
    - BI : 坏块标志(Bad Block Identifier)。每个BLOCK的第1个PAGE或者第2个PAGE的第1个字节指示该块是否坏块。0xFF表示好块，不是0xFF表示坏块。
    - USED : 该块使用标志。0xFF表示空闲块；0xF0表示已用块。
    - LBN0 LBN1 : 逻辑块号(Logic Block No) 。从0开始编码。只在每个BLOCK的第1个PAGE有效，其它PAGE该字段固定为0xFF FF
    - ECC0 ~ ECC6 : 512B主数据区的ECC校验 （按照三星提供ECC算法，256字节对应3个字节的ECC)
    - S-ECC1 S-ECC0 : LSN0和LSN2的ECC校验
    - RSVD : 保留字节，Reserved

	【坏块管理 & 磨损平衡】
	(1) 内部全局数组s_usLUT[]按次序保存物理块号。用于物理块和逻辑块的地址映射。
	(2) 格式化时，将98%的好块用于主数据存储。剩余的2%用于备用区（坏块替换）。
	(3) 写扇区（512B)时，如果扇区内容为空，则直接写入，减少不必要的块擦除操作。有效提高NAND Flash的寿命和读写性能。
	(4) 写扇区时，如果扇区内容不为空，则从末尾开始查找一个空闲块替换掉旧块，替换并改写数据完成后，将旧块擦除，并标注为空闲，之后重建LUT。
	(5) 块复制时，充分利用NAND Flash硬件的Copy-Back功能，无需读源页到内存再写入目标页。这样可显著提高读写效率。
	(6) 磨损平衡还存在缺陷，效果不好。ECC校验暂未实现。

*/

/* 定义NAND Flash的物理地址。这个是由硬件决定的 */
#define Bank3_NAND_ADDR    0x80000000
#define Bank_NAND_ADDR     Bank3_NAND_ADDR

/* 定义操作NAND Flash用到3个宏 */
#define NAND_CMD_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | CMD_AREA)
#define NAND_ADDR_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | ADDR_AREA)
#define NAND_DATA_AREA		*(__IO uint8_t *)(Bank_NAND_ADDR | DATA_AREA)

NAND_HandleTypeDef NAND_Handler;    //NAND FLASH句柄
FMC_NAND_PCC_TimingTypeDef ComSpaceTiming, AttSpaceTiming;

/*
*********************************************************************************************************
*	函 数 名: FMC_NAND_HALInit
*	功能说明: 配置FSMC和GPIO用于NAND Flash接口。这个函数必须在读写nand flash前被调用一次。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void FMC_NAND_HALInit(void)
{
    NAND_Handler.Instance = FMC_NAND_DEVICE;
    NAND_Handler.Init.NandBank = FMC_NAND_BANK3;                        //NAND挂在BANK3上
    NAND_Handler.Init.Waitfeature = FMC_NAND_PCC_WAIT_FEATURE_DISABLE;  //关闭等待特性
    NAND_Handler.Init.MemoryDataWidth = FMC_NAND_PCC_MEM_BUS_WIDTH_8;   //8位数据宽度
    NAND_Handler.Init.EccComputation = FMC_NAND_ECC_ENABLE;             //使用ECC
    NAND_Handler.Init.ECCPageSize = FMC_NAND_ECC_PAGE_SIZE_2048BYTE;    //ECC页大小为2k
    NAND_Handler.Init.TCLRSetupTime = 0;                                //设置TCLR(tCLR=CLE到RE的延时)=(TCLR+TSET+2)*THCLK,THCLK=1/180M=5.5ns
    NAND_Handler.Init.TARSetupTime = 1;                                 //设置TAR(tAR=ALE到RE的延时)=(TAR+TSET+2)*THCLK,THCLK=1/180M=5.5n。

    ComSpaceTiming.SetupTime = 2;       //建立时间
    ComSpaceTiming.WaitSetupTime = 3;   //等待时间
    ComSpaceTiming.HoldSetupTime = 2;   //保持时间
    ComSpaceTiming.HiZSetupTime = 1;    //高阻态时间

    AttSpaceTiming.SetupTime = 2;       //建立时间
    AttSpaceTiming.WaitSetupTime = 3;   //等待时间
    AttSpaceTiming.HoldSetupTime = 2;   //保持时间
    AttSpaceTiming.HiZSetupTime = 1;    //高阻态时间

    HAL_NAND_Init(&NAND_Handler, &ComSpaceTiming, &AttSpaceTiming);
}
void HAL_NAND_MspInit(NAND_HandleTypeDef *hnand)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_FMC_CLK_ENABLE();             //使能FMC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();           //使能GPIOD时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();           //使能GPIOE时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();           //使能GPIOG时钟

    //初始化PD6 R/B引脚
    GPIO_Initure.Pin = GPIO_PIN_6;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;        //输入
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    //初始化PG9 NCE3引脚
    GPIO_Initure.Pin = GPIO_PIN_9;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //输入
    GPIO_Initure.Pull = GPIO_NOPULL;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);

    //初始化PD0,1,4,5,11,12,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);

    //初始化PE7,8,9,10
    GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);
}

/*
*********************************************************************************************************
*	函 数 名: FMC_NAND_ReadStatus
*	功能说明: 使用Read statuc 命令读NAND Flash内部状态
*	形    参:  - Address: 被擦除的快内任意地址
*	返 回 值: NAND操作状态，有如下几种值：
*             - NAND_BUSY: 内部正忙
*             - NAND_READY: 内部空闲，可以进行下步操作
*             - NAND_ERROR: 先前的命令执行失败
*********************************************************************************************************
*/
static uint32_t FMC_NAND_ReadStatus(void)
{
    uint8_t ucData = 0;
    uint32_t ulStatus = NAND_BUSY;

    /* 读状态操作 */
    NAND_CMD_AREA = NAND_CMD_STATUS;
    ucData = *(__IO uint8_t *)(Bank_NAND_ADDR);

    if ((ucData & NAND_ERROR) == NAND_ERROR)
    {
        ulStatus = NAND_ERROR;
    }
    else if ((ucData & NAND_READY) == NAND_READY)
    {
        ulStatus = NAND_READY;
    }
    else
    {
        ulStatus = NAND_BUSY;
    }

    return (ulStatus);
}

/*
*********************************************************************************************************
*	函 数 名: FMC_NAND_GetStatus
*	功能说明: 获取NAND Flash操作状态
*	形    参:  - Address: 被擦除的快内任意地址
*	返 回 值: NAND操作状态，有如下几种值：
*             - NAND_TIMEOUT_ERROR  : 超时错误
*             - NAND_READY          : 操作成功
*********************************************************************************************************
*/
static uint32_t FMC_NAND_GetStatus(void)
{
    //uint32_t ulTimeout = 0x10000;  128M,  512M
    uint32_t ulTimeout = 0x20000;
    uint32_t ulStatus = NAND_READY;

    ulStatus = FMC_NAND_ReadStatus();

    /* 等待NAND操作结束，超时后会退出 */
    while ((ulStatus != NAND_READY) &&(ulTimeout != 0x00))
    {
        ulStatus = FMC_NAND_ReadStatus();
        ulTimeout--;
    }

    if (ulTimeout == 0x00)
    {
        ulStatus =  NAND_TIMEOUT_ERROR;
    }

    /* 返回操作状态 */
    return (ulStatus);
}

/*
*********************************************************************************************************
*	函 数 名: FMC_NAND_Reset
*	功能说明: 复位NAND Flash
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static uint8_t FMC_NAND_Reset(void)
{
    NAND_CMD_AREA = NAND_CMD_RESET;

    		/* 检查操作状态 */
    if (FMC_NAND_GetStatus() == NAND_READY)
    {
        return NAND_OK;
    }

    return NAND_FAIL;
}

/*
*********************************************************************************************************
*	函 数 名: NAND_Init
*	功能说明: 初始化NAND Flash接口
*	形    参:  无
*	返 回 值: 执行结果：
*				- NAND_FAIL 表示失败
*				- NAND_OK 表示成功
*********************************************************************************************************
*/
uint8_t NAND_Init(void)
{
    uint8_t Status;

    FMC_NAND_HALInit();			/* 配置FSMC和GPIO用于NAND Flash接口 */
    FMC_NAND_Reset();			/* 通过复位命令复位NAND Flash到读状态 */
    bsp_DelayMS(100);

    return Status;
}
