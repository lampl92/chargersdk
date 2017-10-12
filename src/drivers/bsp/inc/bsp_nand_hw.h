#ifndef _BSP_NAND_HW_H
#define _BSP_NAND_HW_H
//升级说明
//V1.1 20160520
//1,新增硬件ECC支持(仅在以NAND_ECC_SECTOR_SIZE大小为单位进行读写时处理)
//2,新增NAND_Delay函数,用于等待tADL/tWHR
//3,新增NAND_WritePageConst函数,用于搜寻坏块.
//V1.2 20160525
//1,去掉NAND_SEC_SIZE宏定义，由NAND_ECC_SECTOR_SIZE替代
//2,去掉nand_dev结构体里面的secbuf指针，用不到
//////////////////////////////////////////////////////////////////////////////////

#include "stm32f4xx.h"

#define NAND_MAX_PAGE_SIZE			4096		//定义NAND FLASH的最大的PAGE大小（不包括SPARE区），默认4096字节
#define NAND_ECC_SECTOR_SIZE		512			//执行ECC计算的单元大小，默认512字节

//NAND属性结构体
typedef struct
{
    uint16_t page_totalsize;     	//每页总大小，main区和spare区总和
    uint16_t page_mainsize;      	//每页的main区大小
    uint16_t page_sparesize;     	//每页的spare区大小
    uint8_t  block_pagenum;      	//每个块包含的页数量
    uint16_t plane_blocknum;     	//每个plane包含的块数量
    uint16_t block_totalnum;     	//总的块数量
    uint16_t good_blocknum;      	//好块数量
    uint16_t valid_blocknum;     	//有效块数量(供文件系统使用的好块数量)
    uint32_t id;             		//NAND FLASH ID
    uint16_t *lut;      			   	//LUT表，用作逻辑块-物理块转换
	uint32_t ecc_hard;				//硬件计算出来的ECC值
	uint32_t ecc_hdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];//ECC硬件计算值缓冲区
	uint32_t ecc_rdbuf[NAND_MAX_PAGE_SIZE/NAND_ECC_SECTOR_SIZE];//ECC读取的值缓冲区
}nand_attriute;

extern nand_attriute nand_dev;				//nand重要参数结构体

#define NAND_RB  				 PDin(6)	//NAND Flash的闲/忙引脚



#define NAND_ADDRESS			0X80000000	//nand flash的访问地址,接NCE3,地址为:0X8000 0000
#define NAND_CMD				1<<16		//发送命令
#define NAND_ADDR				1<<17		//发送地址

//NAND FLASH命令
#define NAND_READID         	0X90    	//读ID指令
#define NAND_FEATURE			0XEF    	//设置特性指令
#define NAND_RESET          	0XFF    	//复位NAND
#define NAND_READSTA        	0X70   	 	//读状态
#define NAND_AREA_A         	0X00
#define NAND_AREA_TRUE1     	0X30
#define NAND_WRITE0        	 	0X80
#define NAND_WRITE_TURE1    	0X10
#define NAND_ERASE0        	 	0X60
#define NAND_ERASE1         	0XD0
#define NAND_MOVEDATA_CMD0  	0X00
#define NAND_MOVEDATA_CMD1  	0X35
#define NAND_MOVEDATA_CMD2  	0X85
#define NAND_MOVEDATA_CMD3  	0X10

//NAND FLASH状态
#define NSTA_READY       	   	0X40		//nand已经准备好
#define NSTA_ERROR				0X01		//nand错误
#define NSTA_TIMEOUT        	0X02		//超时
#define NSTA_ECC1BITERR       	0X03		//ECC 1bit错误
#define NSTA_ECC2BITERR       	0X04		//ECC 2bit以上错误


//NAND FLASH型号和对应的ID号
#define MT29F4G08ABADA			0XDC909556	//MT29F4G08ABADA
#define MT29F16G08ABABA			0X48002689	//MT29F16G08ABABA



uint8_t NAND_Init(void);
uint8_t NAND_ModeSet(uint8_t mode);
uint32_t NAND_ReadID(void);
uint8_t NAND_ReadStatus(void);
uint8_t NAND_WaitForReady(void);
uint8_t NAND_Reset(void);
uint8_t NAND_WaitRB(__IO uint8_t rb);
void NAND_Delay(__IO uint32_t i);
uint8_t NAND_ReadPage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
uint8_t NAND_ReadPageComp(uint32_t PageNum,uint16_t ColNum,uint32_t CmpVal,uint16_t NumByteToRead,uint16_t *NumByteEqual);
uint8_t NAND_WritePage(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite);
uint8_t NAND_WritePageConst(uint32_t PageNum,uint16_t ColNum,uint32_t cval,uint16_t NumByteToWrite);
uint8_t NAND_CopyPageWithoutWrite(uint32_t Source_PageNum,uint32_t Dest_PageNum);
uint8_t NAND_CopyPageWithWrite(uint32_t Source_PageNum,uint32_t Dest_PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToWrite);
uint8_t NAND_ReadSpare(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
uint8_t NAND_WriteSpare(uint32_t PageNum,uint16_t ColNum,uint8_t *pBuffer,uint16_t NumByteToRead);
uint8_t NAND_EraseBlock(uint32_t BlockNum);
void NAND_EraseChip(void);

uint16_t NAND_ECC_Get_OE(uint8_t oe,uint32_t eccval);
uint8_t NAND_ECC_Correction(uint8_t* data_buf,uint32_t eccrd,uint32_t ecccl);
#endif



















